/*!
 * @file
 * @brief 時間クラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TIME_H_
#define _INCLUDED_TB_TIME_H_

namespace toybox
{

// 日付
struct TbDate
{
    TbUint32 year;
    TbUint32 month;
    TbUint32 day;
    TbUint32 hour;
    TbUint32 minute;
    TbUint32 second;
};

/*!
 * 時間クラス
 */
class TbTime
{
public:

    // 日付取得
    static TbDate GetDate();

    // 年取得
    static TbUint32 GetYear();

    // 月取得
    static TbUint32 GetMonth();
    
    // 日取得
    static TbUint32 GetDay();

    // 時間取得
    static TbUint32 GetHour();

    // 分取得
    static TbUint32 GetMinutes();

    // 秒取得
    static TbUint32 GetSeconds();

private:
    TbTime();
    ~TbTime();
};

}

#endif