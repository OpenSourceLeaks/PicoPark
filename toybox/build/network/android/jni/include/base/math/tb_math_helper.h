/*!
 * @file
 * @brief 数学系のヘルパー関数郡
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_MATH_HELPER_H_
#define _INCLUDED_TB_MATH_HELPER_H_

#include "base/math/tb_vector4.h"
#include "base/math/tb_vector3.h"
#include "base/math/tb_vector2.h"
#include "base/math/tb_matrix.h"

namespace toybox
{
    //方向と上ベクトルから座標系行列を生成
    void TbCreateMatrixFromDirection(TbMatrix& mtx,const TbVector4& direction,const TbVector4& upVec);

    //上ベクトルと横ベクトルから座標系行列を生成
    void TbCreateMatrixFromNormal(TbMatrix& mtx,const TbVector4& upVec,const TbVector4& sideVec);

    // X軸ベクトルとZ軸ベクトルから座標系行列を生成
    void TbCreateMatrixFromXZAxis(TbMatrix& mtx,const TbVector4& xaxis ,const TbVector4& zaxis );
    void TbCreateMatrixFromXZAxis(TbMatrix& mtx,const TbVector3& xaxis ,const TbVector3& zaxis , const TbVector3& pos );

    //2つのベクトルの成す角を求める(-π～π)
    TbBool TbCalcAngleRad(TbFloat32& rad,const TbVector2& vec1 , const TbVector2& vec2); 

    //2つのベクトルの成す角を求める(0～π)
    TbBool TbCalcAngleRad(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2); 

    //2つのベクトルのZX平面上での成す角を求める(-π～π)
    TbBool TbCalcAngleRadXY(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2); 

    //2つのベクトルのZX平面上での成す角を求める(-π～π)
    TbBool TbCalcAngleRadYZ(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2); 

    //2つのベクトルのZX平面上での成す角を求める(-π～π)
    TbBool TbCalcAngleRadZX(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2); 

    // 点pから線分x0-x1までの距離を求める
    void TbGetDistanceFromLineToPoint( const TbVector4& x0,
                                       const TbVector4& x1,
                                       const TbVector4& p,
                                       TbFloat32* distance,
                                       TbVector4* normal,
                                       TbBool isEndless);

};

#endif
