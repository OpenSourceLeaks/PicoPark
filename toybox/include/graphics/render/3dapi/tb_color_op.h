/*!
 * @file
 * @brief TbColorの演算子オーバーロード
 * @author Miyake Shunsuke
 */
#ifndef _INCLUDED_TB_COLOR_OP_H_
#define _INCLUDED_TB_COLOR_OP_H_

namespace toybox
{

/*!
 * カラーの加算
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */
inline TbColorU32 operator+(const TbColorU32& left , const TbColorU32& right )
{
    TbUint8 red = static_cast<TbUint8>(TbMin<TbUint32>( 0xFFU , static_cast<TbUint32>(left.GetRedU8()) + static_cast<TbUint32>(right.GetRedU8()) ));
    TbUint8 green = static_cast<TbUint8>(TbMin<TbUint32>( 0xFFU , static_cast<TbUint32>(left.GetRedU8()) + static_cast<TbUint32>(right.GetRedU8()) ));
    TbUint8 blue = static_cast<TbUint8>(TbMin<TbUint32>( 0xFFU , static_cast<TbUint32>(left.GetRedU8()) + static_cast<TbUint32>(right.GetRedU8()) ));
    TbUint8 alpha = static_cast<TbUint8>(TbMin<TbUint32>( 0xFFU , static_cast<TbUint32>(left.GetRedU8()) + static_cast<TbUint32>(right.GetRedU8()) ));
    TbColorU32 result = TbColorU32::FromRgbaU8( red , green , blue , alpha );
    return result;
}

/*!
 * カラーの引き算
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */
inline TbColorU32 operator-(const TbColorU32& left , const TbColorU32& right )
{
    TbUint8 red   = static_cast<TbUint8>(TbMax<TbSint32>( 0 , static_cast<TbSint32>(left.GetRedU8()) - static_cast<TbSint32>(right.GetRedU8()) ));
    TbUint8 green = static_cast<TbUint8>(TbMax<TbSint32>( 0 , static_cast<TbSint32>(left.GetRedU8()) - static_cast<TbSint32>(right.GetRedU8()) ));
    TbUint8 blue  = static_cast<TbUint8>(TbMax<TbSint32>( 0 , static_cast<TbSint32>(left.GetRedU8()) - static_cast<TbSint32>(right.GetRedU8()) ));
    TbUint8 alpha = static_cast<TbUint8>(TbMax<TbSint32>( 0 , static_cast<TbSint32>(left.GetRedU8()) - static_cast<TbSint32>(right.GetRedU8()) ));
    TbColorU32 result = TbColorU32::FromRgbaU8( red , green , blue , alpha );
    return result;
}

/*!
 * カラーの乗算
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */
inline TbColorU32 operator*(const TbColorU32& left , const TbFloat32 right)
{
    TbUint8 red   = static_cast<TbUint8>(TbClamp<TbFloat32>( static_cast<TbFloat32>(left.GetRedU8()) * right , 0.0f , 255.0f ));
    TbUint8 green = static_cast<TbUint8>(TbClamp<TbFloat32>( static_cast<TbFloat32>(left.GetGreenU8()) * right , 0.0f , 255.0f ));
    TbUint8 blue  = static_cast<TbUint8>(TbClamp<TbFloat32>( static_cast<TbFloat32>(left.GetBlueU8()) * right , 0.0f , 255.0f ));
    TbUint8 alpha = static_cast<TbUint8>(TbClamp<TbFloat32>( static_cast<TbFloat32>(left.GetAlphaU8()) * right , 0.0f , 255.0f ));
    TbColorU32 result = TbColorU32::FromRgbaU8( red , green , blue , alpha );
    return result;
}

/*!
 * カラーの加算
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */
inline TbColorF128 operator+(const TbColorF128& left , const TbColorF128& right )
{
    TbFloat32 red = TbClamp<TbFloat32>( left.r + right.r , 0.0f , 1.0f );
    TbFloat32 green = TbClamp<TbFloat32>( left.g + right.g , 0.0f , 1.0f );
    TbFloat32 blue = TbClamp<TbFloat32>( left.b + right.b , 0.0f , 1.0f );
    TbFloat32 alpha = TbClamp<TbFloat32>( left.a + right.a , 0.0f , 1.0f );
    TbColorF128 result = TbColorF128::FromRgbaF32( red , green , blue , alpha );
    return result;
}

/*!
 * カラーの引き算
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */
inline TbColorF128 operator-(const TbColorF128& left , const TbColorF128& right )
{
    TbFloat32 red = TbClamp<TbFloat32>( left.r - right.r , 0.0f , 1.0f );
    TbFloat32 green = TbClamp<TbFloat32>( left.g - right.g , 0.0f , 1.0f );
    TbFloat32 blue = TbClamp<TbFloat32>( left.b - right.b , 0.0f , 1.0f );
    TbFloat32 alpha = TbClamp<TbFloat32>( left.a - right.a , 0.0f , 1.0f );
    TbColorF128 result = TbColorF128::FromRgbaF32( red , green , blue , alpha );
    return result;
}

/*!
 * カラーの乗算
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */
inline TbColorF128 operator*(const TbColorF128& left , const TbFloat32 right)
{
    TbFloat32 red = TbClamp<TbFloat32>( left.r * right , 0.0f , 1.0f );
    TbFloat32 green = TbClamp<TbFloat32>( left.g * right , 0.0f , 1.0f );
    TbFloat32 blue = TbClamp<TbFloat32>( left.b * right , 0.0f , 1.0f );
    TbFloat32 alpha = TbClamp<TbFloat32>( left.a * right , 0.0f , 1.0f );
    TbColorF128 result = TbColorF128::FromRgbaF32( red , green , blue , alpha );
    return result;
}

}

#endif