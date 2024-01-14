/*!
 * スレッド
 * @author Miyake Shunsuke
 * @since 2011.06.24
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
        31 , 
        24 ,
        16 , 
        8 ,
        0 ,
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(PRIORITY_TABLE) == TB_THREAD_PRIORITY_MAX );
}

// 依存クラス

/* 
 * スレッドの実行関数.
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
int TbThreadDepend::ThreadProc(int intArg, void *ptrArg)
{
    TbThread* thread = reinterpret_cast<TbThread*>(ptrArg);
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
    : m_Handle()
    , m_IsCreated(TB_FALSE)
    , m_Stack(nullptr);
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
    TB_SAFE_DELETE_ARRAY(m_Stack);
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
        TbUint32 attr = OS_THREAD_ATTR_DETACH;
        if( 0 <= m_Processor ){
            static const TbUint32 AFFINITY_TABLE[] =
            {
                OS_THREAD_ATTR_AFFINITY_CORE0 ,
                OS_THREAD_ATTR_AFFINITY_CORE1 ,
                OS_THREAD_ATTR_AFFINITY_CORE2
            };
            TB_RESQUE_RET(m_Processor<TB_ARRAY_COUNT_OF(AFFINITY_TABLE),TB_E_FAIL);
            attr |= AFFINITY_TABLE[m_Processor];
        }
        m_Depend.m_Stack = TB_NEW TbUint8[m_StackSize];
        BOOL result = OSCreateThread( &m_Depend.m_Handle , 
                                      TbThreadDepend::ThreadProc ,
                                      1 , 
                                      this ,
                                      m_Stack ,
                                      m_StackSize , // メインスレッドと同じスタックサイズ
                                      PRIORITY_TABLE[m_Priority] ,
                                      attr );
        if( result ){
            m_Depend.m_IsCreated = TB_TRUE;
            OSSetThreadName ( &m_Depend.m_Handle , m_Name.GetBuffer() );
            // スレッド開始
            OSResumeThread(&m_Depend.m_Handle);
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
    if( m_Depend.m_IsCreated ){
        OSJoinThread( &m_Depend.m_Handle , NULL );
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
 */
TbResult TbThread::Sleep( TbUint32 msec )
{
    OSSleepSeconds(static_cast<TbFloat32>(msec)*0.001f);
    return TB_S_OK;
}

}
