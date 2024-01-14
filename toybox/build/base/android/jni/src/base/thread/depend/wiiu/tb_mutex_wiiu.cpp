/*!
 * ミューテックス
 * @author Miyake Shunsuke
 * @since 2011.06.24
 */

#include "tb_fwd.h"
#include "base/thread/tb_mutex.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 */
TbMutex::TbMutex()
    : m_Depend()
{
    OSInitMutex(&m_Depend.mutex);
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 */
TbMutex::TbMutex( const TbMutex& src )
    : m_Depend()
{
    m_Depend.mutex = src.m_Depend.mutex;
}

/*! 
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbMutex::~TbMutex()
{
}

/*!
 * ロック
 * @author Miyake_Shunsuke
 */
void TbMutex::Lock()
{
    OSLockMutex(&m_Depend.mutex);
}

/*!
 * アンロック
 * @author Miyake_Shunsuke
 */
void TbMutex::Unlock()
{
    OSUnlockMutex(&m_Depend.mutex);
}

}
