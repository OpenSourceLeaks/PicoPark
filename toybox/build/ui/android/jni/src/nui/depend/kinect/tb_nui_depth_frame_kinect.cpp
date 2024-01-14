/*!
 * NUI深度フレーム( KinectSDK for Windows )
 * @author teco
 */

#include "tb_fwd.h"
#include "nui/tb_nui_depth_frame.h"
#include "nui/tb_nui_util.h"

#if TB_NUIAPI_IS_KINECT

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbNuiDepthFrame::TbNuiDepthFrame( TbNuiImageResolutionType resolutionType )
    : m_Depend( TB_NUI_IMAGE_TYPE_DEPTH , resolutionType )
    , m_Buffer( NULL )
    , m_BufferSize(0)
    , m_ResolutionType(resolutionType)
    , m_Resolution()
    , m_Pitch(0)
    , m_BytePerPixel(0)
{
    m_Resolution = TbNuiGetResolution(resolutionType);
}

/*!
 * デストラクタ
 * @author teco
 */
TbNuiDepthFrame::~TbNuiDepthFrame()
{
    if( m_Buffer ){
        m_Depend.UnlockBuffer();
    }
}

/*!
 * 準備できているか
 * @author teco
 */
TbBool TbNuiDepthFrame::IsReady() const
{
    return m_Depend.IsReady();
}

/*!
 * ロック
 * @author teco
 */
TbResult TbNuiDepthFrame::Lock()
{
    if( !m_Buffer ){
        m_Buffer = m_Depend.LockBuffer();
        return m_Buffer ? TB_S_OK : TB_E_FAIL;
    }
    return TB_E_FAIL;
}

/*!
 * ロック解除
 * @author teco
 */
TbResult TbNuiDepthFrame::Unlock()
{
    if( m_Buffer ){
        m_Depend.UnlockBuffer();
        m_Buffer = NULL;
    }
    return TB_E_FAIL;
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
    if( 0 <= x && x < m_Depend.GetResolution().width && 0 <= y && y < m_Depend.GetResolution().height )
    {
        TbUint16* buffer = reinterpret_cast<TbUint16*>(m_Buffer);
        result = buffer[ y * m_Depend.GetResolution().width + x ] ;
        if( playerIndex ){
            *playerIndex = static_cast<TbSint16>( result & 7 ) - 1;
        }
        result = result >> 3;
    }
    return result;
}

/*!
 * ストリームを開く
 * @author teco
 */
TbResult TbNuiDepthFrame::OpenStream()
{
    return m_Depend.OpenStream();
}

/*!
 * ストリームを閉じる
 * @author teco
 */
TbResult TbNuiDepthFrame::CloseStream()
{
    return m_Depend.CloseStream();
}

// 
/*!
 * 更新
 * @author teco
 */
void TbNuiDepthFrame::Update()
{
    m_Depend.Update();
    m_Pitch = m_Depend.GetPitch();
    m_BytePerPixel = m_Depend.GetBytePerPixel();
}

}
#endif