/*!
 * @file
 * @brief 数学系ヘルパー関数郡
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_math_helper.h"
#include "base/math/tb_math_util.h"

namespace toybox
{
    
/*!
 * 方向と上ベクトルから姿勢行列を設定
 * @param direction 方向(Z軸ベクトルに相当)
 */
void TbCreateMatrixFromDirection(TbMatrix& mtx,const TbVector4& direction,const TbVector4& upVec)
{
    TbVector4 zaxis = direction;
    zaxis.Normalize();
    TbVector4 xaxis = upVec.Cross(zaxis);
    xaxis.Normalize();
    TbVector4 yaxis = zaxis.Cross(xaxis);
    mtx.Set(xaxis.GetX(),xaxis.GetY(),xaxis.GetZ(),0.0f,
        yaxis.GetX(),yaxis.GetY(),yaxis.GetZ(),0.0f,
        zaxis.GetX(),zaxis.GetY(),zaxis.GetZ(),0.0f,
        0.0f,0.0f,0.0f,1.0f);
}

/*!
 * 上ベクトルと横ベクトルから座標系行列を生成
 */
void TbCreateMatrixFromNormal(TbMatrix& mtx,const TbVector4& upVec,const TbVector4& sideVec)
{
    TbVector4 yaxis = upVec;
    yaxis.Normalize();
    TbVector4 zaxis = sideVec.Cross(yaxis);
    zaxis.Normalize();
    TbVector4 xaxis = yaxis.Cross(zaxis);
    mtx.Set(xaxis.GetX(),xaxis.GetY(),xaxis.GetZ(),0.0f,
        yaxis.GetX(),yaxis.GetY(),yaxis.GetZ(),0.0f,
        zaxis.GetX(),zaxis.GetY(),zaxis.GetZ(),0.0f,
        0.0f,0.0f,0.0f,1.0f);
}

template<typename T>
void TbCreateMatrixFromXZAxisT(TbMatrix& mtx,const T& xaxis ,const T& zaxis )
{
    T x = xaxis;
    x.Normalize();
    T z = zaxis;
    z.Normalize();
    T y = z.Cross(x);
    mtx.Set(x.GetX(),x.GetY(),x.GetZ(),0.0f,
            y.GetX(),y.GetY(),y.GetZ(),0.0f,
            z.GetX(),z.GetY(),z.GetZ(),0.0f,
            0.0f,0.0f,0.0f,1.0f);
}

template<typename T>
void TbCreateMatrixFromXZAxisT(TbMatrix& mtx,const T& xaxis ,const T& zaxis , const T& pos )
{
    T x = xaxis;
    x.Normalize();
    T z = zaxis;
    z.Normalize();
    T y = z.Cross(x);
    mtx.Set(x.GetX(),x.GetY(),x.GetZ(),0.0f,
            y.GetX(),y.GetY(),y.GetZ(),0.0f,
            z.GetX(),z.GetY(),z.GetZ(),0.0f,
            pos.GetX(),pos.GetY(),pos.GetZ(),1.0f);
}

// X軸ベクトルとZ軸ベクトルから座標系行列を生成
void TbCreateMatrixFromXZAxis(TbMatrix& mtx,const TbVector4& xaxis ,const TbVector4& zaxis )
{
    TbCreateMatrixFromXZAxisT(mtx,xaxis,zaxis);
}

// X軸ベクトルとZ軸ベクトルから座標系行列を生成
void TbCreateMatrixFromXZAxis(TbMatrix& mtx,const TbVector3& xaxis ,const TbVector3& zaxis , const TbVector3& pos )
{
    TbCreateMatrixFromXZAxisT(mtx,xaxis,zaxis,pos);
}

/*!
 * 2つの2次元ベクトルの成す角(-π～π)の範囲で求める
 */
