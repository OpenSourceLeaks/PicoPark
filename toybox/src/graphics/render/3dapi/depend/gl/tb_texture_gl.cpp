/*!
 * @file
 * @brief DirectX9の固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture.h"

#if TB_3DAPI_IS_GL

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
        return TB_NEW TbUint8[ size ];
    }

}

////////////////////////////////////////////////////

/*!
 * コンストラクタ( ファイルから生成 )
 * @author Miyake Shunsuke
 * @since 2010.09.20
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
 * 
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbTextureDepend::~TbTextureDepend()
{
    glDeleteTextures( 1 , &m_TextureId );
    TB_SAFE_DELETE_ARRAY( m_Buffer );
}

/*!
 * ファイルから生成
 * @author Miyake Shunsuke
 * @since 2011.08.08
 */
TbResult TbTextureDepend::setup( TbTexture* texture , const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType )
{
    glGenTextures( 1 , &m_TextureId );
    TbTexture* currentTexture = TbRenderDevice::GetInstance().GetTexture( 0 );
    glBindTexture( GL_TEXTURE_2D , m_TextureId );

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
        GLenum format;
        GLenum internalFormat;
        if( TbColorFormatToGLenum( &format , result.colorFormat ) && 
            TbColorFormatToTexInternalFormatGLenum(&internalFormat, result.colorFormat) ){

            glTexImage2D( GL_TEXTURE_2D , 0 , internalFormat , result.width , result.height , 0 , format , GL_UNSIGNED_BYTE , result.buffer );
        }
        m_Pitch = result.width * (TbGetColorFormatBitSize(result.colorFormat)/8);
        m_Buffer = reinterpret_cast<TbUint8*>(result.buffer);
        m_BufSize = result.bufSize;
        m_SurfaceInfo.dimension.width = result.width;
        m_SurfaceInfo.dimension.height = result.height;
        m_SurfaceInfo.format = result.colorFormat;
    }

    if( currentTexture ){ // 現在設定されているテクスチャがあれば元に戻す
        glBindTexture( GL_TEXTURE_2D , currentTexture->GetDepend().GetTextureId() );
    }

    return TB_S_OK;
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2011.08.08
 */
TbResult TbTextureDepend::setup( TbTexture* texture , const TbTextureDesc& desc )
{
    glGenTextures( 1 , &m_TextureId );
    TbTexture* currentTexture = TbRenderDevice::GetInstance().GetTexture( 0 );
    glBindTexture( GL_TEXTURE_2D , m_TextureId );

    m_SurfaceInfo.dimension.width = desc.width;
    m_SurfaceInfo.dimension.height = desc.height;
    m_SurfaceInfo.format = TbGetOptimizedColorFormat( desc.colorFormat );
    m_Pitch = desc.width * (TbGetColorFormatBitSize(desc.colorFormat)/8);
    m_BufSize = m_Pitch * desc.height;
    m_Buffer = TB_NEW TbUint8[ m_BufSize ];
    TbMemClear( m_Buffer , m_BufSize );

    GLenum format;
    if( TbColorFormatToGLenum(&format , m_SurfaceInfo.format) ){
        glTexImage2D( GL_TEXTURE_2D , 0 , format , desc.width , desc.height , 0 , format , GL_UNSIGNED_BYTE , m_Buffer );
    }
    if( currentTexture ){ // 現在設定されているテクスチャがあれば元に戻す
        glBindTexture( GL_TEXTURE_2D , currentTexture->GetDepend().GetTextureId() );
    }
    return TB_S_OK;
}

////////////////////////////////////////////////////

/*!
 * メモリに読み込んだファイルから生成
 * @author Miyake Shunsuke
 * @since 2010.09.26
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
 * @since 2010.09.26
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
 * @since 2011.05.27
 */
void TbTexture::setup()
{
    if( ( m_Info.flags | TbTextureDesc::FLAG_BIT_RENDER_TARGET ) && m_Depend.GetTextureId() > 0 ){
        m_RenderTarget = TB_NEW TbRenderTarget( this );
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbTexture::~TbTexture()
{
    TB_SAFE_DELETE(m_RenderTarget);
}

/*!
 * 利用可能
 * @author Miyake Shunsuke
 * @since 2011.08.11
 */
TbBool TbTexture::IsReady() const
{
    return m_Depend.m_TextureId > 0;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbResult TbTexture::Lock( const TbUint32 level , const TbUint32 flag , void** buffer , TbUint32& pitch , TbUint32& height )
{
    TB_UNUSE( level );
    if( buffer && !m_Depend.m_BitArray.Test( TbTextureDepend::FLAG_LOCK ) ){

        // 読み取り専用の場合、直接取得する
        if( flag & TB_LOCK_FLAG_BIT_READ_ONLY ){
#if !(TB_GLAPI_IS_GLES)  // Android版では直接は読みに行かない
            m_Depend.m_BitArray.SetBit( TbTextureDepend::FLAG_LOCK_READ_ONLY , TB_TRUE );

            TbTexture* currentTexture = TbRenderDevice::GetInstance().GetTexture( 0 );
            glBindTexture( GL_TEXTURE_2D , m_Depend.GetTextureId() );

            GLenum format;
            if( TbColorFormatToGLenum( &format , GetFormat( level ) ) ){
                glGetTexImage( GL_TEXTURE_2D , 0 , format , GL_UNSIGNED_BYTE , m_Depend.m_Buffer );
            }

            if( currentTexture && this != currentTexture ){ // 現在設定されているテクスチャがあれば元に戻す
                glBindTexture( GL_TEXTURE_2D , currentTexture->GetDepend().GetTextureId() );
            }
#endif
        }

        *buffer = m_Depend.m_Buffer;
        pitch = m_Depend.m_Pitch;
        height = GetHeight(level);
        m_Depend.m_BitArray.SetBit( TbTextureDepend::FLAG_LOCK , TB_TRUE );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbResult TbTexture::Unlock( const TbUint32 level )
{
    TB_UNUSE( level );
    if( m_Depend.m_BitArray.Test( TbTextureDepend::FLAG_LOCK ) ){

        if( m_Depend.m_TextureId > 0 && !m_Depend.m_BitArray.Test(TbTextureDepend::FLAG_LOCK_READ_ONLY) ) {
            glBindTexture( GL_TEXTURE_2D , m_Depend.m_TextureId );
            GLenum format;
            if( TbColorFormatToGLenum( &format , GetFormat( level ) ) ){
                glTexImage2D( GL_TEXTURE_2D , 0 , format , GetWidth(level) , GetHeight(level) , 0 , format , GL_UNSIGNED_BYTE , m_Depend.m_Buffer );
            }
        }

        m_Depend.m_BitArray.SetBit( TbTextureDepend::FLAG_LOCK , TB_FALSE );
        m_Depend.m_BitArray.SetBit( TbTextureDepend::FLAG_LOCK_READ_ONLY , TB_FALSE );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 解像度(横幅)
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbTexture::GetWidth( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Depend.GetWidth();
}

/*!
 * 解像度(縦幅)
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbTexture::GetHeight( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Depend.GetHeight();
}

/*!
 * フォーマット
 * @author Miyake Shunsuke
 * @since 2011.08.09
 */
TbColorFormat TbTexture::GetFormat( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Depend.GetFormat();
};

}

#endif