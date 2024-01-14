/*!
 * レンダーターゲット
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_depth_stencil_state.h"

#if (TB_3DAPI_IS_GFX)

#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/tb_render_view.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbDepthStencil::TbDepthStencil( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : m_Type( TB_DEPTH_STENCIL_TYPE_BUFFER )
    , m_BitArray()
    , m_Depend()
{
    TB_ASSERT(!"need create from texture");
}

/*!
 * コンストラクタ( テクスチャから生成 )
 * @author teco
 */
TbDepthStencil::TbDepthStencil( TbTexture* texture )
    : m_Type( TB_DEPTH_STENCIL_TYPE_TEXTURE )
    , m_BitArray()
    , m_Depend()
{
    nn::gfx::Device* device = TbGetGfxDevice();
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    if( texture ){
        nn::gfx::DepthStencilView::InfoType info;
        info.SetDefault();
        info.SetImageDimension(nn::gfx::ImageDimension_2d);
        info.SetTexturePtr(texture->GetDepend().GetTexture());
        m_Depend.depthStencilView.Initialize(device, info);
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * コンストラクタ( ビューから生成 )
 * @author teco
 */
TbDepthStencil::TbDepthStencil( TbRenderView* view )
    : m_Type( TB_DEPTH_STENCIL_TYPE_VIEW )
    , m_BitArray()
    , m_Depend()
{
    TB_ASSERT(!"need create from texture");
}

/*!
 * デストラクタ
 * @author teco
 */
TbDepthStencil::~TbDepthStencil()
{
    if (IsValid()) {
        nn::gfx::Device* device = TbGetGfxDevice();
        m_Depend.depthStencilView.Finalize(device);
    }
}
    
}

#endif
