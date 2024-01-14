/*!
 * スレッド
 * @author teco
 */

#include "tb_fwd.h"
#include "base/thread/tb_mutex.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbMutex::TbMutex()
    : m_Depend()
{
    nn::os::InitializeMutex(&m_Depend.mutex,false,0);
}

#if 0
/*!
 * コンストラクタ
 * @author teco
 */
TbMutex::TbMutex( const TbMutex& src )
    : m_Depend()
{
    m_Depend.mutex = src.m_Depend.mutex;
}
#endif

/*! 
 * デストラクタ
 * @author teco
 */
TbMutex::~TbMutex()
{
}

/*!
 * ロック
 * @author teco
 */
void TbMutex::Lock()
{
    nn::os::LockMutex(&m_Depend.mutex);
}

/*!
 * ロックを試みる
 * @author teco
 */
TbBool TbMutex::TryLock()
{
    return nn::os::TryLockMutex(&m_Depend.mutex);
}

/*!
 * アンロック
 * @author teco
 */
void TbMutex::Unlock()
{
    nn::os::UnlockMutex(&m_Depend.mutex);
}

}
