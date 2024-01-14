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
 * @author Miyake Shunsuke
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
 * @author Miyake Shunsuke
 */
TbWindow::~TbWindow()
{
}

/*! 
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2011.03.07
 */
TbResult TbWindow::setup( const TbWindowParameter& param )
{
    TbResult result = TB_E_FAIL;
    
    m_Rect = param.rect;
    m_WindowName = param.name;
    m_Id = static_cast<TbAddr>( ++s_IssueId );
    return result;
}

/*!
 * 座標設定
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult SetPos( TbSint32 x , TbSint32 y )
{
    return TB_E_FAIL; // 0,0固定
}

/*!
 * サイズ設定
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult TbWindow::SetSize(TbSint32 width , TbSint32 height)
{
    m_Rect.width = width;
    m_Rect.height = height;
    return TB_S_OK;
}

/*!
 * 矩形設定 (ディスプレイと同じサイズである必要がある)
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult TbWindow::SetRect(TbSint32 x,TbSint32 y,TbSint32 width,TbSint32 height)
{
    m_Rect.x = x;
    m_Rect.y = y;
    m_Rect.width = width;
    m_Rect.height = height;
    return TB_S_OK;
}

/*!
 * 可視設定
 * @param isVisible 可視
 */
void TbWindow::SetVisible(TbBool isVisible)
{
    // バックグラウンド実行中とみなした実装予定
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
