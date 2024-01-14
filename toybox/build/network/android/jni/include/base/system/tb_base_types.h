/*! 
 * @file
 * @brief 基本型の定義
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_BASE_TYPES_H_
#define _INCLUDED_TB_BASE_TYPES_H_

#include <base/system/tb_target_types.h>
//#include <limits>       // numeric_limits

//8bit整数値
typedef signed char TbSint8;
typedef unsigned char TbUint8;
#define TB_SINT8_BIT_COUNT 8
#define TB_UINT8_BIT_COUNT 8
#define TB_SINT8_MIN (-128)
#define TB_SINT8_MAX (127)
#define TB_UINT8_MIN (0)
#define TB_UINT8_MAX (0xFF)

//16bit整数値
typedef signed short TbSint16;
typedef unsigned short TbUint16;
#define TB_SINT16_BIT_COUNT 16
#define TB_UINT16_BIT_COUNT 16
#define TB_SINT16_MIN (-32768)
#define TB_SINT16_MAX (32767)
#define TB_UINT16_MIN (0)
#define TB_UINT16_MAX (0xFFFF)

//32bit整数値
#if TB_PlATFORM_IS_IOS || TB_IS_LP64
typedef signed int TbSint32;
typedef unsigned int TbUint32;
#else
typedef signed long TbSint32;
typedef unsigned long TbUint32;
#endif
#define TB_SINT32_BIT_COUNT 32
#define TB_UINT32_BIT_COUNT 32
#define TB_SINT32_MIN (-2147483647 - 1)
#define TB_SINT32_MAX (2147483647)
#define TB_UINT32_MIN (0)
#define TB_UINT32_MAX (0xFFFFFFFF)

//64bit整数値
typedef signed long long TbSint64;
typedef unsigned long long TbUint64;
#define TB_SINT64_BIT_COUNT 64
#define TB_UINT64_BIT_COUNT 64
#define TB_SINT64_MIN (-9223372036854775807i64 - 1)
#define TB_SINT64_MAX (9223372036854775807i64)
#define TB_UINT64_MIN (0)
#define TB_UINT64_MAX (0xFFFFFFFFFFFFFFFFui64)

//size_t
typedef size_t TbSizeT;

//文字
typedef char     TbChar8;
#define TB_CHAR_BIT_COUNT (8)
#if (TB_PLATFORM_IS_WIN) || (TB_PLATFORM_IS_FLASH) || (TB_PLATFORM_IS_WIIU)
#   define  TB_CHAR16_IS_WCHAR (1)
    typedef wchar_t TbChar16; // wchar_tが2バイトとは限らないので
#else
// emscriptenとiosのwchar_tは4バイトだった。。。
#   define  TB_CHAR16_IS_WCHAR (0)
    typedef TbUint16 TbChar16; // wchar_tが2バイトとは限らないので
#endif

// 文字テーブル(文字にenum値でアクセスできるようにするためのもの)
enum TbCharTableType
{
    TB_CHAR_TABLE_PERIOD , // '.'
    TB_CHAR_TABLE_YEN    , // '\\'
    TB_CHAR_TABLE_SLASH , // '/'
    TB_CHAR_TABLE_MAX
};

// 第二引数は
template <typename T>
T TbGetChar( TbCharTableType type );

template <>
inline TbChar8 TbGetChar<TbChar8>( TbCharTableType type )
{
    TbChar8 TABLE[] = 
    {
        '.',
        '\\',
        '/'
    };
    return TABLE[type];
}

template <>
inline TbChar16 TbGetChar<TbChar16>( TbCharTableType type )
{
    TbChar16 TABLE[] = 
    {
        L'.',
        L'\\',
        L'/'
    };
    return TABLE[type];
}

//論理値
typedef int TbBool;
#define TB_TRUE 1
#define TB_FALSE 0
#define TB_BOOL_NOT_EQUAL( a , b ) ((a&&!b)||(!a&&b))
#define TB_BOOL_STRING(b) ((b) ? "true" : "false")

//浮動小数
typedef float TbFloat32;
#define TB_FLOAT32_BIT_COUNT 32
#define TB_FLOAT32_MIN (1.175494351e-38F)
#define TB_FLOAT32_MAX (3.402823466e+38F)
#define TB_FLOAT32_EPSILON (1.192092896e-07F)
#define TB_FLOAT32_EQUAL( a , b ) (toybox::TbAbs<TbFloat32>((a)-(b))<=TB_FLOAT32_EPSILON)
#define TB_FLOAT32_EQUAL_EPS( a , b , eps ) (toybox::TbAbs((a)-(b))<=eps)
#define TB_FLOAT32_EQUAL_ZERO( a ) (TbAbs((a))<=TB_FLOAT32_EPSILON)

typedef double TbFloat64;
#define TB_FLOAT64_BIT_COUNT 64
#define TB_FLOAT64_MIN (2.2250738585072014e-308)
#define TB_FLOAT64_MAX (1.7976931348623158e+308)
#define TB_FLOAT64_EPSILON (2.2204460492503131e-016)
#define TB_FLOAT64_EQUAL( a , b ) (toybox::TbAbs((a)-(b))<=TB_FLOAT64_EPSILON)
#define TB_FLOAT64_EQUAL_ZERO( a ) (toybox::TbAbs((a))<=TB_FLOAT64_EPSILON)

//角度
typedef TbFloat32 TbRadian;    //ラジアン値
typedef TbSint16 TbAngle16; //0x8000が180度
typedef TbSint32 TbAngle32; //0x100000が360度
#define TB_PI_RAD 3.14159265f
#define TB_PI_ANG32 0x8000

// 時間
#define TB_MINITUE_SEC (60.0f)
#define TB_HOUR_SEC    (TB_MINITUE_SEC*(60.0f))


// Angle32 -> Radian
inline TbRadian TbAng32ToRad( TbAngle32 ang ){
    return static_cast<TbRadian>( static_cast<TbFloat32>(ang) * 0.000095873799f );
//    return static_cast<TbRad>( ( static_cast<TbFloat32>(ang) / static_cast<TbFloat32>(TB_PI_ANG32) ) * TB_PI_RAD );
}
// Radian -> Angle32
inline TbAngle32 TbRadToAng32( TbRadian rad ){
    return static_cast<TbAngle32>( rad * 10430.378f ); 
//    return static_cast<TbAngle32>( ( static_cast<TbFloat32>(rad) / TB_PI_RAD ) * static_cast<TbFloat32>(TB_PI_ANG32) );
}
// Degree -> Radian
inline TbRadian TbDegToRad( TbFloat32 deg ){
    return deg * 0.017453293f;
//    return static_cast<TbRad>( ( deg / 180.0f ) * TB_PI_RAD );
}
// Degree -> Angle32
inline TbAngle32 TbDegToAng32( TbFloat32 deg ){
//    return static_cast<TbAngle32>( deg * 182.04444f ); 
    return static_cast<TbAngle32>( ( deg / 180.0f ) * static_cast<TbFloat32>(TB_PI_ANG32) );
}
// Radian -> Degree
inline TbFloat32 TbRadToDeg( TbRadian rad ){
    return rad * 57.295780f;
//    return static_cast
// 定義済みマクロ<TbFloat32>( ( rad / TB_PI_RAD ) * TB_PI_RAD );
}
// Angle32 -> Degree
inline TbFloat32 TbAng32ToDeg( TbAngle32 ang ){
    return static_cast<TbRadian>( static_cast<TbFloat32>(ang) * 0.0054931641f );
//    return static_cast<TbFloat32>( ( ang / static_cast<TbFloat32>(TB_PI_ANG32) ) * TB_PI_RAD );
}

//アドレス
#if TB_IS_64BIT
typedef TbUint64 TbAddr;
#else
typedef TbUint32 TbAddr;
#endif

struct TbGuid
{
    static const size_t SIZE = 32;
    TbGuid() {
        for( size_t i = 0; i < SIZE; ++i ) {
            buf[i] = 0U;
        }
    }
    TbUint8 buf[SIZE];
};

// デバッグビルドのみ非const
#if TB_IS_DEBUG
    #define TB_NDEBUG_CONST
#else
    #define TB_NDEBUG_CONST const
#endif

// レガシー環境でのnullptr実装
#if !TB_HAS_NULLPTR

const class Nullptr_t {
public:
    Nullptr_t() {}
public:
    template <class T>    // あらゆる型のポインタに
    operator T*() const {   // 変換できる
        return 0;         // ゼロを返す
    }
 
    template <class C, class T>  // あらゆる型のメンバのポインタに
    operator T C::*() const {    // 変換できる
        return 0;                // こちらもゼロ
    }
 
private:
    void operator&()  const; // アドレスは取得できない
} nullptr;

#endif

#endif
