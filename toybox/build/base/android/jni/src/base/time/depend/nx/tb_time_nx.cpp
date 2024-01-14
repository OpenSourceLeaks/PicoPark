/*!
 * time
 * @author teco
 */

#include "tb_fwd.h"
#include "base/time/tb_time.h"

#include <nn/time/time_TimeZoneApi.h>
#include <nn/time/time_StandardUserSystemClock.h>

namespace toybox
{
    
/*!
 * 日付取得
 * @author teco
 */
TbDate TbTime::GetDate()
{
    nn::time::PosixTime posixTime;
    NN_ABORT_UNLESS_RESULT_SUCCESS(nn::time::StandardUserSystemClock::GetCurrentTime(&posixTime));

    TbDate result;

    nn::time::CalendarTime calendarTime;
    nn::time::CalendarAdditionalInfo calendarAdditionalInfo;
    NN_ABORT_UNLESS_RESULT_SUCCESS(nn::time::ToCalendarTime(&calendarTime, &calendarAdditionalInfo, posixTime));
    result.year = calendarTime.year;
    result.month = calendarTime.month;
    result.day = calendarTime.day;
    result.hour = calendarTime.hour;
    result.minute = calendarTime.minute;
    result.second = calendarTime.second;
    return result;
}

/*!
 * 年取得
 * @author teco
 */
TbUint32 TbTime::GetYear()
{
    return GetDate().year;
}


/*!
 * 月取得
 * @author teco
 */
TbUint32 TbTime::GetMonth()
{
    return GetDate().month;
}

/*!
 * 日取得
 * @author teco
 */
TbUint32 TbTime::GetDay()
{
    return GetDate().day;
}

/*!
 * 時間取得
 * @author teco
 */
TbUint32 TbTime::GetHour()
{
    return GetDate().hour;
}

/*!
 * 分取得
 * @author teco
 */
TbUint32 TbTime::GetMinutes()
{
    return GetDate().minute;
}

/*!
 * 秒取得
 * @author teco
 */
TbUint32 TbTime::GetSeconds()
{
    return GetDate().second;
}

}