TbBool TbCalcAngleRad(TbFloat32& rad,const TbVector2& vec1 , const TbVector2& vec2)
{
    TbFloat32 len1xlen2 = vec1.Length() * vec2.Length();
    if( 0.0f == len1xlen2){
        return TB_FALSE;
    }
    TbFloat32 cosAlpha = vec1.Dot( vec2 ) / len1xlen2;
    if( cosAlpha > 1.0  ){
        cosAlpha = 1.0;
    }if( cosAlpha < -1.0 ){
        cosAlpha = -1.0;
    }
    rad = TbArcCos( cosAlpha );

    if(vec1.Cross(vec2) > 0.0f)
    {
        rad = -rad;
    }
    return TB_TRUE;
}

/*!
 * 2つのベクトルの成す角を求める(0～π)
 */
TbBool TbCalcAngleRad(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2)
{
    TbFloat32 len1xlen2 = vec1.Length() * vec2.Length();
    if( 0.0f == len1xlen2){
        return TB_FALSE;
    }
    TbFloat32 cosAlpha = vec1.Dot( vec2 ) / len1xlen2;
    cosAlpha = TbClamp( cosAlpha , -1.0f , 1.0f );
    rad = TbArcCos( cosAlpha );
    return TB_TRUE;
}

/*!
 * 2つのベクトルのXY平面上での成す角を求める(-π～π)
 */
TbBool TbCalcAngleRadXY(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2)
{
    TbVector2 vec1xy(vec1.GetX(),vec1.GetY());
    vec1xy.Normalize();
    TbVector2 vec2xy(vec2.GetX(),vec2.GetY());
    vec2xy.Normalize();
    return TbCalcAngleRad(rad,vec1xy,vec2xy);
}

/*!
 * 2つのベクトルのYZ平面上での成す角を求める(-π～π)
 */
TbBool TbCalcAngleRadYZ(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2)
{
    TbVector2 vec1yz(vec1.GetY(),vec1.GetZ());
    vec1yz.Normalize();
    TbVector2 vec2yz(vec2.GetY(),vec2.GetZ());
    vec2yz.Normalize();
    return TbCalcAngleRad(rad,vec1yz,vec2yz);
}

/*!
 * 2つのベクトルのZX平面上での成す角を求める(-π～π)
 */
TbBool TbCalcAngleRadZX(TbFloat32& rad,const TbVector4& vec1 , const TbVector4& vec2)
{
    TbVector2 vec1zx(vec1.GetZ(),vec1.GetX());
    vec1zx.Normalize();
    TbVector2 vec2zx(vec2.GetZ(),vec2.GetX());
    vec2zx.Normalize();
    return TbCalcAngleRad(rad,vec1zx,vec2zx);
}

/*!
 * 点pから線分x0-11までの距離を求める
 * @param x0 線分の起点
 * @param x1 線分の終点
 * @param p  点
 * @param distance 距離格納先
 * @param normal 線から点への方向ベクトル
 * @author Miyake Shunsuke
 */
void TbGetDistanceFromLineToPoint( const TbVector4& x0,
                                   const TbVector4& x1,
                                   const TbVector4& p,
                                   TbFloat32* distance,
                                   TbVector4* normal,
                                   TbBool isEndless)
{
    //ライン
    TbVector4 line = x1 - x0;
    TbFloat32 lineLength = line.Length();

    TbVector4 lineToPoint = p - x0;

    const float epsilon = 0.0001f;
    // 線分があまりに短い場合は却下
    if( lineLength > epsilon )
    {
        float t = lineToPoint.Dot(line) / lineLength;

        //射影した長さが線分を越えた場合はpからx1の距離
        if( !isEndless && t > lineLength )
        {
            lineToPoint = p - x1;
        }
        //線への距離を発見
        else if(isEndless || t >= 0.0f) 
        {
            TbVector4 normalPoint = line;
            normalPoint /= lineLength;
            normalPoint *= t;
            normalPoint += x0;
            lineToPoint = p - normalPoint;
        }
    }

    TbFloat32 len = lineToPoint.Length();
    if(distance)
    {
        *distance = len;
    }
    if(normal)
    {
        *normal = lineToPoint / len;
    }
}

};
