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
    if (m_Depend.layer) {
        nn::vi::DestroyLayer(m_Depend.layer);
    }
}

/*! 
 * セットアップ
 * @author teco
 */
TbResult TbWindow::setup( const TbWindowParameter& param )
{
    TbResult ret = TB_E_FAIL;
    
    m_Rect = param.rect;
    m_WindowName = param.name;
    m_Id = static_cast<TbAddr>( ++s_IssueId );
    
    nn::vi::Display* disp = TbWindowManager::GetInstance().GetDepend().display;
    nn::Result result = nn::vi::CreateLayer(&m_Depend.layer, disp);
    TB_ASSERT(result.IsSuccess());

    result = nn::vi::SetLayerScalingMode(m_Depend.layer, nn::vi::ScalingMode_FitToLayer);
    TB_ASSERT(result.IsSuccess());

    return result.IsSuccess() ? TB_S_OK : TB_E_FAIL;
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
