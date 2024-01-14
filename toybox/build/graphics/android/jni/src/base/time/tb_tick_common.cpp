/*!
 *
 * @author Miyake Shunsuke
 * @since 2011.05.17
 */

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

TbFloat64     TbTick::s_PreFrameMilliSecond = 0.0;
TbFloat64     TbTick::s_FrameSpeedMilliSecond = 0.0;
TbFloat64     TbTick::s_FrameMilliSecondPerCount = 16.6;
TbUint32      TbTick::s_FrameCount = 0;
TbUint32      TbTick::s_DebugFrameSpeedCountMax = 0;
TbUint32      TbTick::s_DebugFrameSpeedCount = 1;
TbBool        TbTick::s_IsActiveDebugFrameSpeedCount = TB_FALSE;

/*!
 * 毎フレームにかかった時間を計測します。
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */
void TbTick::Update()
{
    if( s_PreFrameMilliSecond < TB_FLOAT32_EPSILON ){
        s_PreFrameMilliSecond = GetMilliSeconds();
    }
    TbFloat64 msec = GetMilliSeconds();
    s_FrameSpeedMilliSecond = msec - s_PreFrameMilliSecond;
    s_PreFrameMilliSecond = msec;
    if( s_FrameMilliSecondPerCount > TB_FLOAT32_EPSILON ){
        TbFloat32 count = static_cast<TbFloat32>(s_FrameSpeedMilliSecond / s_FrameMilliSecondPerCount);
        TbFloat32 frac = TbFraction(count);
        s_FrameCount = static_cast<TbUint32>(( count - frac ) + ( ( frac >= 0.5f ) ? 1.0f : 0.0f ));
        // デバッグ
#if TB_IS_DEBUG
        if( s_IsActiveDebugFrameSpeedCount ){
            s_FrameCount = s_DebugFrameSpeedCount;
        }else if( s_DebugFrameSpeedCountMax  > 0 ){
            s_FrameCount = TbMin( s_DebugFrameSpeedCountMax , s_FrameCount );
        }
#endif
    }
}

}
