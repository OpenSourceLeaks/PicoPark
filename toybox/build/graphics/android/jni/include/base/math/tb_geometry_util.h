/*!
 * @file
 * @brief ジオメトリ系ユーティリティ関数
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_GEOMETRY_UTIL_H_
#define _INCLUDED_TB_GEOMETRY_UTIL_H_

#include <base/math/tb_geometry.h>

namespace toybox
{
    
    // 線の整数座標配列サイズを取得
    TbUint32 TbGetLinePointCount( const TbPointS32& start , 
                                  const TbPointS32& end );

    // 線を整数座標配列で取得
    TbUint32 TbGetLinePointArray( TbPointS32* pointArray , 
                                  TbUint32 pointCount , 
                                  const TbPointS32& start , 
                                  const TbPointS32& end );

    // 半円の整数座標配列で取得
    TbUint32 TbGetEllipsePointCount( TbSint32 width , 
                                     TbSint32 height , 
                                     const TbPointS32& center );
    
    // 半円を整数座標配列で取得
    TbUint32 TbGetEllipsePointArray( TbPointS32* pointArray , 
                                     TbUint32 pointCount ,
                                     const TbPointS32& center,
                                     TbSint32 width , 
                                     TbSint32 height
                                   );
    
    // 半円を整数座標配列で取得(角度制限、半径制限付き)
    TbUint32 TbGetEllipsePointArray( TbPointS32* pointArray , 
                                     TbUint32 pointCount ,
                                     const TbPointS32& center ,
                                     TbSint32 width , 
                                     TbSint32 height , 
                                     TbAngle32 startAng , 
                                     TbAngle32 endAng , 
                                     TbSint32  widthLimit , 
                                     TbSint32  heightLimit
                                    );

    // 交差判定
    TbBool TbIsGeometryContacted( const TbRectF32& c1 , const TbRectF32& c2 );

    // 交差判定
    TbBool TbIsGeometryContacted( const TbCircleF32& c1 , const TbCircleF32& c2 );

    // 交差判定
    TbBool TbIsGeometryContacted( const TbRectF32& c1 , const TbCircleF32& c2 );

};

#endif
