/*!
 * スマートデバイス用ユーティリティ関数群
 * @author teco
 */

#ifndef _INCLUDED_TB_SMART_DEVICE_UTIL_H_
#define _INCLUDED_TB_SMART_DEVICE_UTIL_H_

namespace toybox
{

enum TbSmartDeviceTarget
{
    TB_SMART_DEVICE_IPHONE_35   ,   // 3.5inch版iPhone
    TB_SMART_DEVICE_IPHONE_47   ,   // 4.7inch版iPhone
    TB_SMART_DEVICE_IPHONE_40   ,   // 4inch版iPhone
    TB_SMART_DEVICE_IPHONE_55   ,   // 5.5inch版iPhone
    TB_SMART_DEVICE_IPAD        ,   // iPad
    TB_SMART_DEVICE_IPAD_PRO    ,   // iPadPro
    TB_SMART_DEVICE_REFERENCE   ,   // リファレンスモデル
    TB_SMART_DEVICE_MAX         ,   // 
};

class TbSmartDeviceUtil
{
public:
    
    // 画面縦横比取得(width/height)
    static TbFloat32 GetScreenAspect( TbSmartDeviceTarget target )
    {
        static const TbFloat32 TABLE[] =
        {
            (960.0f/640.0f) , 
            (1334.0f/750.0f) , 
            (1136.0f/640.0f) , 
            (2208.0f/1242.0f) , 
            (1024/748.0f) , 
            (2732.0f/2048.0f) , 
            (400.0f/240.0f) , 
        };
        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(TABLE)==TB_SMART_DEVICE_MAX);
        if( TB_VERIFY(0 <= target && target < TB_ARRAY_COUNT_OF(TABLE)) ){
            return TABLE[target];
        }
        return TABLE[TB_SMART_DEVICE_REFERENCE];
    }
    
private:
    TbSmartDeviceUtil();
    ~TbSmartDeviceUtil();
};

}

#endif
