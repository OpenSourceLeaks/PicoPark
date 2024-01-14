/*!
 * スレッド
 * @author Miyake Shunsuke
 * @since 2011.06.24
 */

#include "tb_fwd.h"
#include "base/thread/tb_thread.h"
#include "base/system/tb_assert.h"
#include <process.h> 

namespace toybox
{

namespace
{
    static const TbSint32 PRIORITY_TABLE[] = 
    {
        THREAD_PRIORITY_LOWEST , 
        THREAD_PRIORITY_BELOW_NORMAL ,
        THREAD_PRIORITY_NORMAL , 
        THREAD_PRIORITY_ABOVE_NORMAL ,
        THREAD_PRIORITY_HIGHEST ,
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(PRIORITY_TABLE) == TB_THREAD_PRIORITY_MAX );

    static const TbUint32 THREAD_ID_INVALID = 0xFFFFFFFF;

    #pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
    #pragma pack(pop)

    // スレッド名設定（デバッガに表示できるように）
    void setThreadName( const char* name , DWORD threadId ) 
    {
        const DWORD MS_VC_EXCEPTION=0x406D1388;

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = threadId;
        info.dwFlags = 0;

        __try
        {
            RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
}

// 依存クラス

/* 
 * スレッドの実行関数.
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
unsigned WINAPI TbThreadDepend::ThreadProc(void* param)
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
    return 0;
}

/*!
 * コンストラクタ.25
 */
TbThreadDepend::TbThreadDepend()
    : m_Handle(0)
    , m_ThreadId(THREAD_ID_INVALID)
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

/*!
 * ハンドル設定
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThreadDepend::setHandle( HANDLE handle )
{
    m_Handle = handle;
    return TB_S_OK;
}

/*!
 * スレッドID設定
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbResult TbThreadDepend::setThreadId( TbUint32 threadId )
{
    m_ThreadId = threadId;
    return TB_S_OK;
}

///////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.25
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
 * @author Miyake_Shunsuke
 * @since 2015.01.04
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
 * @author Miyake_Shunsuke
 * @since 2015.01.04
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
    TB_RESQUE_RET(STATE_PRE!=m_State.Load(),TB_E_FAIL); // 初期化前は許容しない

    if( m_State.CompareAndSwap( STATE_RUN , STATE_READY ) ){ // 現在スレッドが有効かどうか
        m_Depend.Detach();
        unsigned int threadId = 0;
        HANDLE handle = reinterpret_cast<HANDLE>(::_beginthreadex( NULL , 
                                                                   m_StackSize , // メインスレッドと同じスタックサイズ
                                                                   TbThreadDepend::ThreadProc , 
                                                                   this , 
                                                                   CREATE_SUSPENDED ,  // 初期状態は一時停止
                                                                   &threadId ));
        if( handle ){
            m_Depend.setHandle( handle );
            m_Depend.setThreadId( threadId );
            setThreadName( m_Name.GetBuffer() , threadId );
            if( 0 <= m_Processor ){
                // プロセッサの指定があれば設定する
                ::SetThreadAffinityMask( handle , TB_BIT(m_Processor) );
            }
            ::SetThreadPriority(handle , PRIORITY_TABLE[m_Priority] );
            // スレッド開始
            ::ResumeThread(handle);
            return TB_S_OK;
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
    if( m_Depend.GetHandle() ){
        WaitForSingleObject( m_Depend.GetHandle() , INFINITE );
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
    ::Sleep(msec);
    return TB_S_OK;
}

}
