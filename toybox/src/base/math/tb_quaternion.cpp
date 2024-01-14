/*!
 * クォータニオンクラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_quaternion.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

/*!
 * 球面線形補間
 * @author Miyake_Shunsuke
 * @since 2014.04.02
 */
TbQuaternion TbQuaternion::Slerp(const TbQuaternion& left,const TbQuaternion& right, const TbFloat32 t)
{
    TB_ASSERT(0.0f<=t&&t<=1.0f);
    if( TB_FLOAT32_EQUAL(t,0.0f) ){
        return left;
    }
    if( TB_FLOAT32_EQUAL(t,1.0f) ){
        return right;
    }
    TbQuaternion result = right;
    TbFloat32 qr = left.Dot(right);
    if( qr < 0.0f ) {
        result = -result;
        qr = -qr;
    }
    if( qr > 1.0f-TB_FLOAT32_EPSILON){
        result = TbLerp(left,right,t);
        result.Normalize();
        return result;
    }
    TbFloat32 ph = TbArcCos(qr);
    TbFloat32 sp = TbSin(ph);
    TbFloat32 t0 = TbSin((1.0f-t)*ph);
    TbFloat32 t1 = TbSin(t*ph);
    result = (left*t0 + result*t1) / sp;
    return result;
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
TbQuaternion& TbQuaternion::operator = (const TbQuaternion& quaternion)
{
    x = quaternion.x;
    y = quaternion.y;
    z = quaternion.z;
    w = quaternion.w;
    return *this;
}

/*!
 * 乗算代入演算子
 * @author Miyake Shunsuke
 */
TbQuaternion& TbQuaternion::operator *= (const TbQuaternion& q)
{
    TbVector4 v1( x , y , z );
    TbVector4 v2( q.x , q.y , q.z );
    TbVector4 v;
    w = w*q.w - v1.Dot( v2 );
    v = v2*w + v1*q.w + v1.Cross( v2 );
    x = v.GetX();
    y = v.GetY();
    z = v.GetZ();
    return *this;
}

/*!
 * 乗算代入演算子
 * @author Miyake Shunsuke
 */
TbQuaternion TbQuaternion::operator * (const TbQuaternion& quaternion) const
{
    TbQuaternion result = (*this);
    result *= quaternion;
    return result;
}

/*!
 * 単位クォータニオン化
 * @author Miyake Shunsuke
 */
void TbQuaternion::Unit()
{
    x = y = z = 0.0f;
    w = 0.0f;
}

/*!
 * 設定
 * @author Miyake Shunsuke
 */
void TbQuaternion::Set(TbFloat32 vx,TbFloat32 vy,TbFloat32 vz,TbAngle32 angle)
{
    TbFloat32 c = TbCos( TbAng32ToRad(angle)/2.0f );
    TbFloat32 s = TbSin( TbAng32ToRad(angle)/2.0f );
    w = c;
    x = vx*s;
    y = vy*s;
    z = vz*s;
}

/*!
 * 正規化
 * @author Miyake Shunsuke
 */
TbResult TbQuaternion::Normalize()
{
    TbFloat32 len = TbSqrt(x*x + y*y + z*z + w*w);
    if( len > TB_EPSILON )
    {
        x /= len;
        y /= len;
        z /= len;
        w /= len;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 行列取得
 * @return 行列
 * @author Miyake Shunsuke
 * @since 2011.03.16
 */
TbMatrix TbQuaternion::GetMatrix() const
{
    TbMatrix result;
    GetMatrix(result);
    return result;
}

/*!
 * 行列取得
 * @return 行列
 * @author Miyake Shunsuke
 * @since 2011.03.16
 */
void TbQuaternion::GetMatrix( TbMatrix& dst ) const
{
#if 1
    TbVector4 xaxis( 1.0f-2*y*y-2*z*z , 2*x*y+2*w*z , 2*x*z-2*w*y );
    TbVector4 yaxis( 2*x*y-2*w*z , 1-2*x*x-2*z*z , 2*y*z+2*w*x );
    TbVector4 zaxis( 2*x*z+2*w*y , 2*y*z-2*w*x , 1-2*x*x-2*y*y );
    xaxis.Normalize();
    yaxis.Normalize();
    zaxis.Normalize();
    dst.Set( xaxis , yaxis , zaxis );
#else
    dst.Set(  1.0f-2*y*y-2*z*z , 2*x*y+2*w*z , 2*x*z-2*w*y , 0.0f , 
              2*x*y-2*w*z , 1-2*x*x-2*z*z , 2*y*z+2*w*x , 0.0f , 
              2*x*z+2*w*y , 2*y*z-2*w*x , 1-2*x*x-2*y*y , 0.0f , 
              0.0f , 0.0f , 0.0f , 1.0f );
#endif
}

}
