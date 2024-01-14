/*!
 * Window管理クラス(Flash版)
 * Flash版ではシングルウィンドウなので、実質一つだけ管理
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */

#include "tb_fwd.h"
#include "graphics/window/tb_window_manager.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbWindowManager::TbWindowManager()
    : m_BitArray()
    , m_WindowList(1) // Androidは単一ウィンドウ
    , m_FocusWindow( NULL )
    , m_Depend()
{
    m_BitArray.SetBit( FLAG_ERROR , setup() == TB_S_OK );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.4.10
 */
TbWindowManager::~TbWindowManager()
{
    DeleteWindowAll();
}

/*!
 * ウィンドウクラスをセットアップ
 * @author Miyake Shunsuke
 * @since 2010.4.10
 */
TbResult TbWindowManager::setup()
{
}

/*!
 * 更新
 */
TbBool TbWindowManager::Update() 
{
    return TB_TRUE;
}

}
