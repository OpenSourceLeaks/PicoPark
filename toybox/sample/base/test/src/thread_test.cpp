/*!
 * 
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#include "fwd.h"
#include "thread_test.h"

#include <base/thread/tb_thread_group.h>
#include <base/thread/tb_thread_pool.h>
#include <base/thread/tb_thread_pool_func_job.h>
#include <base/system/tb_processor.h>
#include <base/util/tb_bind.h>

using namespace toybox;

namespace
{

TbSint32 s_Val = 0;
TbAtomic<TbSint32> s_AtomicVal = 0;
TbThreadUpdateResult runAdd()
{
    for( TbSint32 i = 0; i < 99999; ++i ) {
        ++s_Val;
        ++s_AtomicVal;
    }
    TbPrintf("runAdd:cpu=%d\n",TbProcessor::GetCurrentId());
    return TB_THREAD_UPDATE_RESULT_FINISH;
}

TbThreadUpdateResult runSub()
{
    for( TbSint32 i = 0; i < 100000; ++i ) {
        --s_Val;
        --s_AtomicVal;
    }
    TbPrintf("runSub:cpu=%d\n",TbProcessor::GetCurrentId());
    return TB_THREAD_UPDATE_RESULT_FINISH;
}

void execJob1(TbUint32 id)
{
    for( TbSint32 i = 0; i < 1000000; ++i ) {
        // Waitのためだけの処理
        ++s_Val;
        ++s_AtomicVal;
    }
    TbPrintf("Exec Job1(ID=%d,Processor=%d)\n",id,TbProcessor::GetCurrentId());
}

void execJob2(TbUint32 id)
{
    for( TbSint32 i = 0; i < 1000000; ++i ) {
        // Waitのためだけの処理
        --s_Val;
        --s_AtomicVal;
    }
    TbPrintf("Exec Job2(ID=%d,Processor=%d)\n",id,TbProcessor::GetCurrentId());
}

}

/*!
 * スレッドテスト
 * @author Miyake Shunsuke
 * @since 2012.12.8
 */
void TestThread()
{
    // スレッド作成
    TbThreadGroup threadGroup(10);
    TbThreadGroup::ThreadHandle thread[8];
    thread[0] = threadGroup.CreateThread("add1",runAdd);
    thread[1] = threadGroup.CreateThread("add2",runAdd);
    thread[2] = threadGroup.CreateThread("add3",runAdd);
    thread[3] = threadGroup.CreateThread("add4",runAdd);
    thread[4] = threadGroup.CreateThread("sub1",runSub);
    thread[5] = threadGroup.CreateThread("sub2",runSub);
    thread[6] = threadGroup.CreateThread("sub3",runSub);
    thread[7] = threadGroup.CreateThread("sub4",runSub);
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(thread); ++i ) {
        thread[i].SetProcessor(1);
    }
    threadGroup.StartAll();

    // スレッド終了
    threadGroup.JoinAll();
    
    TbPrintf("s_Val=%d\n",s_Val);
    TbPrintf("s_AtomicVal=%d\n",s_AtomicVal.Load());
}

/*!
 * ワーカースレッドテスト
 * @author Miyake Shunsuke
 * @since 2012.12.8
 */
void TestWorkerThread()
{
    // スレッドプール設定
    TbThreadParam workerParam[3];
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(workerParam); ++i ) {
        workerParam[i].name.SetFormatedString( "Worker%d" , i );
        workerParam[i].processor = i%2;
    }
    TbThreadPool::InitParam initParam;
    initParam.workerCount = TB_ARRAY_COUNT_OF(workerParam);
    initParam.workerParamList = workerParam;
    initParam.jobMax = 10;
    TbThreadPool threadPool(initParam);

    // ジョブ作成
    TbThreadPoolJob* jobList[8];
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(jobList); ++i ){
        TbStaticString32 name = TbStaticString32::Format("Job%d",i);
        if( i%2==0 ) {
            jobList[i] = new TbThreadPoolFuncJob(name.GetBuffer(),TbBind(execJob1,i));
        }else{
            jobList[i] = new TbThreadPoolFuncJob(name.GetBuffer(),TbBind(execJob2,i));
        }
        jobList[i]->SetSequential();
        jobList[i]->Start();
    }

    // ジョブ終了待ち
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(jobList); ++i ){
        jobList[i]->Join();
    }

    while(1){}
    TbPrintf("Finish!");
}

/*!
 * ワーカースレッドテスト
 * @author Miyake Shunsuke
 * @since 2012.12.8
 */
