/*!
 * pthread版ミューテックス
 * @author Miyake Shunsuke
 * @since 2012.12.10
 */

#include "tb_fwd.h"
#include "base/thread/tb_mutex.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.12.10
 */
TbMutex::TbMutex()
    : m_Depend()
{
    // 特に細かい属性の設定はしないので第二引数はNULLにしておく
    pthread_mutex_init( &m_Depend.mutex , NULL );
}

/*! 
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.10
 */
TbMutex::~TbMutex()
{
    pthread_mutex_destroy( &m_Depend.mutex );
}

/*!
 * ロック
 * @author Miyake_Shunsuke
 * @since 2012.12.10
 */
void TbMutex::Lock()
{
    pthread_mutex_lock( &m_Depend.mutex );
}

/*!
 * アンロック
 * @author Miyake_Shunsuke
 * @since 2012.12.10
 */
void TbMutex::Unlock()
{
    pthread_mutex_unlock( &m_Depend.mutex );
}

}
