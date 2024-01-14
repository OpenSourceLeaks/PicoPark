/*!
 * スレッド
 * @author teco
 */

#include "tb_fwd.h"
#include "base/thread/tb_thread.h"
#include "base/system/tb_assert.h"

namespace toybox
{

namespace
{
    static const TbSint32 PRIORITY_TABLE[] =
    {
        nn::os::LowestThreadPriority ,
        nn::os::LowestThreadPriority + (nn::os::LowestThreadPriority - nn::os::DefaultThreadPriority) / 2 ,
        nn::os::DefaultThreadPriority ,
        nn::os::HighestThreadPriority + (nn::os::HighestThreadPriority - nn::os::DefaultThreadPriority) / 2 ,
        nn::os::HighestThreadPriority ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(PRIORITY_TABLE) == TB_THREAD_PRIORITY_MAX);

    static const TbUint32 THREAD_ID_INVALID = 0xFFFFFFFF;

}

/* 
 * スレッドの実行関数.
 */
void TbThreadDepend::ThreadProc(void* param)
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
}

///////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
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
 * @author teco
 */
TbThread::~TbThread()
{
    // スレッドが動いていることは許容しない！
    if (m_Depend.isCreated)
    {
        nn::os::DestroyThread(&m_Depend.thread);
        m_Depend.isCreated = TB_FALSE;
    }
    if (m_Depend.stack) {
        TbFreeDefaultHeap(m_Depend.stack);
    }
}

/*!
 * 初期化
 * @author teco
 */
void TbThread::Initialize( const char* name )
{
    TB_RESQUE(STATE_PRE==m_State.Load());
    TB_RESQUE(name);
    m_Name = name;
    m_State.Store(STATE_READY);
}

/*!
 * 初期化
 * @author teco
 */
void TbThread::Initialize( const TbThreadParam& param )
{
    TB_RESQUE(STATE_PRE==m_State.Load());
    m_Name = param.name;
    m_Processor = param.processor;
    m_Priority = param.priority;
    m_StackSize = TB_ALIGN_ROUND_UP(param.stackSize,nn::os::ThreadStackAlignment);
    m_State.Store(STATE_READY);
}


/*!
 * スレッド開始
 * @author teco
 */
TbResult TbThread::Start()
{
    TB_RESQUE_RET(STATE_PRE!=m_State.Load(),TB_E_FAIL); // 初期化前は許容しない

    if( m_State.CompareAndSwap( STATE_RUN , STATE_READY ) ){ // 現在スレッドが有効かどうか
        unsigned int threadId = 0;
        if (m_Depend.isCreated) {
            nn::os::DestroyThread(&m_Depend.thread);
        }
        if (m_Depend.stack) {
           TbFreeDefaultHeap(m_Depend.stack);
        }
        m_Depend.stack = reinterpret_cast<char*>(TbMallocDefaultHeap(m_StackSize,nn::os::ThreadStackAlignment));
        m_Depend.isCreated = TB_FALSE;

        nn::Result result = nn::os::CreateThread(&m_Depend.thread,
                                                TbThreadDepend::ThreadProc,
                                                this,
                                                m_Depend.stack,
                                                m_StackSize,
                                                PRIORITY_TABLE[m_Priority]);
        if( TB_VERIFY(result.IsSuccess()) ){
            m_Depend.isCreated = TB_TRUE;
            nn::os::SetThreadName(&m_Depend.thread, m_Name.GetBuffer());
            if( 0 <= m_Processor ){
                // プロセッサの指定があれば設定する
                nn::os::SetThreadCoreMask(&m_Depend.thread, m_Processor, TB_BIT(m_Processor));
            }
            // スレッド開始
            nn::os::StartThread(&m_Depend.thread);
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * 終了待ち
 * @author teco
 */
TbResult TbThread::Join()
{
    if( m_Depend.isCreated ){
        nn::os::WaitThread( &m_Depend.thread );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * プロセッサ割当
 * @author teco
 */
TbResult TbThread::SetProcessor( TbSint32 processor )
{
    m_Processor = processor;
    return TB_S_OK;
}

/*!
 * 優先順位設定
 * @author teco
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
 * @author teco
s */
TbResult TbThread::Sleep( TbUint32 msec )
{
    nn::TimeSpan time = nn::TimeSpan::FromMilliSeconds(msec);
    nn::os::SleepThread(time);
    return TB_S_OK;
}

}
