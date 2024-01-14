/*!
 * スレッド(シングルスレッド用)
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
}

/*!
 * コンストラクタ
 * @author teco
 */
TbMutex::TbMutex( const TbMutex& src )
    : m_Depend()
{
}

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
}

/*!
 * アンロック
 * @author teco
 */
void TbMutex::Unlock()
{
}

}
