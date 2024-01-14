/*!
 * Apple用のチッククラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include <sys/time.h>

//clock_gettime is not implemented on OSX
int clock_gettime( struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}

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
    clock_gettime( &time );
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
    clock_gettime( &time );
    TbFloat64 result = (time.tv_sec * 1000000.0) + (time.tv_nsec * 0.001);
    return result;
}

}
