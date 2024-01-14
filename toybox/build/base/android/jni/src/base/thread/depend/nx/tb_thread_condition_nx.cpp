/*!
 * スレッド
 * @author teco
 */

#include "tb_fwd.h"
#include "base/thread/tb_thread_condition.h"
#include "base/thread/tb_scoped_lock.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbThreadCondition::TbThreadCondition( TbUint32 capacity )
    : m_Depend()
    , m_InternalMutex()
{
    nn::os::InitializeConditionVariable(&m_Depend.cond);
}

/*! 
 * デストラクタ
 * @author teco
 */
TbThreadCondition::~TbThreadCondition()
{
}

/*!
 * 待ち
 * @author teco
 */
TbResult TbThreadCondition::Wait( TbMutex& mutex )
{
    nn::os::WaitConditionVariable(&m_Depend.cond, &mutex.GetDepend().mutex);
    return TB_S_OK;
}

/*!
 * 起こす
 * @author teco
 */
void TbThreadCondition::Notify()
{
    nn::os::SignalConditionVariable(&m_Depend.cond);
}

// 
/*!
 * 全て起こす
 * @author teco
 */
void TbThreadCondition::NotifyAll()
{
    nn::os::BroadcastConditionVariable(&m_Depend.cond);
}

}
