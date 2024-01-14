/*!
 * Window管理クラス(Android版)
 * Android版ではシングルウィンドウなので、実質一つだけ管理
 * @author Miyake Shunsuke
 * @since 2012.07.05
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
    return TB_S_OK;
}

/*!
 * 更新
 * AndroidではWindowManagerがアプリを終了させることはない
 */
TbBool TbWindowManager::Update() 
{
    return TB_TRUE;
}

}
