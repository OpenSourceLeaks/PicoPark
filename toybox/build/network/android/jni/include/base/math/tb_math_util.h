/*!
 * @file
 * @brief 数学系関数郡
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_MATH_H_
#define _INCLUDED_TB_MATH_H_

namespace toybox
{

#define TB_PI 3.14159265f
#define TB_EPSILON 1.192092896e-07F
#define TB_E  2.71828f

//宣言

// 符号値取得
template<typename T> T TbSign(const T val);

// 符号値文字列取得
template<typename T> const char* TbSignStr(const T val);

// 絶対値
template<typename T> T TbAbs(const T val);

// 最大値
template<typename T> T TbMax(const T val1, const T val2);

// 最小値
template<typename T> T TbMin(const T val1, const T val2);

// 補完値
template<typename T> T TbLerp(const T val1, const T val2, TbFloat32 t);

// クランピング
template<typename T> T TbClamp(const T val, const T min, const T max);

// 逆クランピング
template<typename T> T TbReverseClamp(const T val, const T min, const T max);

// クリッピング
template<typename T> T TbClip(const T val, const T min, const T max);

// クリッピング( クリップ外の時の値設定可能 )
template<typename T> T TbClip(const T val, const T min, const T max, const T outValue);

// 逆クリッピング
template<typename T> T TbReverseClip(const T val, const T min, const T max);

// 逆クリッピング( クリップ外の時の値設定可能 )
template<typename T> T TbReverseClip(const T val, const T min, const T max, const T clipValue);

// 範囲内チェック
template<typename T1, typename T2, typename T3> TbBool TbCheckRange(const T1& val, const T2& min, const T3& max);

// 平方根
TbFloat32 TbSqrt(TbFloat32 val);

// 割算の余り
TbFloat32 TbMod(const TbFloat32& x, const TbFloat32& y);

// 乗算
TbFloat32 TbPow(const TbFloat32& x, const TbSint32& y);

// 乗算
TbFloat32 TbPow(const TbFloat32& x, const TbFloat32& y);

// 二乗
inline TbFloat32 TbSquare(TbFloat32 value)
{
    return value * value;
}

// 四捨五入
TbFloat32 TbRound( TbFloat32 value);

//切上げ
TbFloat32 TbRoundUp(TbFloat32 value);

//切捨て
TbFloat32 TbRoundDown(TbFloat32 value, TbFloat32 digit);

//切捨て
TbFloat32 TbRoundDown(TbFloat32 value);

// float値の整数部値桁数取得
TbUint32 TbCountDigitFloat(const TbFloat32& value);

// float値の小数部桁数取得
TbSint32 TbCountDicimal(const TbFloat32& value);

// int値の整数部桁数取得
TbUint32 TbCountDigitInt(const TbSint32& value);

//TbFloat32 TbSizeRangeScale(TbFloat32 value , TbFloat32 fromSrc ,TbFloat32 toSrc ,TbFloat32 fromDst , TbFloat32 toDst);

// 小数部取得
TbFloat32 TbFraction(TbFloat32 value);

// 一番近い2の乗数に繰り上げ
TbUint32 TbAlignPow2(TbUint32 a);

// sin
inline TbFloat32 TbSin(TbFloat32 value)
{
    return static_cast<TbFloat32>(sin(value));
}

// sinh
inline TbFloat32 TbSinh(TbFloat32 value)
{
    return static_cast<TbFloat32>(sinh(value));
}

// arcsin
inline TbFloat32 TbArcSin(TbFloat32 value)
{
    return static_cast<TbFloat32>(asin(value));
}

// cos
inline TbFloat32 TbCos(TbFloat32 value)
{
    return static_cast<TbFloat32>(cos(value));
}

// cosh
inline TbFloat32 TbCosh(TbFloat32 value)
{
    return static_cast<TbFloat32>(cosh(value));
}

// arccos
inline TbFloat32 TbArcCos(TbFloat32 value)
{
    return static_cast<TbFloat32>(acos(value));
}

// tan
inline TbFloat32 TbTan(TbFloat32 value)
{
    return static_cast<TbFloat32>(tan(value));
}

// tanh
inline TbFloat32 TbTanh(TbFloat32 value)
{
    return static_cast<TbFloat32>(tanh(value));
}

// arctan
inline TbFloat32 TbArcTan(TbFloat32 value)
{
    return static_cast<TbFloat32>(atan(value));
}

// arctan2
inline TbFloat32 TbArcTan2(TbFloat32 y, TbFloat32 x)
{
    return static_cast<TbFloat32>(atan2(y,x));
}

// 指数関数
inline TbFloat32 TbExp(TbFloat32 value)
{
    return static_cast<TbFloat32>(exp(value));
}

// x以下の最大の整数
inline TbFloat32 TbFloor(TbFloat32 value)
{
    return static_cast<TbFloat32>(floor(value));
}

// x以上の最小の整数
inline TbFloat32 TbCeil(TbFloat32 value)
{
    return static_cast<TbFloat32>(ceil(value));
}

}

// 実装
#include "./tb_math_util_inl.h"

#endif