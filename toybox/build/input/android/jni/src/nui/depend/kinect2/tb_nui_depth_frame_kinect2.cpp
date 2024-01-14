/*!
 * NUI深度フレーム( KinectSDK for Windows )
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */

#include "tb_fwd.h"
#include "nui/tb_nui_depth_frame.h"
#include "nui/tb_nui_manager.h"
#include "nui/tb_nui_util.h"

#if TB_NUIAPI_IS_KINECT2

#define ENABLE_COMMON_MULTI_SOURCE (0)

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbNuiDepthFrame::TbNuiDepthFrame( TbNuiImageResolutionType resolutionType )
    : m_Buffer( NULL )
    , m_BufferSize(0)
    , m_ResolutionType(resolutionType)
    , m_Resolution()
    , m_Pitch(0)
    , m_BytePerPixel(0)
{
    TB_ASSERT(resolutionType==TB_NUI_IMAGE_RESOLUTION_512x424);

    m_BytePerPixel = sizeof(TbUint16);
    m_Resolution = TbNuiGetResolution(resolutionType);
    m_Pitch = m_Resolution.width * m_BytePerPixel;
    m_BufferSize = m_Pitch * m_Resolution.height;
}

/*!
 * デストラクタ
 * @author teco
 */
TbNuiDepthFrame::~TbNuiDepthFrame()
{
#if !ENABLE_COMMON_MULTI_SOURCE
    TB_SAFE_RELEASE(m_Depend.depthFrame);
    TB_SAFE_RELEASE(m_Depend.bodyIndexFrame);
#endif
    TB_SAFE_RELEASE(m_Depend.depthReader);
    TB_SAFE_RELEASE(m_Depend.bodyIndexFrameReader);
}

/*!
 * 準備できているか
 * @author teco
 */
TbBool TbNuiDepthFrame::IsReady() const
{
    return m_Depend.isReady;
}

/*!
 * ロック
 * @author teco
 */
TbResult TbNuiDepthFrame::Lock()
{
    return m_Buffer ? TB_S_OK : TB_E_FAIL;
}

/*!
 * ロック解除
 * @author teco
 */
TbResult TbNuiDepthFrame::Unlock()
{
    return m_Buffer ? TB_S_OK : TB_E_FAIL;
}

/*!
 * 深度取得
 * @param x X座標
 * @param y Y座標
 * @return 深度値
 * @author teco
 */
TbUint16 TbNuiDepthFrame::GetDepth( TbSint32 x , TbSint32 y , TbSint16* playerIndex )
{
    TbUint16 result = 0;
    if( !m_Buffer ){
        return result;
    }
    if( 0 <= x && x < m_Resolution.width && 0 <= y && y < m_Resolution.height )
    {
        TbUint16* buffer = reinterpret_cast<TbUint16*>(m_Buffer);
        TbUint32 index = (y * m_Resolution.width) + x;
        result = buffer[ index ];
        if( playerIndex && m_Depend.bodyIndexBuffer ){
            TbUint8 value = m_Depend.bodyIndexBuffer[index];
            if( 0xFF != value ){
                *playerIndex = value;
            }
        }
    }
    return result;
}

/*!
 * ストリームを開く
 * @author teco
 */
TbResult TbNuiDepthFrame::OpenStream()
{
    TB_SAFE_RELEASE(m_Depend.depthReader);
    TB_SAFE_RELEASE(m_Depend.bodyIndexFrameReader);

#if !ENABLE_COMMON_MULTI_SOURCE
    IKinectSensor* sensor = TbNuiManager::GetInstance().GetDepend().GetSensor();

    {
        IDepthFrameSource* source = nullptr;
        IKinectSensor* sensor = TbNuiManager::GetInstance().GetDepend().GetSensor();
        HRESULT hr = sensor->get_DepthFrameSource(&source);
        TB_RESQUE_RET(SUCCEEDED(hr),TB_E_FAIL);
        source->OpenReader(&m_Depend.depthReader);
        TB_SAFE_RELEASE(source);
    }
    {
        IBodyIndexFrameSource* source = nullptr;
        IKinectSensor* sensor = TbNuiManager::GetInstance().GetDepend().GetSensor();
        HRESULT hr = sensor->get_BodyIndexFrameSource(&source);
        TB_RESQUE_RET(SUCCEEDED(hr),TB_E_FAIL);
        source->OpenReader(&m_Depend.bodyIndexFrameReader);
        TB_SAFE_RELEASE(source);
    }
#endif
    return TB_S_OK;
}

/*!
 * ストリームを閉じる
 * @author teco
 */
TbResult TbNuiDepthFrame::CloseStream()
{
    TB_SAFE_RELEASE(m_Depend.depthReader);
    TB_SAFE_RELEASE(m_Depend.bodyIndexFrameReader);
    return TB_S_OK;
}

// 
/*!
 * 更新
 * @author teco
 */
void TbNuiDepthFrame::Update()
{
#if !ENABLE_COMMON_MULTI_SOURCE
    if( !m_Depend.depthReader || !m_Depend.bodyIndexFrameReader ){
        return;
    }
#endif

    INT64 time = 0;
    UINT    bufferSize = 0;
    RGBQUAD *buffer = nullptr;
    USHORT minValue = 0;
    USHORT maxValue = 0;
    m_Buffer = nullptr;
    m_Depend.bodyIndexBuffer = nullptr;
    HRESULT hr = S_OK;
    IFrameDescription* frameDescription = nullptr;

#if !ENABLE_COMMON_MULTI_SOURCE
    TB_SAFE_RELEASE(m_Depend.depthFrame);
    TB_SAFE_RELEASE(m_Depend.bodyIndexFrame);

    if (SUCCEEDED(hr))
    {
        hr = m_Depend.depthReader->AcquireLatestFrame(&m_Depend.depthFrame);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_Depend.bodyIndexFrameReader->AcquireLatestFrame(&m_Depend.bodyIndexFrame);
    }
#else
    m_Depend.depthFrame = TbNuiManager::GetInstance().GetDepend().GetDepthFrame();
    m_Depend.bodyIndexFrame = TbNuiManager::GetInstance().GetDepend().GetBodyIndexFrame(); 
    if( !m_Depend.depthFrame || !m_Depend.bodyIndexFrame ){
        hr = E_FAIL;
    }
#endif

    if (SUCCEEDED(hr)) {
       hr = m_Depend.depthFrame->get_RelativeTime(&time);
    }
    if (SUCCEEDED(hr)) {
        hr = m_Depend.depthFrame->get_FrameDescription(&frameDescription);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_Depend.depthFrame->get_DepthMinReliableDistance(&minValue);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_Depend.depthFrame->get_DepthMaxReliableDistance(&maxValue);
    }
    if (SUCCEEDED(hr)) {
        m_Depend.isReady = TB_TRUE;
        UINT capa = 0;
        hr = m_Depend.depthFrame->AccessUnderlyingBuffer(&capa,reinterpret_cast<UINT16**>(&m_Buffer));
    }
    if (SUCCEEDED(hr)) {
        UINT capa = 0;
        hr = m_Depend.bodyIndexFrame->AccessUnderlyingBuffer(&capa,reinterpret_cast<BYTE**>(&m_Depend.bodyIndexBuffer));
    }
    TB_SAFE_RELEASE(frameDescription);
}

}

#endif