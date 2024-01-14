/*!
 * 曲線表現
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */

#ifndef _INCLUDED_TB_CURVE_H_
#define _INCLUDED_TB_CURVE_H_

#include "tb_vector2.h"

namespace toybox
{

/*!
 * ベジェ関数
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
TbFloat32 TbParBezFunc(TbFloat32 t, TbFloat32 p1, TbFloat32 p2)
{
    TbFloat32 s = 1-t;
    return (3*s*s*t*p1) + (3*s*t*t*p2) + (t*t*t);
}

/*!
 * ベジェ曲線
 * 0.0 ～ 1.0の空間内での始点の方向ベクトル(p1)と終点のお方向ベクトル(p2)を元に計算
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
TbFloat32 TbBezier( TbFloat32 x , const TbVector2& p1 , const TbVector2& p2 ) 
{
    // TbVector2 p0( 0.0f , 0.0f );
    // TbVector2 p3( 1.0f , 1.0f );

    TbFloat32 dividingT=0.5f;
    TbFloat32 divideResult = 0.0f;

    for(TbUint32 i=0; i<100; ++i) // 最大100回試す
    {
        divideResult=x-TbParBezFunc(dividingT,p1.GetX(),p2.GetX());
        
        // 収束したら終了
        if(TbAbs(divideResult) < 0.0001) {
            break;
        }

        // todo:Newton法に変更
        if( divideResult < 0.0f ){
            dividingT -= 1.0f/static_cast<TbFloat32>(4<<i);
        }else{
            dividingT += 1.0f/static_cast<TbFloat32>(4<<i);
        }
    }

    return TbParBezFunc(dividingT,p1.GetY(),p2.GetY());
}

}

#endif