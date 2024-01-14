/*!
 * 条件変数
 * @author Miyake Shunsuke
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
    OSInitEvent( &m_Depend.m_Event , TRUE , OS_EVENT_AUTO );
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
    mutex.Unlock();
    {
        // 起こされるまで待つ
        OSWaitEvent( &m_Depend.m_Event );
    }
    mutex.Lock();
    return TB_S_OK;
}

/*!
 * 起こす
 * @author Miyake_Shunsuke
 */
void TbThreadCondition::Notify()
{
    TbScopedLock lock( m_InternalMutex );
    OSSignalEvent( &m_Depend.m_Event );
}

// 
/*!
 * 全て起こす
 * @author Miyake_Shunsuke
 */
void TbThreadCondition::NotifyAll()
{
    TbScopedLock lock( m_InternalMutex );
    OSSignalEventAll( &m_Depend.m_Event );
}

}
