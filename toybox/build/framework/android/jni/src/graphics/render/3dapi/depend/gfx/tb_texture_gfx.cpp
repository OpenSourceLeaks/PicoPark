/*!
 * @file
 * @brief GFX用テクスチャ
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture.h"

#if TB_3DAPI_IS_GFX

 //#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"
#include "graphics/image/tb_image_bmp.h"
#include "graphics/image/tb_image_tga.h"

namespace toybox
{

void* allockTextureBuffer(TbUint32 size)
{
    return TB_NEW TbUint8[size];
}

////////////////////////////////////////////////////

/*!
 * コンストラクタ( ファイルから生成 )
 * @author Miyake Shunsuke
 */
TbTextureDepend::TbTextureDepend()
    : m_BitArray()
    , m_SurfaceInfo()
    , m_Pitch(0U)
    , m_BufSize(0U)
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
    nn::gfx::Device* device = TbGetGfxDevice();
    m_DescriptorPool.Finalize();
    m_TextureView.Finalize(device);
    m_Texture.Finalize(device);
    m_MemoryPool.Finalize();
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
            TbImageBmp::Import( result , info , allockTextureBuffer);
        }
        break;
    case TB_TEXTURE_FILE_TYPE_TGA:
        {
            TbImageTga::Import( result , info , allockTextureBuffer );
        }
        break;
    default:
        TB_ASSERT(!"unsupported");
        break;
    }

    if( TB_VERIFY( result.buffer ) )
    {
        m_SurfaceInfo.dimension.width = result.width;
        m_SurfaceInfo.dimension.height = result.height;
        m_SurfaceInfo.format = result.colorFormat;
        m_Pitch = result.width * (TbGetColorFormatBitSize(result.colorFormat) / 8);
        m_BufSize = result.bufSize;

        nn::gfx::Device* device = TbGetGfxDevice();

        nn::gfx::ImageFormat format;
        if( TbColorFormatToImageFormat( &format , result.colorFormat ) ) {

            nn::gfx::Texture::InfoType info;
            info.SetDefault();
            info.SetGpuAccessFlags(nn::gfx::GpuAccess_Texture);
            info.SetWidth(result.width);
            info.SetHeight(result.height);
            info.SetImageStorageDimension(nn::gfx::ImageStorageDimension_2d);
            info.SetImageFormat(format);
            info.SetMipCount(1);
            info.SetTileMode(nn::gfx::TileMode_Linear);

            TbSizeT size = setupMemoryPool(info,TB_FALSE);

            void* dst = m_MemoryPool.Get()->Map();
            TbMemCopy(dst, result.buffer,m_BufSize);
            m_MemoryPool.Get()->FlushMappedRange(0, m_BufSize);
            m_MemoryPool.Get()->Unmap();
            m_Texture.Initialize(device,info, m_MemoryPool.Get(), 0, size);
            setupTextureView();
        }
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
    nn::gfx::Device* device = TbGetGfxDevice();

    nn::gfx::ImageFormat format;
    if (TbColorFormatToImageFormat(&format, desc.colorFormat)) {
        int gpuFlags = 0;
        TbBool isCompressible = TB_FALSE;
        nn::gfx::Texture::InfoType info;
        info.SetDefault();
        info.SetWidth(desc.width);
        info.SetHeight(desc.height);
        info.SetImageStorageDimension(nn::gfx::ImageStorageDimension_2d);
        info.SetImageFormat(format);
        info.SetMipCount(desc.levels);
        // レンダーターゲットしても使える
        if( desc.flags & TbTextureDesc::FLAG_BIT_RENDER_TARGET ){
            gpuFlags |= nn::gfx::GpuAccess_ColorBuffer;
        }
        if (desc.flags & TbTextureDesc::FLAG_BIT_DEPTH_STENCIL) {
            gpuFlags = nn::gfx::GpuAccess_DepthStencil;
            isCompressible = TB_TRUE;
        } else {
            gpuFlags |= nn::gfx::GpuAccess_Texture;
            info.SetTileMode(nn::gfx::TileMode_Linear);
        }
        info.SetGpuAccessFlags(gpuFlags);
        TbSizeT size = setupMemoryPool(info, isCompressible);
        if (gpuFlags & nn::gfx::GpuAccess_Texture) {
            // 白色にする
            void* dst = m_MemoryPool.Get()->Map();
            TbMemSet(dst, 0xFF , m_BufSize);
            m_MemoryPool.Get()->FlushMappedRange(0, m_BufSize);
            m_MemoryPool.Get()->Unmap();
        }
        m_Texture.Initialize(device, info, m_MemoryPool.Get(), 0, size);
        setupTextureView();
    }
    return TB_S_OK;
}

/*!
 * メモリープールセットアップ
 * @author Miyake Shunsuke
 */
