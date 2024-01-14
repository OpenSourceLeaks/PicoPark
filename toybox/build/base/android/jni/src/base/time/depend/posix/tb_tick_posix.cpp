/*!
 * POSIX用のチッククラス
 * @author Miyake Shunsuke
 * @since 2011.05.17
 */

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include <time.h>

namespace toybox
{

/*!
 * m秒取得
 * @author Miyake Shunsuke
 * @since 2011.05.20
 */
TbFloat64 TbTick::GetMilliSeconds()
{
    timespec time;
    clock_gettime( CLOCK_MONOTONIC , &time );
    TbFloat64 result = (time.tv_sec * 1000.0) + (time.tv_nsec * 0.000001);
    return result;
}

/*!
 * m秒取得
 * @author Miyake Shunsuke
 * @since 2011.05.20
 */
TbFloat64 TbTick::GetMicroSeconds()
{
    timespec time;
    clock_gettime( CLOCK_MONOTONIC , &time );
    TbFloat64 result = (time.tv_sec * 1000000.0) + (time.tv_nsec * 0.001);
    return result;
}

}
