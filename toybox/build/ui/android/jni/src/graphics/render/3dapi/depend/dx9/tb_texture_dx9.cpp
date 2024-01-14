/*!
 * @file
 * @brief テクスチャ(DX9)
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"

namespace toybox
{

////////////////////////////////////////////////////
    
/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbTextureDepend::TbTextureDepend( const void* fileBuf , const TbUint32 bufSize )
    : TbLostResourceDx9( &TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_Type( TYPE_BUFFER )
    , m_BitArray()
    , m_Texture( nullptr )
    , m_RecoverBuf( nullptr )
    , m_RecoverBufSize( 0 )
    , m_LevelCount( 0U )
    , m_SurfaceInfo( nullptr )
{
    TbMemClear( &m_Desc , sizeof(m_Desc) );
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT result = D3DXCreateTextureFromFileInMemory( device.GetDepend().GetDeviceDX9() , fileBuf , bufSize , &m_Texture );
    if( result != S_OK ){
        m_BitArray.SetBit( FLAG_ERROR , TB_TRUE );
        TbPrintf("D3DXCreateTextureFromFileInMemory has error (%d)\n",result);
    }else{
        createSurfaceInfoList();
        m_RecoverBufSize = bufSize;
        m_RecoverBuf = TB_NEW TbUint8[ m_RecoverBufSize ];
        TbMemCopy( m_RecoverBuf , fileBuf , m_RecoverBufSize );
    }
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbTextureDepend::TbTextureDepend( const TbTextureDesc& desc )
    : TbLostResourceDx9( &TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_Type( TYPE_DYNAMIC )
    , m_BitArray()
    , m_Texture( nullptr )
    , m_RecoverBuf( nullptr )
    , m_RecoverBufSize( 0 )
    , m_LevelCount( 0U )
    , m_SurfaceInfo( nullptr )
{
    m_Desc = desc;
    createDynamic();
}

/*!
 * テクスチャ生成
 * @author Miyake Shunsuke
 * @since 2011.11.04
 */
void TbTextureDepend::createDynamic()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbBool isError = TB_TRUE;

    D3DPOOL pool = D3DPOOL_MANAGED; // レンダーターゲット、深度以外はMANAGED
    TbUint32 usage = 0U;
    if( m_Desc.flags & TbTextureDesc::FLAG_BIT_RENDER_TARGET ){
        usage |= D3DUSAGE_RENDERTARGET;
        pool = D3DPOOL_DEFAULT;
    }
    if( m_Desc.flags & TbTextureDesc::FLAG_BIT_DEPTH_STENCIL ){
        usage |= D3DUSAGE_DEPTHSTENCIL;
        pool = D3DPOOL_DEFAULT;
    }
#if 0
        // 未対応
        D3DUSAGE_DYNAMIC; // テクスチャサイズを動的変更
        D3DUSAGE_AUTOGENMIPMAP; // ミップマップ自動生成
#endif
    D3DFORMAT formatDx9 = TbColorFormatToD3DFORMAT(TbGetOptimizedColorFormat(m_Desc.colorFormat));
    HRESULT result =  device.GetDepend().GetDeviceDX9()->CreateTexture( m_Desc.width , 
                                                                        m_Desc.height , 
                                                                        m_Desc.levels ,
                                                                        usage , 
                                                                        formatDx9 , 
                                                                        pool, 
                                                                        &m_Texture ,
                                                                        nullptr );
    isError = ( S_OK != result );
    if( !isError ){
        createSurfaceInfoList();
    }else{
        TbPrintf("CreateTexture has error (%d)\n",result);
        TB_ASSERT_MSG( D3DERR_INVALIDCALL != result , "D3DERR_INVALIDCALL" );
    }
    // 未実装
    m_BitArray.SetBit( FLAG_ERROR , isError );
}

