/*!
 * NUIカラーフレーム( KinectSDK for Windows )
 * @author teco
 */

#include "tb_fwd.h"
#include "nui/tb_nui_color_frame.h"
#include "nui/tb_nui_manager.h"
#include "nui/tb_nui_util.h"

#if TB_NUIAPI_IS_KINECT2

#define ENABLE_DIRECT_YUY2 (1)
#define ENABLE_COMMON_MULTI_SOURCE (0)

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbNuiColorFrame::TbNuiColorFrame( TbNuiImageResolutionType resolutionType )
    : m_Buffer( NULL )
    , m_BufferSize(0)
    , m_ResolutionType(resolutionType)
    , m_Resolution()
    , m_Pitch(0)
    , m_BytePerPixel(0)
{
    TB_ASSERT(resolutionType==TB_NUI_IMAGE_RESOLUTION_1920x1080 ||
              resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540 );

    m_BytePerPixel = sizeof(TbUint16);
    m_Resolution = TbNuiGetResolution(resolutionType);
    m_Pitch = m_Resolution.width * m_BytePerPixel;
    m_BufferSize = m_Pitch * m_Resolution.height;
    m_Depend.scaleResolution = resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540 ? 2 : 1;
#if !ENABLE_DIRECT_YUY2
    {
        TbDimensionS32 bufRes = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_1920x1080);
        m_Buffer = new TbUint16[bufRes.width*bufRes.height];
    }
#endif
}

/*!
 * デストラクタ
 * @author teco
 */
TbNuiColorFrame::~TbNuiColorFrame()
{
#if !ENABLE_COMMON_MULTI_SOURCE
    TB_SAFE_RELEASE(m_Depend.reader);
#endif
}

/*!
 * 準備できているか
 * @author teco
 */
TbBool TbNuiColorFrame::IsReady() const
{
    return m_Depend.isReady;
}

/*!
 * バッファをロックして取得
 * @author teco
 */
TbResult TbNuiColorFrame::Lock()
{
    return m_Buffer ? TB_S_OK : TB_E_FAIL;
}

/*!
 * バッファのロックを解除
 * @author teco
 */
TbResult TbNuiColorFrame::Unlock()
{
    return m_Buffer ? TB_S_OK : TB_E_FAIL;
}

/*!
 * ストリームを開く
 * @author teco
 */
TbResult TbNuiColorFrame::OpenStream()
{
    TB_SAFE_RELEASE(m_Depend.reader);
#if !ENABLE_COMMON_MULTI_SOURCE
    IColorFrameSource* source = nullptr;
    IKinectSensor* sensor = TbNuiManager::GetInstance().GetDepend().GetSensor();
    HRESULT hr = sensor->get_ColorFrameSource(&source);
    TB_RESQUE_RET(SUCCEEDED(hr),TB_E_FAIL);
    
    source->OpenReader(&m_Depend.reader);
    TB_SAFE_RELEASE(source);
    if( !TB_VERIFY(SUCCEEDED(hr)) ){
        return TB_E_FAIL;
    }
#endif
    return TB_S_OK;
}

/*!
 * ストリームを閉じる
 * @author teco
 */
TbResult TbNuiColorFrame::CloseStream()
{
    TB_SAFE_RELEASE(m_Depend.reader);
    return TB_S_OK;
}

#define TB_NUI_COLOR_CLIP(a) TbClamp<TbSint32>(a,0,255)

/*!
 * カラー取得
 * @author teco
 */
TbUint32 TbNuiColorFrame::GetColor( TbUint32 x , TbUint32 y ) const
{
    if( !m_Buffer ){
        return 0;
    }
    const TbUint8* bytes = reinterpret_cast<const TbUint8*>(m_Buffer);
    const TbDimensionS32& resSize = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_1920x1080);

    TbUint32 bx = x * m_Depend.scaleResolution;
    TbUint32 by = y * m_Depend.scaleResolution;
    TbUint32 index = (by<<1) * resSize.width + (bx<<1);

    const TbUint8* ptrIn = &bytes[ index ];
    TbSint32 y0 = ptrIn[0];
    TbSint32 u0 = ptrIn[1];
    TbSint32 y1 = ptrIn[2];

    TbSint32 v0 = ptrIn[3];

    TbSint32 c = y0 - 16;
    TbSint32 d = u0 - 128;
    TbSint32 e = v0 - 128;

    TbUint32 color = 0;
    color = TB_COLOR_A8R8G8B8(0xFF ,
                              TB_NUI_COLOR_CLIP(( 298 * c + 409 * e + 128) >> 8) , // red
                              TB_NUI_COLOR_CLIP(( 298 * c - 100 * d - 208 * e + 128) >> 8) , // green
                              TB_NUI_COLOR_CLIP(( 298 * c + 516 * d + 128) >> 8) ); // blue

    // 1920x1080未対応

    /*
    c = y1 - 16;
    ptrOut[3] = TB_NUI_COLOR_CLIP(( 298 * c + 516 * d + 128) >> 8); // blue
    ptrOut[4] = TB_NUI_COLOR_CLIP(( 298 * c - 100 * d - 208 * e + 128) >> 8); // green
    ptrOut[5] = TB_NUI_COLOR_CLIP(( 298 * c + 409 * e + 128) >> 8); // red
    ptrOut += 6;
    */

    return color;
}

/*!
 * 更新
 * @author teco
 */
void TbNuiColorFrame::Update()
{
#if !ENABLE_COMMON_MULTI_SOURCE
    if( !m_Depend.reader ){
        return;
    }
#endif
    IFrameDescription* frameDescription = nullptr;
    ColorImageFormat imageFormat = ColorImageFormat_None;
    RGBQUAD *buffer = nullptr;
    HRESULT hr = S_OK;
#if ENABLE_DIRECT_YUY2
#if !ENABLE_COMMON_MULTI_SOURCE
    TB_SAFE_RELEASE(m_Depend.frame);
#endif
    m_Buffer = nullptr;
#endif

#if !ENABLE_COMMON_MULTI_SOURCE
    hr = m_Depend.reader->AcquireLatestFrame(&m_Depend.frame);
    if( FAILED(hr) ){
        return;
    }
#else
    m_Depend.frame = TbNuiManager::GetInstance().GetDepend().GetColorFrame();
    hr = m_Depend.frame ? S_OK : E_FAIL;
#endif
    if (SUCCEEDED(hr)) {
        hr = m_Depend.frame->get_RawColorImageFormat(&imageFormat);
    }
    if (SUCCEEDED(hr) && imageFormat == ColorImageFormat_Yuy2 ) {
        m_Depend.isReady = TB_TRUE;
        UINT capa = 0;
        BYTE* buffer = nullptr;
        m_Depend.frame->AccessRawUnderlyingBuffer(&capa,&buffer);
#if ENABLE_DIRECT_YUY2
        m_Buffer = buffer;
#else
        TbMemCopy(m_Buffer,buffer,capa);
        TB_SAFE_RELEASE(m_Depend.frame);
#endif
//        colorFrame->CopyConvertedFrameDataToArray(m_BufferSize,reinterpret_cast<BYTE*>(m_Buffer),ColorImageFormat_Bgra);
    }
    TB_SAFE_RELEASE(frameDescription);
}



}

#endif