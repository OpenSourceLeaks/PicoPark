/*!
 * @file
 * @brief DirectX9の深度ステンシル依存コード
 * @author Miyake Shunsuke
 * @since 2011.05.31
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"
#include "base/system/tb_assert.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
TbDepthStencilDepend::TbDepthStencilDepend( TbRenderView* view )
    : TbLostResourceDx9( &view->GetDepend() )
    , m_DepthStencil( nullptr )
    , m_Width( 0U )
    , m_Height( 0U )
    , m_ColorFormat( TB_COLOR_FORMAT_UNKNOWN )
    , m_Surface( nullptr )
    , m_View( view )
{
    TB_ASSERT( view );
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */

TbDepthStencilDepend::TbDepthStencilDepend( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : TbLostResourceDx9( TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_DepthStencil( nullptr )
    , m_Width( width )
    , m_Height( height )
    , m_ColorFormat( colorFormat )
    , m_Surface( nullptr )
{
    TB_ASSERT( m_Device );
}

/*!
 * コンストラクタ（テクスチャから生成）
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
TbDepthStencilDepend::TbDepthStencilDepend( TbTexture* texture )
    : TbLostResourceDx9( &texture->GetDepend() )
    , m_DepthStencil( nullptr )
    , m_Width( 0U )
    , m_Height( 0U )
    , m_ColorFormat( TB_COLOR_FORMAT_UNKNOWN )
    , m_Surface( nullptr )
    , m_Texture( texture )
{
    TB_ASSERT( m_Texture );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbDepthStencilDepend::~TbDepthStencilDepend()
{
    TB_SAFE_RELEASE( m_Surface );
}

/*!
 * デバイスロスト時の対応
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
void TbDepthStencilDepend::setup( TbDepthStencil* depthStencil )
{
    if( !depthStencil ){
        return;
    }
    m_DepthStencil = depthStencil;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    switch( m_DepthStencil->GetType() )
    {
    case TB_DEPTH_STENCIL_TYPE_VIEW: // ビュー
        {
            device.GetDepend().GetDeviceDX9()->GetDepthStencilSurface( &m_Surface );
            TB_ASSERT( m_Surface );
        }
        break;
    case TB_DEPTH_STENCIL_TYPE_BUFFER: // バッファ
        {
            device.GetDepend().GetDeviceDX9()->CreateDepthStencilSurface( m_Width , 
                                                                          m_Height ,
                                                                          TbColorFormatToD3DFORMAT( m_ColorFormat ) ,
                                                                          D3DMULTISAMPLE_NONE ,
                                                                          0 , 
                                                                          FALSE ,
                                                                          &m_Surface ,
                                                                          nullptr );
        }
        break;
    case TB_DEPTH_STENCIL_TYPE_TEXTURE: // テクスチャ
        {
            m_Texture->GetDepend().GetTextureDx9()->GetSurfaceLevel( 0 , &m_Surface );
        }
        break;
    }
    TbGetSizeFromIDirect3DSurface9( m_Width , m_Height , m_Surface );
}

/*!
 * 解放(デバイスロスト用)
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
void TbDepthStencilDepend::Release()
{
    TB_SAFE_RELEASE( m_Surface );
}

// 
/*!
 * 再構築(デバイスロスト用)
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
void TbDepthStencilDepend::Recover()
{
    setup( m_DepthStencil );
}

////////////////////////////////////////////

/*!
 * コンストラクタ(ビューに自動生成されたタイプ)
 * 生成直後に呼ばれる必要がある
 * @author Miyake Shunsuke
 * @since 2011.06.01
 */
TbDepthStencil::TbDepthStencil( TbRenderView* view )
    : m_Type( TB_DEPTH_STENCIL_TYPE_VIEW )
    , m_BitArray()
    , m_Depend( view )
{
    setup();
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
TbDepthStencil::TbDepthStencil( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : m_Type( TB_DEPTH_STENCIL_TYPE_BUFFER )
    , m_BitArray()
    , m_Depend( width , height , colorFormat )
{
    setup();
}

/*!
 * コンストラクタ( テクスチャから生成 )
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbDepthStencil::TbDepthStencil( TbTexture* texture )
    : m_Type( TB_DEPTH_STENCIL_TYPE_TEXTURE )
    , m_BitArray()
    , m_Depend( texture )
{
    setup();
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
void TbDepthStencil::setup()
{
    m_Depend.setup( this );
    if( m_Depend.GetSurface() ){
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbDepthStencil::~TbDepthStencil()
{
}

/*!
 * 解像度横幅取得
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbDepthStencil::GetWidth() const
{
    return m_Depend.GetWidth();
}

/*!
 * 解像度縦幅取得
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbDepthStencil::GetHeight() const
{
    return m_Depend.GetHeight();
}

}

#endif