/*!
 * デストラクタ
 * 
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbTextureDepend::~TbTextureDepend()
{
    TB_SAFE_DELETE_ARRAY( m_RecoverBuf );
    TB_SAFE_DELETE_ARRAY( m_SurfaceInfo );
    TB_SAFE_RELEASE( m_Texture );
}

/*!
 * 解放
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbTextureDepend::Release()
{
    switch( m_Type ){
    case TYPE_BUFFER:
        TB_SAFE_RELEASE( m_Texture );
        break;
    case TYPE_DYNAMIC:
        TB_SAFE_RELEASE( m_Texture );
        break;
    }
}

/*!
 * 再構築
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbTextureDepend::Recover()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    switch( m_Type ){
    case TYPE_BUFFER:
        {
            HRESULT result = D3DXCreateTextureFromFileInMemory( device.GetDepend().GetDeviceDX9() , m_RecoverBuf , m_RecoverBufSize , &m_Texture );
        }
        break;
    case TYPE_DYNAMIC:
        // 復元はしない
        createDynamic();
        break;
    }
}

/*!
 * 解像度リスト生成
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
void TbTextureDepend::createSurfaceInfoList()
{
    // 生成済みなら何もしない
    if( m_SurfaceInfo ){
        return;
    }
    m_LevelCount = m_Texture ? m_Texture->GetLevelCount() : 0U;
    if( m_LevelCount > 0 ){
        m_SurfaceInfo = TB_NEW SurfaceInfo[m_LevelCount];
        for( TbUint32 level = 0; level < m_LevelCount; ++level ){
            TbUint32 width = 0;
            TbUint32 height = 0;
            TbColorFormat colorFormat = TB_COLOR_FORMAT_UNKNOWN;
            TbGetSurfaceInfoFromIDirect3DTexture9( width , height , colorFormat , m_Texture , level );
            m_SurfaceInfo[ level ].dimension.width = width;
            m_SurfaceInfo[ level ].dimension.height = height;
            m_SurfaceInfo[ level ].format = colorFormat;
        }
    }
}

////////////////////////////////////////////////////

/*!
 * メモリに読み込んだファイルから生成
 * @param fileBuf 画像ファイルバッファ
 * @param fileBuf 画像ファイルバッファサイズ
 * @param fileType ファイルタイプ
 * @param device 対応デバイス
 * @author Miyake Shunsuke
 * @since 2010.09.26
 */
TbTexture::TbTexture( const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType )
    : m_Info()
    , m_RenderTarget( nullptr )
    , m_DepthStencil( nullptr )
    , m_Depend( fileBuf , bufSize )
{
    TbMemClear( &m_Info , sizeof(m_Info) );
    if( m_Depend.m_Texture ){
        TbGetColorFormatFromIDirect3DTexture9( m_Info.colorFormat , m_Depend.m_Texture , 0 );
    }
}

/*!
 * 空のテクスチャ生成
 * @param desc 生成情報
 * @param device 対応デバイス
 * @author Miyake Shunsuke
 * @since 2010.09.26
 */
TbTexture::TbTexture( const TbTextureDesc& desc )
    : m_Info()
    , m_RenderTarget( nullptr )
    , m_DepthStencil( nullptr )
    , m_Depend( desc )
{
    TbMemCopy( &m_Info , &desc , sizeof(m_Info ) );
    if( desc.flags & TbTextureDesc::FLAG_BIT_RENDER_TARGET ){
        m_RenderTarget = TB_NEW TbRenderTarget( this );
    }
}

/*!
 * 利用可能
 * @author Miyake Shunsuke
 * @since 2011.08.11
 */
