/*!
 * レンダービュークラス (Android用)
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_view.h"

#if TB_3DAPI_TARGET == TB_3DAPI_GL

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

/////////////////////////////////////////////////////////////////////
// 依存コード

/*!
 * コンストラクタ
 * @parma device 対応デバイス
 * @param param 生成パラメータ
 * @param index 既に生成済みのSwapChainがあれば、そのインデックス
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderViewDepend::TbRenderViewDepend()
    : m_RenderTarget( NULL )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    TB_SAFE_DELETE( m_RenderTarget);
}

/*!
 * カレントビューに設定。設定をコピーするビューの設定可
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */
void TbRenderViewDepend::SetCurrent( TbRenderView* copySrc ) 
{
    // 何もしない (複数ビューはありえない)
    TB_ASSERT(!copySrc || &copySrc->GetDepend()==this);
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderView::TbRenderView( const TbRenderViewParameter& param , TbSint32 index )
    : m_BitArray()
    , m_Depend()
    , m_Param( param )
    , m_DepthStencil( NULL )
    , m_FrontBufferFormat( TB_COLOR_FORMAT_UNKNOWN )
{
    m_Depend.m_RenderTarget = TB_NEW TbRenderTarget( this , 1 );
    m_DepthStencil = TB_NEW TbDepthStencil( this );
    m_FrontBufferFormat = TB_COLOR_FORMAT_X8R8G8B8;
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderView::~TbRenderView()
{
    TB_SAFE_DELETE( m_DepthStencil );
    TB_SAFE_DELETE( m_Depend.m_RenderTarget);
}

/*!
 * 画面に表示
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */
void TbRenderView::Present()
{
    TbRenderDevice::GetInstance().GetViewport(m_PresentRegion);
#if TB_GLAPI_IS_GLSL2
    // Javaコード側で行う
    // TB_ASSERT(!"Unsupported (Alternative is Java code)");
#endif
}

}

#endif
