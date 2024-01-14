/*!
 * スレッドクラス
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#include "tb_fwd.h"
#include "base/thread/tb_thread_pool.h"
#include "base/thread/tb_thread_pool_job.h"
#include "base/thread/tb_func_thread.h"
#include "base/thread/tb_scoped_lock.h"

#include <base/thread/tb_wait_notify_thread.h>

namespace toybox
{

namespace 
{
    class WorkerThread : public TbFuncThread
    {
    public:
        typedef TbFuncThread Super;
    public:
        // コンストラクタ
        WorkerThread( const TbThreadParam& param , const Super::UpdateFunc& updateFunc )
            : Super()
        {
            Initialize(param,updateFunc);
        }
    public:
        // スレッドループ処理
        virtual TbThreadUpdateResult Update(){ 
            if( IsRequestedFinish() ){
                return TB_THREAD_UPDATE_RESULT_FINISH;
            }
            return Super::Update();
        };
    };
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.12.05
 */
TbThreadPool::TbThreadPool( const InitParam& initParam )
    : m_WorkerThread(initParam.workerCount)
    , m_JobQueue(initParam.jobMax)
    , m_SequentialJobQueue(initParam.jobMax)
    , m_Mutex()
    , m_Condition(initParam.workerCount)
{
    for( TbUint32 i = 0; i < initParam.workerCount; ++i ) {
        WorkerThread* worker = NULL;
        if( initParam.sequentialJobWorkerIndex == i ) {
            worker = TB_NEW WorkerThread(initParam.workerParamList[i],TbCreateMemFunc(this,&Self::updateWorkerWithSequentialJob));
        }else{
            worker = TB_NEW WorkerThread(initParam.workerParamList[i],TbCreateMemFunc(this,&Self::updateWorker));
        }
        m_WorkerThread.PushBack(worker);
        worker->Start();
    }
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.12.05
 */
TbThreadPool::~TbThreadPool()
{
    TbArray<TbThread*>::Iterator endIte = m_WorkerThread.End();
    for( TbArray<TbThread*>::Iterator ite = m_WorkerThread.Begin(); ite != endIte ; ++ite ){
        (*ite)->RequestFinish();
    }
    m_Condition.NotifyAll();
    for( TbArray<TbThread*>::Iterator ite = m_WorkerThread.Begin(); ite != endIte ; ++ite ){
        (*ite)->Join();
        TB_DELETE *ite;
    }
}

/*!
 * ジョブを追加
 * @author Miyake_Shunsuke
 * @since 2012.12.05
 */
TbBool TbThreadPool::PushJob( TbThreadPoolJob* job )
{
    if( !job ) {
        return TB_FALSE;
    }
    job->Retain();
    // とりあえず今はキューに入れることができないことは考慮しない
    if( job->IsSequential() ) {
        TbScopedLock lock(m_Mutex);
        if( !TB_VERIFY(m_SequentialJobQueue.PushBack(job)) ){
            return TB_FALSE;
        }
    }else{
        TbScopedLock lock(m_Mutex);
        if( !TB_VERIFY(m_JobQueue.PushBack(job)) ){
            return TB_FALSE;
        }
    }
    m_Condition.NotifyAll();
    return TB_TRUE;
}

/*!
 * ワーカースレッド更新
 * @author Miyake_Shunsuke
 * @since 2012.12.05
 */
TbThreadUpdateResult TbThreadPool::updateWorker()
{
    TbThreadPoolJob* job = NULL;
    {
        TbScopedLock lock(m_Mutex);
        if( m_JobQueue.IsEmpty() ) {
            m_Condition.Wait(m_Mutex);
        }else{
            job = m_JobQueue.GetFront();
            m_JobQueue.PopFront();
        }
    }
    if( job ) {
        job->ExecuteJob();
        job->Release();
    }
    return TB_THREAD_UPDATE_RESULT_PENDING;
}

/*!
 * ワーカースレッド更新
 * @author Miyake_Shunsuke
 * @since 2012.12.05
 */
TbThreadUpdateResult TbThreadPool::updateWorkerWithSequentialJob()
{
    TbThreadPoolJob* job = NULL;
    {
        TbScopedLock lock(m_Mutex);
        if( m_SequentialJobQueue.IsEmpty() ) {
            if( m_JobQueue.IsEmpty() ){
                m_Condition.Wait(m_Mutex);
            }else{
                job = m_JobQueue.GetFront();
                m_JobQueue.PopFront();
            }
        }else{
            job = m_SequentialJobQueue.GetFront();
            m_SequentialJobQueue.PopFront();
        }
    }
    if( job ) {
        job->ExecuteJob();
        job->Release();
    }
    return TB_THREAD_UPDATE_RESULT_PENDING;
}

}