TbBool TbTexture::IsReady() const
{
    return !m_Depend.IsError();
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.26
 */
TbTexture::~TbTexture()
{
    TB_SAFE_DELETE( m_RenderTarget );
    TB_SAFE_DELETE( m_DepthStencil );
}

/*!
 * テクスチャロック
 * @param level レベル
 * @param buffer バッファ
 * @param pitch ピッチ
 * @param height 縦幅
 * @author Miyake Shunsuke
 * @since 2011.08.09
 */
TbResult TbTexture::Lock( const TbUint32 level , const TbUint32 flag , void** buffer , TbUint32& pitch , TbUint32& height )
{
    if( !buffer ){
        return TB_E_INVALIDARG;
    }
    if( !m_Depend.m_Texture ){
        return TB_E_FAIL;
    }
    if( ( flag & TB_LOCK_FLAG_BIT_READ_ONLY ) && 
        ( flag & TB_LOCK_FLAG_BIT_WRITE_ONLY ) )
    {
        // ありえない組み合わせ
        return TB_E_INVALIDARG;
    }
    DWORD flagDX9 = 0;
    if( flag & TB_LOCK_FLAG_BIT_READ_ONLY ){
        flagDX9 = D3DLOCK_READONLY;
    }else if( flag & TB_LOCK_FLAG_BIT_WRITE_ONLY ){
        flagDX9 = D3DLOCK_DISCARD;
    }
    D3DLOCKED_RECT rect;

    HRESULT hr = m_Depend.m_Texture->LockRect( level , &rect , nullptr , flagDX9 );
    if( !SUCCEEDED(hr) ){
        return TB_E_FAIL;
    }
    pitch = rect.Pitch;
    *buffer = rect.pBits;
    height = GetHeight( level );
    return TB_S_OK;
}

/*!
 * テクスチャロック解除
 * @param level レベル
 * @author Miyake Shunsuke
 * @since 2011.08.09
 */
TbResult TbTexture::Unlock( const TbUint32 level )
{
    if( !m_Depend.m_Texture ){
        return TB_E_FAIL;
    }
    m_Depend.m_Texture->UnlockRect( level );
    return TB_S_OK;
}

/*!
* 利用可能
* @author Miyake Shunsuke
*/
TbResult TbTexture::Write(const TbUint32 level, void* buffer, TbUint32 offset, TbUint32 size)
{
    TB_ASSERT(level == 0);
    void* lockBuffer = nullptr;
    TbUint32 height = 0;
    TbUint32 pitch = 0;
    Lock(level, 0, &lockBuffer, pitch, height);

    TbUint8* dst = reinterpret_cast<TbUint8*>(lockBuffer);
    TbMemCopy(&dst[offset], buffer, size);
    Unlock(level);
    return TB_S_OK;
}

// 白に書き込む
TbResult TbTexture::WriteWhite(const TbUint32 level)
{
    TB_ASSERT(level == 0);
    void* lockBuffer = nullptr;
    TbUint32 height = 0;
    TbUint32 pitch = 0;
    Lock(level, 0, &lockBuffer, pitch, height);

    TbUint8* dst = reinterpret_cast<TbUint8*>(lockBuffer);
    TbMemSet(dst, 0xFF, pitch*height);
    Unlock(level);
    return TB_S_OK;
}

/*!
 * 解像度(横幅)
 * @param level ミップマップレベル
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbTexture::GetWidth( const TbUint32 level ) const
{
    const TbTextureDepend::SurfaceInfo* surfaceInfo = m_Depend.GetSurfaceInfo( level );
    if( surfaceInfo ){
        return surfaceInfo->dimension.width;
    }
    return 0U;
}

/*!
 * 解像度(縦幅)
 * @param level ミップマップレベル
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbTexture::GetHeight( const TbUint32 level ) const
{
    const TbTextureDepend::SurfaceInfo* surfaceInfo = m_Depend.GetSurfaceInfo( level );
    if( surfaceInfo ){
        return surfaceInfo->dimension.height;
    }
    return 0U;
}

/*!
 * フォーマット取得
 * @param level ミップマップレベル
 * @author Miyake Shunsuke
 * @since 2011.08.09
 */
TbColorFormat TbTexture::GetFormat( const TbUint32 level ) const
{
    const TbTextureDepend::SurfaceInfo* surfaceInfo = m_Depend.GetSurfaceInfo( level );
    if( surfaceInfo ){
        return surfaceInfo->format;
    }
    return TB_COLOR_FORMAT_UNKNOWN;
}

}

#endif