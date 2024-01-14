/*!
 * スレッド
 * @author Miyake Shunsuke
 * @since 2011.06.24
 */

#include "tb_fwd.h"
#include "base/thread/tb_thread.h"
#include "base/system/tb_assert.h"
#include <unistd.h>

namespace toybox
{

namespace
{
}

// 依存クラス

/* 
 * スレッドの実行関数.
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
void* TbThreadDepend::ThreadProc(void* param)
{
    TbThread* thread = reinterpret_cast<TbThread*>(param);
    if( thread )
    {
        thread->Begin();
        while(1){
            if( TB_THREAD_UPDATE_RESULT_FINISH == thread->Update() ){
                break;
            }
        }
        thread->End();
        thread->setState( TbThread::STATE_READY );
    }
    return NULL;
}

/*!
 * コンストラクタ.25
 */
TbThreadDepend::TbThreadDepend()
    : m_Thread()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbThreadDepend::~TbThreadDepend()
{
    Detach();
}

///////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbThread::TbThread()
    : m_State(STATE_PRE)
    , m_Name()
    , m_Processor( TB_THREAD_PROCESSOR_ALL )
    , m_Priority( TB_THREAD_PRIORITY_NORMAL )
    , m_StackSize( TB_THREAD_STACK_SIZE_DEFAULT )
    , m_IsRequestdFinish(TB_FALSE)
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbThread::~TbThread()
{
    // スレッドが動いていることは許容しない！
    TB_ASSERT(STATE_RUN!=m_State.Load());
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 */
void TbThread::Initialize( const char* name )
{
    TB_RESQUE(STATE_PRE==m_State.Load());
    TB_RESQUE(name);
    m_Name = name;
    m_State.Store(STATE_READY);
}

//
/*!
 * 初期化
 * @author Miyake Shunsuke
 */
void TbThread::Initialize( const TbThreadParam& param )
{
    TB_RESQUE(STATE_PRE==m_State.Load());
    m_Name = param.name;
    m_Processor = param.processor;
    m_Priority = param.priority;
    m_StackSize = param.stackSize;
    m_State.Store(STATE_READY);
}

/*!
 * スレッド開始
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThread::Start()
{
    if( m_State.CompareAndSwap( STATE_RUN , STATE_READY ) ){ // 現在スレッドが有効かどうか
        // 属性設定
        pthread_attr_t attr;
        sched_param param;
        pthread_attr_init (&attr);
        pthread_attr_getschedparam (&attr, &param);
        param.sched_priority = m_Priority;
        pthread_attr_setschedparam (&attr, &param);
        
        // スレッド作成
        m_Depend.Detach();
        pthread_t& handle = m_Depend.getHandle();
        int result = pthread_create(&handle,&attr,TbThreadDepend::ThreadProc,this);
        if( 0 == result ) {
//            setThreadName( m_Name.GetBuffer() , threadId );
            if( 0 <= m_Processor ){
                // プロセッサの指定があれば設定する
//                ::SetThreadAffinityMask( handle , TB_BIT(m_Processor) );
            }
            return TB_S_OK;
        }else{
            TB_ASSERT(!"pthread_create is failed");
        }
    }
    return TB_E_FAIL;
}

/*!
 * 終了待ち
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThread::Join()
{
    int ret = pthread_join(m_Depend.getHandle(),NULL);
    if( 0 == ret ){
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * プロセッサ割当
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThread::SetProcessor( TbSint32 processor )
{
    m_Processor = processor;
    return TB_S_OK;
}

/*!
 * 優先順位設定
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThread::SetPriority( TbThreadPriority priority )
{
    if( TB_THREAD_PRIORITY_BEGIN <= priority && priority < TB_THREAD_PRIORITY_END ){
        m_Priority = priority;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * スリープ
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThread::Sleep( TbUint32 msec )
{
    ::sleep(msec);
    return TB_S_OK;
}

}
