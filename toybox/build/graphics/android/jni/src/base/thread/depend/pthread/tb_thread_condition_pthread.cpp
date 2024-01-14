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
{
    m_Depend.cond = PTHREAD_COND_INITIALIZER;
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
    pthread_cond_wait(&m_Depend.cond, &mutex.GetDepend().mutex);
    return TB_S_OK;
}

/*!
 * 起こす
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
void TbThreadCondition::Notify()
{
    pthread_cond_signal(&m_Depend.cond);
}

// 
/*!
 * 全て起こす
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
void TbThreadCondition::NotifyAll()
{
    pthread_cond_broadcast(&m_Depend.cond);
}

}
