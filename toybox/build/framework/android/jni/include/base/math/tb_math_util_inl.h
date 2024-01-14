/*!
 * @file
 * @brief 数学系関数郡
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_MATH_INL_H_
#define _INCLUDED_TB_MATH_INL_H_

namespace toybox
{

/*!
 * 符号関数
 */
template<typename T> T TbSign(const T val)
{
    return (val >= (T)0) ? (T)1 : (T)-1;
}

/*!
 * 符号リテラル文字取得
 */
template<typename T> const char* TbSignStr(const T val)
{
    return (TbSign(val) > (T)0) ? "+" : "-";
}

/*!
 * 絶対値
 */
template<typename T> T TbAbs(const T val)
{
    return (val >= (T)0) ? val : (val * (T)-1);
}

/*!
 * 最大値
 */
template<typename T> T TbMax(const T val1,const T val2)
{
    return (val1 > val2) ? val1 : val2;
}

/*!
 * 最小値
 */
template<typename T> T TbMin(const T val1,const T val2)
{
    return (val1 > val2) ? val2 : val1;
}

/*!
 * 線形補完
 */
template<typename T> T TbLerp(const T val1,const T val2,TbFloat32 t)
{
    return  static_cast<T>((val2-val1) * t) + val1;
}

/*
template<> inline TbSint32 TbLerp<TbSint32>(const TbSint32 val1,const TbSint32 val2,TbFloat32 t)
{
    return  static_cast<TbSint32>(static_cast<TbFloat32>(val2-val1) * t) + val1;
}

template<> TbUint32 TbLerp<TbUint32>(const TbUint32 val1,const TbUint32 val2,TbFloat32 t)
{
    return  static_cast<TbUint32>(static_cast<TbFloat32>(val2-val1) * t) + val1;
}

template<> TbSint16 TbLerp<TbSint16>(const TbSint16 val1,const TbSint16 val2,TbFloat32 t)
{
    return  static_cast<TbSint16>(static_cast<TbFloat32>(val2-val1) * t) + val1;
}

template<> TbUint16 TbLerp<TbUint16>(const TbUint16 val1,const TbUint16 val2,TbFloat32 t)
{
    return  static_cast<TbUint16>(static_cast<TbFloat32>(val2-val1) * t) + val1;
}

template<> TbSint8 TbLerp<TbSint8>(const TbSint8 val1,const TbSint8 val2,TbFloat32 t)
{
    return  static_cast<TbSint8>(static_cast<TbFloat32>(val2-val1) * t) + val1;
}

template<> TbUint8 TbLerp<TbUint8>(const TbUint8 val1,const TbUint8 val2,TbFloat32 t)
{
    return  static_cast<TbUint8>(static_cast<TbFloat32>(val2-val1) * t) + val1;
}
*/
/*!
 * クランピング
 */
template<typename T> T TbClamp(const T val,const T min,const T max)
{
    return val < min ? min : (val > max ? max : val);
}

/*!
 * クリッピング
 */
template<typename T> T TbClip(const T val,const T min,const T max)
{
    return (val < min || max < val) ? static_cast<T>(0) : val;
}

/*!
 * クリッピング
 */
template<typename T> T TbClip(const T val,const T min,const T max,const T outValue)
{
    return (val < min || max < val) ? static_cast<T>(outValue) : val;
}

/*!
 * 逆クリッピング
 */
template<typename T> T TbReverseClip(const T val,const T min,const T max)
{
    return (min <= val && val <= max) ? static_cast<T>(0) : val;
}

/*!
 * 逆クリッピング(クリップ時の返り値)
 */
template<typename T> T TbReverseClip(const T val,const T min,const T max,const T clipValue)
{
    return (min <= val && val <= max) ? static_cast<T>(clipValue) : val;
}

/*!
 * 範囲チェック
 */
template<typename T1,typename T2,typename T3> TbBool TbCheckRange(const T1& val,const T2& min,const T3& max)
{
    return (min <= val) && (val <= max);
}

