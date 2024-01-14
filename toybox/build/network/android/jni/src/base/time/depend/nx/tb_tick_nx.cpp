/*!
 * tick
 * @author teco
 */

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

/*!
 * m秒取得
 * @author teco
 */
TbFloat64 TbTick::GetMilliSeconds()
{
    nn::os::Tick    tick = nn::os::GetSystemTick();
    nn::TimeSpan    timeSpan = nn::os::ConvertToTimeSpan(tick);
    int64_t micro = timeSpan.GetMicroSeconds();
    return static_cast<TbFloat64>(micro)*0.001;
}

}
