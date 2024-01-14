/*! 
 * @file
 * @brief 基本型の定義
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_BASE_TYPES_UTIL_H_
#define _INCLUDED_TB_BASE_TYPES_UTIL_H_

#include "tb_base_types.h"
#include "tb_assert.h"

// TbFloat32のビットパターン取得
inline TbUint32 TbFloat32toUint32( TbFloat32 value )
{
#if 1
   TB_STATIC_ASSERT( sizeof( TbFloat32 ) == sizeof( TbUint32 ) );
   TbUint32 ret;
   memcpy( &ret, &value, sizeof( TbFloat32 ) );
   return ret;
#else // 以下のやり方はaliasing ruleに反する
    return *reinterpret_cast<TbUint32*>(&value);
#endif
}

// NANQかどうか
inline TbBool TbIsNanQ( TbFloat32 value )
{
    TbUint32 u = TbFloat32toUint32(value);
    return (0x7FC00000 <= u && u <= 0x7FFFFFFF) || 
           (0xFFC00000 <= u && u <= 0xFFFFFFF);
}

// NANQかどうか
inline TbBool TbIsNanS( TbFloat32 value )
{
    TbUint32 u = TbFloat32toUint32(value);
    return (0x7F800001 <= u && u <= 0x7FBFFFFF) || 
           (0xFF800001 <= u && u <= 0xFFBFFFFF);
}

// 非数かどうか
inline TbBool TbIsNan( TbFloat32 value ) 
{
    return TbIsNanQ(value) || TbIsNanS(value);
}

// 無限大かどうか
inline TbBool TbIsInfinite( TbFloat32 value )
{
    if( value < TB_FLOAT32_MIN || TB_FLOAT32_MAX <= value ){
        return TB_TRUE;
    }
    return TB_FALSE;
}

#endif