/*!
 * 平方根
 */
inline TbFloat32 TbSqrt(TbFloat32 val)
{
    return sqrtf(val);
}

/*!
 * 累乗
 */
inline TbFloat32 TbPow(const TbFloat32& x,const TbSint32& y)
{
    return static_cast<TbFloat32>(pow(x,y));
}

/*!
 * 累乗
 */
inline TbFloat32 TbPow(const TbFloat32& x,const TbFloat32& y)
{
    return static_cast<TbFloat32>(pow(x,y));
}

/*!
 * 四捨五入
 */
inline TbFloat32 TbRound(TbFloat32 value)
{
    return roundf(value);
}

/*!
 * 切上げ
 */
inline TbFloat32 TbRoundUp(TbFloat32 value)
{
    TbFloat32	result = 0.0f;
	if(value < 0.f) {
		result = -TbCeil(TbAbs(value));
	}else {
		result = TbCeil(TbAbs(value));
	}
	return result;
}

/*!
 * 少数切捨て
 */
inline TbFloat32 TbRoundDown(TbFloat32 value)
{
    TbFloat32	result = 0.0f;
	if(value < 0.f) {
		result = -TbFloor(TbAbs(value));
	}else {
		result = TbFloor(TbAbs(value));
	}
	return result;
}

/*!
 * 少数切捨て
 * @param digit 下桁数
 */
inline TbFloat32 TbRoundDown(TbFloat32 value, TbFloat32 digit) {
    TbFloat32 coef = TbPow(10.0f, digit);
    return value > 0.0f ? TbFloor(value * coef) / coef :
                          TbCeil(value * coef) / coef;
}

/*!
 * 少数の余剰取得
 */
inline TbFloat32 TbMod(const TbFloat32& x,const TbFloat32& y)
{
    return static_cast<TbFloat32>(fmod(x,y));
}

/*!
 * floatの整数桁数取得
 */
inline TbUint32 TbCountDigitFloat(const TbFloat32& value)
{
    TbSint32 digitVal = static_cast<TbSint32>(TbRoundDown(value));
    TbUint32 result = 0;
    do{
        ++result;
        digitVal /= 10;
    }while(digitVal != 0);
    return result;
}

/*!
 * 符号無し整数の桁数取得
 */
inline TbUint32 TbCountDigitInt(const TbSint32& value)
{
    TbSint32 digitVal = value;
    TbUint32 result = 0;
    do{
        ++result;
        digitVal /= 10;
    }while(digitVal != 0);
    return result;
}

/*!
 * floatの少数桁数取得
 */
inline TbSint32 TbCountDicimal(const TbFloat32& value)
{
    TbFloat32 decVal = TbMod(value,1.0f);
    TbSint32 result = 0;
    do{
        ++result;
        decVal *= 10;
    }while(TbMod(decVal,1.0f) != 0.0f);
    return result;
}

/*!
 * 数値の少数部を取得
 * @author Miyake Shunsuke
 * @since 2010.06.06
 */
inline TbFloat32 TbFraction( TbFloat32 value )
{
    TbFloat64 exp;
    return static_cast<TbFloat32>(modf(value,&exp));
}

// 一番近い2の乗数に繰り上げ
inline TbUint32 TbAlignPow2(TbUint32 a)
{
    // 既に冪数であるかのチェック。有名なアルゴリズムだそうで
    if( a & ( a - 1 ) )
    {
        // uint32のビット数(4byte * 8bit)だけシフトを繰り返す。ついでにシフトもする
        for( int count = 0 ; count < sizeof( int ) * TB_CHAR_BIT_COUNT ; count++ , a >>= 1 ){
            // 終端ビットの桁数 + 1で算出。
            if( a == 1 ) {
                return a << (count + 1);
            }
        }
    }
    // 安全第一。
    return a;
}

}

#endif