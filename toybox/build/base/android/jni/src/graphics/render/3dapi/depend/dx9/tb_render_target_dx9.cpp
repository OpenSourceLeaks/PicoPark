/*!
 * @file
 * @brief DirectX9の固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_target.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_view.h"
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

TbRenderTargetDepend::TbRenderTargetDepend( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : TbLostResourceDx9( &TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_RenderTarget( nullptr )
    , m_Width( width )
    , m_Height( height )
    , m_ColorFormat( colorFormat )
    , m_BackBufferIndex( 0 )
    , m_Surface( nullptr )
{
    TB_ASSERT( m_Device );
}

/*!
 * コンストラクタ（ビューから生成）
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
TbRenderTargetDepend::TbRenderTargetDepend( TbRenderView* view , TbUint32 backBufferIndex )
    : TbLostResourceDx9( &view->GetDepend() )
    , m_RenderTarget( nullptr )
    , m_Width( 0U )
    , m_Height( 0U )
    , m_ColorFormat( TB_COLOR_FORMAT_UNKNOWN )
    , m_BackBufferIndex( 0 )
    , m_Surface( nullptr )
    , m_View( view )
{
    TB_ASSERT( m_View );
}

/*!
 * コンストラクタ（テクスチャから生成）
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
TbRenderTargetDepend::TbRenderTargetDepend( TbTexture* texture )
    : TbLostResourceDx9( &texture->GetDepend() )
    , m_RenderTarget( nullptr )
    , m_Width( 0U )
    , m_Height( 0U )
    , m_ColorFormat( TB_COLOR_FORMAT_UNKNOWN )
    , m_BackBufferIndex( 0 )
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
TbRenderTargetDepend::~TbRenderTargetDepend()
{
    TB_SAFE_RELEASE( m_Surface );
}

/*!
 * デバイスロスト時の対応
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
void TbRenderTargetDepend::setup( TbRenderTarget* renderTarget )
{
    if( !renderTarget ){
        return;
    }
    m_RenderTarget = renderTarget;
    HRESULT hr = S_OK;
    switch( renderTarget->GetType() )
    {
    case TB_RENDER_TARGET_TYPE_BUFFER:
        {
            TbRenderDevice& device = TbRenderDevice::GetInstance();
            device.GetDepend().GetDeviceDX9()->CreateRenderTarget( m_Width , 
                                                                   m_Height ,
                                                                   TbColorFormatToD3DFORMAT( m_ColorFormat ) ,
                                                                   D3DMULTISAMPLE_NONE ,
                                                                   0 , 
                                                                   FALSE ,
                                                                   &m_Surface ,
                                                                   nullptr );
        }
        break;
    case TB_RENDER_TARGET_TYPE_VIEW:
        {
            hr = m_View->GetDepend().GetSwapChain()->GetBackBuffer( m_BackBufferIndex , D3DBACKBUFFER_TYPE_MONO , &m_Surface );
            TB_ASSERT(TB_SUCCEEDED(hr));
        }
        break;
    case TB_RENDER_TARGET_TYPE_TEXTURE:
        {
            m_Texture->GetDepend().GetTextureDx9()->GetSurfaceLevel( 0 , &m_Surface );
        }
        break;
    };
    TbGetColorFormatFromIDirect3DSurface9( m_ColorFormat , m_Surface );
    TbGetSizeFromIDirect3DSurface9( m_Width , m_Height , m_Surface );
}

/*!
 * 解放(デバイスロスト用)
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
void TbRenderTargetDepend::Release()
{
    TB_SAFE_RELEASE( m_Surface );
}

// 
/*!
 * 再構築(デバイスロスト用)
 * @author Miyake Shunsuke
 * @since 2011.05.30
 */
void TbRenderTargetDepend::Recover()
{
    setup( m_RenderTarget );
}

////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
TbRenderTarget::TbRenderTarget( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : m_Type( TB_RENDER_TARGET_TYPE_BUFFER )
    , m_BitArray()
    , m_Depend( width , height , colorFormat )
{
    setup();
}

/*!
 * コンストラクタ(Viewから生成)
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbRenderTarget::TbRenderTarget( TbRenderView* view , TbUint32 backBufferIndex )
    : m_Type( TB_RENDER_TARGET_TYPE_VIEW )
    , m_BitArray()
    , m_Depend( view , backBufferIndex )
{
    setup();
}
 
/*!
 * コンストラクタ( テクスチャから生成 )
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbRenderTarget::TbRenderTarget( TbTexture* texture )
    : m_Type( TB_RENDER_TARGET_TYPE_TEXTURE )
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
void TbRenderTarget::setup()
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
TbRenderTarget::~TbRenderTarget()
{
}

/*!
 * 解像度横幅取得
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbRenderTarget::GetWidth() const
{
    return m_Depend.GetWidth();
}

/*!
 * 解像度縦幅取得
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbRenderTarget::GetHeight() const
{
    return m_Depend.GetHeight();
}

/*!
 * ピッチ幅取得
 * @author Miyake Shunsuke
 * @since 2011.10.30
 */
TbUint32 TbRenderTarget::GetPitch()
{
    IDirect3DSurface9* surface = m_Depend.GetSurface();

    D3DLOCKED_RECT lockedRect;
    if( FAILED( surface->LockRect( &lockedRect , nullptr , D3DLOCK_READONLY ) ) ){
        return TB_E_FAIL;
    }

    TbUint32 result = lockedRect.Pitch;
    surface->UnlockRect();

    return result;
}

/*!
 * フォーマット取得
 * @author Miyake Shunsuke
 * @since 2011.10.30
 */
TbColorFormat TbRenderTarget::GetFormat() const
{
    return m_Depend.GetFormat();
}

}

#endif