/*!
 * WiiU用のチッククラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include <time.h>

namespace toybox
{

/*!
 * m秒取得
 * @author Miyake Shunsuke
 */
TbFloat64 TbTick::GetMilliSeconds()
{
    OSTick tick = OSGetTick();
    TbFloat64 result = static_cast<TbFloat64>(OSTicksToMicroseconds(tick))*0.001f;
    return result;
}

/*!
 * m秒取得
 * @author Miyake Shunsuke
 */
TbFloat64 TbTick::GetMicroSeconds()
{
    OSTick tick = OSGetTick();
    TbFloat64 result = static_cast<TbFloat64>(OSTicksToMicroseconds(tick));
    return result;
}

}
