/*!
 * カラーフレーム( KinectSDK for Windows )
 * @author teco
 */

#include "tb_fwd.h"
#include "nui/tb_nui_color_frame.h"
#include "nui/tb_nui_util.h"

#if TB_NUIAPI_IS_KINECT

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbNuiColorFrame::TbNuiColorFrame( TbNuiImageResolutionType resolutionType )
    : m_Depend( TB_NUI_IMAGE_TYPE_COLOR , resolutionType )
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
TbNuiColorFrame::~TbNuiColorFrame()
{
}

/*!
 * 準備できているか
 * @author teco
 */
TbBool TbNuiColorFrame::IsReady() const
{
    return m_Depend.IsReady();
}

/*!
 * バッファをロックして取得
 * @author teco
 */
TbResult TbNuiColorFrame::Lock()
{
    if( !m_Buffer ){
        m_Buffer = m_Depend.LockBuffer();
        return m_Buffer ? TB_S_OK : TB_E_FAIL;
    }
    return TB_E_FAIL;
}

/*!
 * バッファのロックを解除
 * @author teco
 */
TbResult TbNuiColorFrame::Unlock()
{
    if( m_Buffer ){
        m_Depend.UnlockBuffer();
        m_Buffer = NULL;
    }
    return TB_E_FAIL;
}

/*!
 * ストリームを開く
 * @author teco
 */
TbResult TbNuiColorFrame::OpenStream()
{
    return m_Depend.OpenStream();
}

/*!
 * ストリームを閉じる
 * @author teco
 */
TbResult TbNuiColorFrame::CloseStream()
{
    return m_Depend.CloseStream();
}

/*!
 * 更新
 * @author teco
 */
void TbNuiColorFrame::Update()
{
    m_Depend.Update();
    m_Pitch = m_Depend.GetPitch();
    m_BytePerPixel = m_Depend.GetBytePerPixel();
}

/*!
 * カラー取得
 * @author teco
 */
TbUint32 TbNuiColorFrame::GetColor( TbUint32 x , TbUint32 y ) const
{
    if( m_Buffer ){
        const TbUint32* buf = reinterpret_cast<const TbUint32*>(m_Buffer);
        if( x < m_Resolution.width && y < m_Resolution.height ) {
            return 0xFF000000 | buf[y*m_Resolution.width+x];
        }
    }
    return 0;
}

}

#endif