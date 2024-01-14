/*!
 * NUIイメージフレーム( KinectSDK for Windows )
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */

#include <tb_fwd.h>
#include <nui/tb_nui_types.h>

#if TB_NUIAPI_IS_KINECT

#include <nui/depend/kinect/tb_nui_image_frame.h>
namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
TbNuiImageFrame::TbNuiImageFrame( TbNuiImageType type , TbNuiImageResolutionType resolutionType )
    : m_Type( type )
    , m_ResolutionType( resolutionType )
    , m_BitArray()
    , m_Resolution()
    , m_Pitch( 0 )
    , m_BytePerPixel( 0 )
    , m_CurrentImageFrameIndex( 0 )
    , m_CurrentImageFrameIndexOffset( 0 )
    , m_StreamHandle( NULL )
{
    TbMemClear( m_ImageFrame , sizeof(m_ImageFrame) );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
TbNuiImageFrame::~TbNuiImageFrame()
{
    TB_ASSERT( TB_S_OK == CloseStream() );
}

/*!
 * ストリームを開く
 * @retval TB_S_OK 成功
 * @retval TB_E_FAIL 失敗
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
TbResult TbNuiImageFrame::OpenStream()
{
    if( m_StreamHandle ){
        return TB_E_FAIL;
    }
    struct ImageTypeInfo {
        NUI_IMAGE_TYPE type;
        TbUint32 bytesPerPixel;
    };
    struct ResolutionInfo{
        NUI_IMAGE_RESOLUTION type;
        TbSint32 width;
        TbSint32 height;
    };
    static const ImageTypeInfo IMAGE_INFO_TABLE[] = {
        { NUI_IMAGE_TYPE_COLOR , 4 } , 
        { NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX , 2 }
    };
    static const ResolutionInfo RESOLUTION_INFO_TABLE[] = {
        { NUI_IMAGE_RESOLUTION_80x60 , 80 , 60 } , 
        { NUI_IMAGE_RESOLUTION_320x240 , 320 , 240 } , 
        { NUI_IMAGE_RESOLUTION_INVALID , 0 , 0 } , // 未サポート
        { NUI_IMAGE_RESOLUTION_640x480 , 640 , 480 } , 
        { NUI_IMAGE_RESOLUTION_INVALID , 0 , 0 } , // 未サポート
        { NUI_IMAGE_RESOLUTION_1280x960 , 1280 , 960 } ,
        { NUI_IMAGE_RESOLUTION_INVALID , 0 , 0 } // 未サポート
    };
    TB_RESQUE_RET(RESOLUTION_INFO_TABLE[ m_ResolutionType ].width!=0,TB_E_FAIL);
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF( IMAGE_INFO_TABLE ) == TB_NUI_IMAGE_TYPE_MAX );
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF( RESOLUTION_INFO_TABLE ) == TB_NUI_IMAGE_RESOLUTION_MAX );
    HRESULT hr = NuiImageStreamOpen( IMAGE_INFO_TABLE[ m_Type ].type ,
                                     RESOLUTION_INFO_TABLE[ m_ResolutionType ].type ,
                                     0 ,
                                     FRAME_MAX ,
                                     NULL , 
                                     &m_StreamHandle );
    if( !SUCCEEDED(hr) ){
        return TB_E_FAIL;
    }
    m_BytePerPixel = IMAGE_INFO_TABLE[ m_Type ].bytesPerPixel;
    m_BitArray.SetBit( FLAG_READY , TB_TRUE );
    return TB_S_OK;
}

/*!
 * ストリームを閉じる
 * @retval TB_S_OK 成功
 * @retval TB_E_FAIL 失敗
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
TbResult TbNuiImageFrame::CloseStream()
{
    if( !m_StreamHandle ){
        return TB_E_FAIL;
    }
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF( m_ImageFrame ); ++i ){
        if( m_ImageFrame[i] ){
            HRESULT hr = NuiImageStreamReleaseFrame( m_StreamHandle , m_ImageFrame[i] );
            if( !TB_VERIFY( SUCCEEDED(hr) ) ){
                return TB_E_FAIL;
            }
            m_ImageFrame[i] = NULL;
        }
    }
    m_BytePerPixel = 0;
    m_Pitch = 0;
    m_StreamHandle = NULL;
    m_BitArray.SetBit( FLAG_READY , TB_FALSE );
    return TB_S_OK;
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
void TbNuiImageFrame::Update()
{
    if( !m_StreamHandle ){
        return;
    }
    HRESULT hr = E_FAIL;
    const NUI_IMAGE_FRAME* frame = NULL;

    hr = NuiImageStreamGetNextFrame( m_StreamHandle , 0, &frame );
    if ( SUCCEEDED( hr ) ){
        m_CurrentImageFrameIndex = ( m_CurrentImageFrameIndex + 1 ) % FRAME_MAX;
        if( m_ImageFrame[ m_CurrentImageFrameIndex ] ){
            hr = NuiImageStreamReleaseFrame( m_StreamHandle , m_ImageFrame[ m_CurrentImageFrameIndex ] );
        }
        m_ImageFrame[ m_CurrentImageFrameIndex ] = frame;

        // サーフェイス情報更新
        if( m_ImageFrame[ m_CurrentImageFrameIndex ] ){
            NUI_SURFACE_DESC surfaceDesc;
            if( SUCCEEDED(m_ImageFrame[ m_CurrentImageFrameIndex ]->pFrameTexture->GetLevelDesc( 0 , &surfaceDesc )) ){
                m_Resolution.width = surfaceDesc.Width;
                m_Resolution.height = surfaceDesc.Height;
            }
            m_Pitch = m_ImageFrame[ m_CurrentImageFrameIndex ]->pFrameTexture->Pitch();
        }
    }
}

/*!
 * バッファをロックして取得
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
void* TbNuiImageFrame::LockBuffer()
{
    // 参照先決定
   const NUI_IMAGE_FRAME* frame = GetCurrentImageFrame();
 
    // ロック
    if( m_BitArray.Test( FLAG_LOCK_BUFFER ) ){
        return NULL;
    }
    void* result = NULL;
    if( frame ){
        NUI_LOCKED_RECT rect;
        frame->pFrameTexture->LockRect( 0 , &rect , NULL , 0 );
        result = rect.pBits;
        m_BitArray.SetBit( FLAG_LOCK_BUFFER , TB_TRUE );
    }
    return result;
}

/*! 
 * バッファのロックを解除
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */
TbResult TbNuiImageFrame::UnlockBuffer()
{
    // 参照先決定
    const NUI_IMAGE_FRAME* frame = GetCurrentImageFrame();
 
    // ロック解除
    if( frame ){
        frame->pFrameTexture->UnlockRect( 0 );
        m_BitArray.SetBit( FLAG_LOCK_BUFFER , TB_FALSE );
    }
    return TB_S_OK;
}

/*!
 * 現在のイメージ
 * @author Miyake Shunsuke
 * @since 2011.10.14
 */
const NUI_IMAGE_FRAME* TbNuiImageFrame::GetCurrentImageFrame() const
{
    TbUint32 index = m_CurrentImageFrameIndex;
    if( m_CurrentImageFrameIndexOffset > m_CurrentImageFrameIndex ){
        index = index + FRAME_MAX - m_CurrentImageFrameIndexOffset;
    }else{
        index -= m_CurrentImageFrameIndexOffset;
    }
    return m_ImageFrame[index];
}

}
#endif