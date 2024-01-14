/*!
 * スレッド
 * @author Miyake Shunsuke
 * @since 2011.06.24
 */

#include "tb_fwd.h"
#include "base/thread/tb_thread_condition.h"
#include "base/thread/tb_scoped_lock.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
TbThreadCondition::TbThreadCondition( TbUint32 capacity )
    : m_Depend( capacity )
    , m_InternalMutex()
{
}

/*! 
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
TbThreadCondition::~TbThreadCondition()
{
}

/*!
 * 待ち
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
TbResult TbThreadCondition::Wait( TbMutex& mutex )
{
    if( m_Depend.m_NotifyEventDeque.IsFull() ){
        return TB_E_FAIL;
    }
    mutex.Unlock();
    {
        m_InternalMutex.Lock();
        
        // キューに追加する
        HANDLE eve = CreateEvent(NULL,TB_FALSE,TB_FALSE,NULL);
        ResetEvent(eve);
        m_Depend.m_NotifyEventDeque.PushBack(eve);
        
        m_InternalMutex.Unlock();

        // 起こされるまで待つ
        WaitForSingleObject(eve,INFINITE);

        // 後始末
        CloseHandle(eve);
    }
    mutex.Lock();
    return TB_S_OK;
}

/*!
 * 起こす
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
void TbThreadCondition::Notify()
{
    TbScopedLock lock( m_InternalMutex );
    if( m_Depend.m_NotifyEventDeque.IsEmpty() ){
        return;
    }
    HANDLE eve = m_Depend.m_NotifyEventDeque.GetFront();
    SetEvent( eve );
    m_Depend.m_NotifyEventDeque.PopFront();
}

// 
/*!
 * 全て起こす
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
void TbThreadCondition::NotifyAll()
{
    TbScopedLock lock( m_InternalMutex );
    while( !m_Depend.m_NotifyEventDeque.IsEmpty() ){
        HANDLE eve = m_Depend.m_NotifyEventDeque.GetFront();
        SetEvent( eve );
        m_Depend.m_NotifyEventDeque.PopFront();
    }
}

}
