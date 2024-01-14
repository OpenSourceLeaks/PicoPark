/*!
 * 3DAPIユーティリティ関数
 * @author Miyake Shunsuke
 * @since 2011.07.03
 */

#ifndef _INCLUDED_TB_3DAPI_UTIL_H_
#define _INCLUDED_TB_3DAPI_UTIL_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{
    
/*!
 * 指定個数のプリミティブを描画するために必要な頂点数を取得
 * @author teco
 */
inline TbSizeT TbGetPrimitiveVertexCount(TbPrimitiveType type,TbUint32 primitiveCount)
{
    static const TbUint32 PRIMCOUNT_SCALE[] =
    {
        1 , // TB_PT_POINTLIST 
        2 , // TB_PT_LINELIST
        1 , // TB_PT_LINESTRIP
        3 , // TB_PT_TRIANGLELIST
        1 , // TB_PT_TRIANGLESTRIP
        1 , // TB_PT_TRIANGLEFAN
    };
    static const TbUint32 PRIMCOUNT_OFFSET[] =
    {
        0 , // TB_PT_POINTLIST 
        0 , // TB_PT_LINELIST
        1 , // TB_PT_LINESTRIP
        0 , // TB_PT_TRIANGLELIST
        2 , // TB_PT_TRIANGLESTRIP
        2 , // TB_PT_TRIANGLEFAN
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(PRIMCOUNT_SCALE) == TB_PT_MAX );
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(PRIMCOUNT_OFFSET) == TB_PT_MAX );
    TB_ASSERT(type<TB_PT_MAX);
    // 範囲外チェックはとりあえずしない
    return (primitiveCount * PRIMCOUNT_SCALE[type]) + PRIMCOUNT_OFFSET[type];
}

}

#endif
