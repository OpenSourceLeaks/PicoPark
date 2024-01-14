/*!
 * レンダービューの依存コード(Stage3D)
 * @author Miyake Shunsuke
 * @since 2013.04.10
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_render_view.h"

#if TB_3DAPI_IS_S3D

#include "graphics/3dapi/tb_render_device.h"
#include "graphics/3dapi/tb_depth_stencil.h"
#include "graphics/3dapi/tb_render_target.h"

namespace toybox
{

/////////////////////////////////////////////////////////////////////
// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.5.05
 */
TbRenderViewDepend::TbRenderViewDepend()
    : m_RenderTarget( NULL )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.5.05
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    TB_SAFE_DELETE( m_RenderTarget);
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.5.05
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
 * @since 2013.5.05
 */
TbRenderView::~TbRenderView()
{
    TB_SAFE_DELETE( m_DepthStencil );
    TB_SAFE_DELETE( m_Depend.m_RenderTarget);
}

/*!
 * 画面に表示
 * @author Miyake Shunsuke
 * @since 2013.5.05
 */
void TbRenderView::Present()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.GetDepend().GetContext()->present();
}
    
/*!
 * レンダーターゲット取得
 * @author Miyake Shunsuke
 * @since 2013.05.01
 */
TbRenderTarget* TbRenderView::GetRenderTarget( TbUint32 backBufferIndex )
{
    if( TB_VERIFY(0==backBufferIndex) ){
        return m_Depend.m_RenderTarget;
    }
    return NULL;
}

}

#endif
