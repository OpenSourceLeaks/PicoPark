/*!
 * @file
 * @brief DirectX9の固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture.h"

#if TB_3DAPI_IS_GX2

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/depend/gl/tb_render_util_gl.h"
#include "graphics/image/tb_image_bmp.h"
#include "graphics/image/tb_image_tga.h"

namespace toybox
{

////////////////////////////////////////////////////

namespace
{

    void* allockTextureBuffer( TbUint32 size )
    {
        return TbGX2Alloc(size,4);
    }

}

////////////////////////////////////////////////////

/*!
 * コンストラクタ( ファイルから生成 )
 * @author Miyake Shunsuke
 */
TbTextureDepend::TbTextureDepend()
    : m_BitArray()
    , m_TextureId( 0 )
    , m_Buffer( nullptr )
    , m_BufSize( 0U )
    , m_Pitch( 0U )
    , m_SurfaceInfo()
{
    m_SurfaceInfo.dimension.width = 0;
    m_SurfaceInfo.dimension.height = 0;
    m_SurfaceInfo.format = TB_COLOR_FORMAT_UNKNOWN;
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbTextureDepend::~TbTextureDepend()
{
    if( m_Buffer ){
        TbGX2Free(m_Buffer);
    }
}

/*!
 * ファイルから生成
 * @author Miyake Shunsuke
 */
TbResult TbTextureDepend::setup( TbTexture* texture , const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType )
{
    TbImageImportInfo info;
    info.fileBuf =  fileBuf;
    info.fileBufSize = bufSize;
    info.colorFormat = TB_COLOR_FORMAT_A8B8G8R8;
    TbImageImportResult result;
    TbMemClear( &result , sizeof(result) );
    
    switch( fileType )
    {
    case TB_TEXTURE_FILE_TYPE_BMP:
        {
            TbImageBmp::Import( result , info , allockTextureBuffer );
        }
        break;
    case TB_TEXTURE_FILE_TYPE_TGA:
        {
            TbImageTga::Import( result , info , allockTextureBuffer );
        }
        break;
    }

    if( TB_VERIFY( result.buffer ) )
    {
        GX2SurfaceFormat format;
        if( TbColorFormatToGX2SurfaceFormat( &format , result.colorFormat ) ) {
            GX2InitTexture(&m_Texture,
                           result.width,  // width
                           result.height,  // height
                           1,  // depth
                           0,  // num mips
                           format,
                           GX2_SURFACE_DIM_2D);
            m_Texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
            GX2CalcSurfaceSizeAndAlignment(&m_Texture);
            GX2InitTextureRegs(&m_Texture);
            GX2InitTexturePtrs(&m_Texture,result.buffer,nullptr);
        }
        m_Pitch = result.width * (TbGetColorFormatBitSize(result.colorFormat)/4);
        m_Buffer = reinterpret_cast<TbUint8*>(result.buffer);
        m_BufSize = result.bufSize;
        m_SurfaceInfo.dimension.width = result.width;
        m_SurfaceInfo.dimension.height = result.height;
        m_SurfaceInfo.format = result.colorFormat;
        GX2Invalidate(GX2_INVALIDATE_CPU_TEXTURE, m_Texture.surface.imagePtr, m_Texture.surface.imageSize);
    }

    return TB_S_OK;
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 */
TbResult TbTextureDepend::setup( TbTexture* texture , const TbTextureDesc& desc )
{
    m_SurfaceInfo.dimension.width = desc.width;
    m_SurfaceInfo.dimension.height = desc.height;
    m_SurfaceInfo.format = TbGetOptimizedColorFormat( desc.colorFormat );
    m_Pitch = desc.width * (TbGetColorFormatBitSize(desc.colorFormat)/8);
    m_BufSize = m_Pitch * desc.height;
    GX2SurfaceFormat format;
    if( TbColorFormatToGX2SurfaceFormat( &format , result.colorFormat ) ) {
        GX2InitTexture(&m_Texture,
                       desc.width,  // width
                       desc.height,  // height
                       1,  // depth
                       0,  // num mips
                       format,
                       GX2_SURFACE_DIM_2D);

        // レンダーターゲットしても使える
        if( desc.flags | TbTextureDesc::FLAG_BIT_RENDER_TARGET ){
            m_Texture.surface.use = GX2_SURFACE_USE_COLOR_BUFFER_TEXTURE;
        }
        
        m_Texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
        GX2CalcSurfaceSizeAndAlignment(&m_Texture);
        m_BufSize = m_Texture.surface.imageSize;
        m_Buffer = TbGX2Alloc(m_Texture.surface.imageSize,
                              m_Texture.surface.alignment);
        TbMemClear( m_Buffer , m_BufSize );
        GX2InitTextureRegs(&m_Texture);
        GX2InitTexturePtrs(&m_Texture,m_Buffer,nullptr);
        GX2Invalidate(GX2_INVALIDATE_CPU_TEXTURE, m_Texture.surface.imagePtr, m_Texture.surface.imageSize);
    }
    return TB_S_OK;
}

////////////////////////////////////////////////////

/*!
 * メモリに読み込んだファイルから生成
 * @author Miyake Shunsuke
 */
TbTexture::TbTexture( const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType )
    : m_Info()
    , m_RenderTarget( nullptr )
    , m_DepthStencil( nullptr )
    , m_Depend()
{
    TbMemClear( &m_Info , sizeof(m_Info ) );
    m_Depend.setup( this , fileBuf , bufSize , fileType );
    setup();
}

/*!
 * 空のテクスチャ生成
 * @author Miyake Shunsuke
 */
TbTexture::TbTexture( const TbTextureDesc& desc )
    : m_Info()
    , m_RenderTarget( nullptr )
    , m_DepthStencil( nullptr )
    , m_Depend()
{
    TbMemCopy( &m_Info , &desc , sizeof(m_Info ) );
    m_Depend.setup( this , desc );
    setup();
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 */
void TbTexture::setup()
{
    if( ( m_Info.flags & TbTextureDesc::FLAG_BIT_RENDER_TARGET ) && m_Depend.GetTextureId() > 0 ){
        m_RenderTarget = TB_NEW TbRenderTarget( this );
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbTexture::~TbTexture()
{
    TB_SAFE_DELETE(m_RenderTarget);
}

/*!
 * 利用可能
 * @author Miyake Shunsuke
 */
TbBool TbTexture::IsReady() const
{
    return m_Depend.m_TextureId > 0;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 */
TbResult TbTexture::Lock( const TbUint32 level , const TbUint32 flag , void** buffer , TbUint32& pitch , TbUint32& height )
{
    TB_UNUSE( level );
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 */
TbResult TbTexture::Unlock( const TbUint32 level )
{
    TB_UNUSE( level );
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * 解像度(横幅)
 * @author Miyake Shunsuke
 */
TbUint32 TbTexture::GetWidth( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Depend.GetWidth();
}

/*!
 * 解像度(縦幅)
 * @author Miyake Shunsuke
 */
TbUint32 TbTexture::GetHeight( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Depend.GetHeight();
}

/*!
 * フォーマット
 * @author Miyake Shunsuke
 */
TbColorFormat TbTexture::GetFormat( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Depend.GetFormat();
};

}

#endif