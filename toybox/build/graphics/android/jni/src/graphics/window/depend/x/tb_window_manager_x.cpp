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
 * @author teco
 */
TbWindowManager::TbWindowManager()
    : m_BitArray()
    , m_WindowList(1)
    , m_FocusWindow( NULL )
    , m_Depend()
{
    m_BitArray.SetBit( FLAG_ERROR , setup() == TB_S_OK );
}

/*!
 * デストラクタ
 * @author teco
 */
TbWindowManager::~TbWindowManager()
{
    DeleteWindowAll();
#if !TB_PLATFORM_IS_EMSCRIPTEN
    XCloseDisplay(m_Depend.display);
#endif
}

/*!
 * ウィンドウクラスをセットアップ
 * @author teco
 */
TbResult TbWindowManager::setup()
{
    m_Depend.display = XOpenDisplay( nullptr );
    return TB_S_OK;
}

/*!
 * 更新
 */
TbBool TbWindowManager::Update() 
{
    return TB_TRUE;
}

}
