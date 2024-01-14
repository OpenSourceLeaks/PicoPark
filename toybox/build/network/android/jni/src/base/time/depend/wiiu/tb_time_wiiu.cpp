/*!
 *
 * @author Miyake Shunsuke
 * @since 2011.05.17
 */

#include "tb_fwd.h"
#include "base/time/tb_time.h"

#include <time.h>

namespace toybox
{
    
/*!
 * 日付取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbDate TbTime::GetDate()
{
    TbDate result;
    OSTick tick = OSGetTick();

    OSCalendarTime calender;
    OSTicksToCalendarTime(tick,&calender);

    result.year = calender.year;
    result.month = calender.mon;
    result.day = calender.mday;
    result.hour = calender.hour;
    result.minute = calender.min;
    result.second = calender.sec;
    
    return result;
}

/*!
 * 年取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbUint32 TbTime::GetYear()
{
    return GetDate().year;
}


/*!
 * 月取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbUint32 TbTime::GetMonth()
{
    return GetDate().month;
}

/*!
 * 日取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbUint32 TbTime::GetDay()
{
    return GetDate().day;
}

/*!
 * 時間取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbUint32 TbTime::GetHour()
{
    return GetDate().hour;
}

/*!
 * 分取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbUint32 TbTime::GetMinutes()
{
    return GetDate().minute;
}

/*!
 * 秒取得
 * @author Miyake Shunsuke
 * @since 2012.08.08
 */
TbUint32 TbTime::GetSeconds()
{
    return GetDate().second;
}

}
