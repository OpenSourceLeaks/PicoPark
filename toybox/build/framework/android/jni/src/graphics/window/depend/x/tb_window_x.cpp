/*!
 * ウィンドウ
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */

#include "tb_fwd.h"
#include "graphics/window/tb_window.h"
#include "graphics/window/tb_window_manager.h"
#include "graphics/display/tb_display_manager.h"

namespace toybox
{
namespace{
    TbUint32 s_IssueId = 0;
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
TbWindow::TbWindow( const TbWindowParameter& param )
    : m_BitArray()
    , m_Id(0)
    , m_WindowName("")
    , m_Rect()
    , m_Depend()
{
    if( TB_FAILED(setup( param )) ){
        m_BitArray.SetBit( FLAG_ERROR );
    }
}

/*!
 * デストラクタ
 * @author teco
 */
TbWindow::~TbWindow()
{
}

/*! 
 * セットアップ
 * @author teco
 */
TbResult TbWindow::setup( const TbWindowParameter& param )
{
    TbResult result = TB_E_FAIL;
    
    m_Rect = param.rect;
    m_WindowName = param.name;
    m_Id = static_cast<TbAddr>( ++s_IssueId );
    
    Display* xdisp = TbWindowManager::GetInstance().GetDepend().display;
    Window rootWin = DefaultRootWindow( xdisp );
    int screen = DefaultScreen( xdisp );
    
    XSetWindowAttributes  swa;
    TbMemClear(swa);
    m_Depend.window = XCreateWindow( xdisp, 
                                     rootWin, // 親
                                     m_Rect.x, m_Rect.y,    // x,y
                                     m_Rect.width, m_Rect.height,// w,h
                                     0,       // border_width
                                     CopyFromParent,  // depth
                                     InputOutput,     // class
                                     CopyFromParent,  // visual
                                     0, // valuemask
                                     &swa );

    XMapWindow( xdisp, m_Depend.window );
    return result;
}

/*!
 * 座標設定
 * @author teco
 */
TbResult SetPos( TbSint32 x , TbSint32 y )
{
    return TB_E_FAIL; // 0,0固定
}

/*!
 * サイズ設定
 * @author teco
 */
TbResult TbWindow::SetSize(TbSint32 width , TbSint32 height)
{
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * 矩形設定 (ディスプレイと同じサイズである必要がある)
 * @author teco
 */
TbResult TbWindow::SetRect(TbSint32 x,TbSint32 y,TbSint32 width,TbSint32 height)
{
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * 可視設定
 * @author teco
 */
void TbWindow::SetVisible(TbBool isVisible)
{
    TB_ASSERT(!"unsupported");
}

/*!
 * 最大化
 */
void TbWindow::Maxmize()
{
    // 何もしない
}

/*!
 * 最小化
 */
void TbWindow::Minimize()
{
    // 何もしない
}

}
