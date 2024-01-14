/*!
 * テクスチャ実装(Stage3D依存)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_texture.h"

#if TB_3DAPI_IS_S3D

#include "graphics/3dapi/tb_render_device.h"
#include "graphics/3dapi/tb_depth_stencil.h"
#include "graphics/3dapi/depend/gl/tb_render_util_gl.h"
#include "graphics/image/tb_image_bmp.h"
#include "graphics/image/tb_image_tga.h"

using namespace  AS3::ui::flash::display3D;

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
    , m_Texture()
    , m_Buffer( NULL )
    , m_BufSize( 0U )
    , m_Pitch( 0U )
{
}

/*!
 * デストラクタ
 * 
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbTextureDepend::~TbTextureDepend()
{
    TB_SAFE_DELETE_ARRAY( m_Buffer );
}

/*!
 * ファイルから生成
 * @author Miyake Shunsuke
 * @since 2011.08.08
 */
TbResult TbTextureDepend::setup( TbTexture* texture , const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType )
{
    TbImageImportInfo info;
    info.fileBuf =  fileBuf;
    info.fileBufSize = bufSize;
    info.colorFormat = TB_COLOR_FORMAT_B8G8R8A8;
    
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

    if( TB_VERIFY( result.buffer && result.colorFormat == TB_COLOR_FORMAT_B8G8R8A8 ) )
    {
        AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
        m_Texture = context->createTexture( result.width , result.height ,Context3DTextureFormat::BGRA , false );
        m_Pitch = result.width * (TbGetColorFormatByteSize(result.colorFormat));
        m_Buffer = reinterpret_cast<TbUint8*>(result.buffer);
        m_BufSize = result.bufSize;
        m_Texture->uploadFromByteArray(AS3::ui::internal::get_ram(),reinterpret_cast<int>(m_Buffer),0,m_Buffer);
        texture->m_Info.width = result.width;
        texture->m_Info.height = result.height;
        texture->m_Info.colorFormat = result.colorFormat;
    }else{
        return TB_E_FAIL;
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
    if( desc.colorFormat != TB_COLOR_FORMAT_A8R8G8B8 ){
        return TB_E_FAIL;
    }
    
    AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
    m_Texture = context->createTexture( desc.width , desc.height , Context3DTextureFormat::BGRA , true );
    m_Pitch = desc.width * (TbGetColorFormatBitSize(desc.colorFormat)/8);
    m_BufSize = m_Pitch * desc.height;
    m_Buffer = TB_NEW TbUint8[ m_BufSize ];
    TbMemClear( m_Buffer , m_BufSize );

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
    , m_RenderTarget( NULL )
    , m_DepthStencil( NULL )
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
    , m_RenderTarget( NULL )
    , m_DepthStencil( NULL )
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
    if( ( m_Info.flags & TbTextureDesc::FLAG_BIT_RENDER_TARGET ) ){
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
}

/*!
 * 利用可能
 * @author Miyake Shunsuke
 * @since 2011.08.11
 */
TbBool TbTexture::IsReady() const
{
    TbTextureDepend::TextureS3d& tex = const_cast<TbTextureDepend::TextureS3d&>(m_Depend.GetTexture());
    return tex != TbTextureDepend::TextureS3d();
}

/*!
 * テクスチャ書き込み
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */
TbResult TbTexture::Write( const TbUint32 level , void* buffer , TbUint32 offset , TbUint32 size )
{
    // 未サポート
    return TB_E_FAIL;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbResult TbTexture::Lock( const TbUint32 level , const TbUint32 flag , void** buffer , TbUint32& pitch , TbUint32& height )
{
    // 未サポート
    return TB_E_FAIL;
}

/*!
 * テクスチャロック
 * @author Miyake Shunsuke
 * @since 2011.05.25
 */
TbResult TbTexture::Unlock( const TbUint32 level )
{
    // 未サポート
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
    return m_Info.width;
}

/*!
 * 解像度(縦幅)
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbTexture::GetHeight( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Info.height;
}

/*!
 * フォーマット
 * @author Miyake Shunsuke
 * @since 2011.08.09
 */
TbColorFormat TbTexture::GetFormat( const TbUint32 level ) const
{
    TB_UNUSE( level );
    return m_Info.colorFormat;
};

}

#endif