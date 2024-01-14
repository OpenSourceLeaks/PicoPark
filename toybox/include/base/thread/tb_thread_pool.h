/*!
 * スレッドプールクラス
 * @author teco
 */
 
#ifndef _INCLUDED_TB_THREAD_POOL_H_
#define _INCLUDED_TB_THREAD_POOL_H_

#include <base/container/tb_array.h>
#include <base/container/tb_queue.h>
#include <base/util/tb_singleton.h>
#include <base/thread/tb_mutex.h>
#include <base/thread/tb_thread_condition.h>
#include <base/thread/tb_thread_types.h>

namespace toybox
{

class TbThread;
class TbThreadPoolJob;

/*!
 * スレッドプールクラス
 * @author teco
 */
class TbThreadPool : public TbSingletonRef<TbThreadPool>
{
public:
    typedef TbThreadPool Self;
public:

    struct InitParam
    {
        InitParam() : workerParamList(NULL) , workerCount(0) , sequentialJobWorkerIndex(0) , jobMax(1) {}
        TbThreadParam* workerParamList;           // ワーカースレッドパラメータ
        TbUint32       workerCount;               // ワーカースレッド数
        TbUint32       sequentialJobWorkerIndex;  // sequentialJobを担当するワーカー
        TbUint32       jobMax;                    // 受け入れ可能なジョブ最大数
    };

public:

    // コンストラクタ
    explicit TbThreadPool( const InitParam& initParam );

    // デストラクタ
    virtual ~TbThreadPool();

public:

    // ジョブを追加
    TbBool PushJob( TbThreadPoolJob* job );

private:

    // スレッドから呼ばれる関数
    TbThreadUpdateResult updateWorker();

    // スレッドから呼ばれる関数
    TbThreadUpdateResult updateWorkerWithSequentialJob();

private:

    TbArray<TbThread*>         m_WorkerThread;       //!< ワーカースレッド
    TbQueue<TbThreadPoolJob*>  m_JobQueue;           //!< ジョブキュー
    TbQueue<TbThreadPoolJob*>  m_SequentialJobQueue; //!< シーケンシャルジョブキュー(先に積まれたジョブが終わる前に開始しないことを保証するキュー)
    TbMutex                    m_Mutex;              //!< ミューテックス
    TbThreadCondition          m_Condition;          //!< 条件変数

};

}

#endif
