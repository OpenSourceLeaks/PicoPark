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

    time_t now;
    time(&now);
    tm* date = localtime(&now);
    result.year = static_cast<TbUint32>(date->tm_year + 1900);
    result.month = static_cast<TbUint32>(date->tm_mon + 1);
    result.day = date->tm_mday;
    result.hour = date->tm_hour;
    result.minute = date->tm_min;
    result.second = date->tm_sec;
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