TbSizeT TbTextureDepend::setupMemoryPool( nn::gfx::Texture::InfoType& info , TbBool isCompressible )
{
    nn::gfx::Device* device = TbGetGfxDevice();
    size_t alignment = nn::gfx::Texture::CalculateMipDataAlignment(device, info);
    size_t size = nn::gfx::Texture::CalculateMipDataSize(device, info);
    size_t pitch = nn::gfx::Texture::GetRowPitch(device, info);
    TB_ASSERT(m_Pitch == pitch);
    TB_ASSERT(m_BufSize <= size);

    // メモリープールセットアップ
    {
        int property = 0;
        if(isCompressible ){
            property =  nn::gfx::MemoryPoolProperty_CpuInvisible | 
                        nn::gfx::MemoryPoolProperty_GpuCached |
                        nn::gfx::MemoryPoolProperty_Compressible;
        }else {
            property = nn::gfx::MemoryPoolProperty_CpuCached | nn::gfx::MemoryPoolProperty_GpuCached;
        }
        m_MemoryPool.Initialize(property, size, alignment);
    }
    return size;
}

/*!
 * テクスチャビュー作成
 * @author Miyake Shunsuke
 */
void TbTextureDepend::setupTextureView()
{
    nn::gfx::ImageFormat format;
    if (TbColorFormatToImageFormat(&format, m_SurfaceInfo.format))
    {
        nn::gfx::Device* device = TbGetGfxDevice();
        nn::gfx::TextureView::InfoType info;
        info.SetDefault();
        info.SetImageDimension(nn::gfx::ImageDimension_2d);
        info.SetImageFormat(format);
        info.SetTexturePtr(&m_Texture);
        info.SetChannelMapping(nn::gfx::ChannelMapping_Red, nn::gfx::ChannelMapping_Green,
            nn::gfx::ChannelMapping_Blue, nn::gfx::ChannelMapping_Alpha);
        m_TextureView.Initialize(device, info);

        m_DescriptorPool.InitializeAsTexture(&m_TextureView);
    }
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
    , m_LockedBuffer( nullptr )
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
    , m_LockedBuffer( nullptr )
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
    if( ( m_Info.flags & TbTextureDesc::FLAG_BIT_RENDER_TARGET ) && m_Depend.GetTextureView() ){
        m_RenderTarget = TB_NEW TbRenderTarget( this );
    }
    if ((m_Info.flags & TbTextureDesc::FLAG_BIT_DEPTH_STENCIL) && m_Depend.GetTextureView()) {
        m_DepthStencil = TB_NEW TbDepthStencil(this);
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbTexture::~TbTexture()
{
    TB_ASSERT(!m_LockedBuffer);
    TB_SAFE_DELETE(m_RenderTarget);
}

/*!
 * 利用可能
 * @author Miyake Shunsuke
 */
TbBool TbTexture::IsReady() const
{
    return m_Depend.m_MemoryPool.IsInitialize();
}

/*!
 * 利用可能
 * @author Miyake Shunsuke
 */
TbResult TbTexture::Write(const TbUint32 level, void* buffer, TbUint32 offset, TbUint32 size)
{
    TB_ASSERT(level == 0);
    TbBool isLocked = m_LockedBuffer != nullptr;
    if (!isLocked) {
        void* lockBuffer =  nullptr;
        TbUint32 height = 0;
        TbUint32 pitch = 0;
        Lock(level,0,&lockBuffer,pitch,height);
    }
    TbUint8* dst = reinterpret_cast<TbUint8*>(m_LockedBuffer);
    TbMemCopy(&dst[offset], buffer , size );
    if (!isLocked) {
        Unlock(level);
    }
    return TB_S_OK;
}

// 白に書き込む
TbResult TbTexture::WriteWhite(const TbUint32 level)
{
    TB_ASSERT(level == 0);
    TbBool isLocked = m_LockedBuffer != nullptr;
    if (!isLocked) {
        void* lockBuffer = nullptr;
        TbUint32 height = 0;
        TbUint32 pitch = 0;
        Lock(level, 0, &lockBuffer, pitch, height);
    }
    if (m_LockedBuffer)
    {
        TbUint8* dst = reinterpret_cast<TbUint8*>(m_LockedBuffer);
        TbUint32 pitch = m_Depend.m_Pitch;
        TbUint32 height = m_Depend.GetHeight();
        TbUint8* src = new TbUint8[pitch*height];
        TbMemSet(src, 0xFF, pitch*height);
        TbMemCopy(dst, src, pitch*height);
//        TbMemSet(dst, 0xFF, pitch*height);
        TB_SAFE_DELETE_ARRAY(src);
    }

    if (!isLocked) {
        Unlock(level);
    }
    return TB_S_OK;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 */
TbResult TbTexture::Lock( const TbUint32 level , const TbUint32 flag , void** buffer , TbUint32& pitch , TbUint32& height )
{
    TB_UNUSE( level );
    TB_ASSERT(level == 0);
    if(!m_LockedBuffer){
        m_LockedBuffer = m_Depend.m_MemoryPool.Get()->Map();
        *buffer = m_LockedBuffer;
        pitch = m_Depend.m_Pitch;
        height = m_Depend.GetHeight();
    }
    return TB_E_FAIL;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 */
TbResult TbTexture::Unlock( const TbUint32 level )
{
    TB_ASSERT(level == 0);
    if(m_LockedBuffer)
    {
        m_Depend.m_MemoryPool.Get()->FlushMappedRange(0, m_Depend.m_BufSize);
        m_Depend.m_MemoryPool.Get()->Unmap();
        m_LockedBuffer = nullptr;
    }
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