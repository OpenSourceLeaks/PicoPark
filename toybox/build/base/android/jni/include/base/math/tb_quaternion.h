/*!
 * @file
 * @brief クォータニオンクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_QUATERNION_H_
#define _INCLUDED_TB_QUATERNION_H_

#include <base/math/tb_matrix.h>

namespace toybox
{

/*!
 * クォータニオンクラス
 * @author Miyake Shunsuke
 */
class TbQuaternion
{
public:

    // 球面線形補完
    static TbQuaternion Slerp(const TbQuaternion& left,const TbQuaternion& right, const TbFloat32 interpolater);

public:

    //コンストラクタ
    TbQuaternion(void) {
        Unit();
    }

    //コンストラクタ
    TbQuaternion(TbFloat32 vx,TbFloat32 vy,TbFloat32 vz,TbAngle32 angle) {
        Set( vx , vy , vz , angle );
    }

    //コンストラクタ
    TbQuaternion(const TbVector4& axis,TbAngle32 angle) {
        Set(axis.GetX(),axis.GetY(),axis.GetZ(),angle);
    }

    //デストラクタ
    ~TbQuaternion(void) {}

public: //演算子

    TbQuaternion& operator = (const TbQuaternion& quaternion);

    // 加算
    TbQuaternion  operator + (const TbQuaternion& q) const {
        TbQuaternion result;
        result.x = x+q.x;
        result.y = y+q.y;
        result.z = z+q.z;
        result.w = w+q.w;
        return result;
    }

    // 減算
    TbQuaternion operator-() const {
        TbQuaternion result;
        result.x = -x;
        result.y = -y;
        result.z = -z;
        result.w = -w;
        return result;
    }

    TbQuaternion operator-(const TbQuaternion& q) const {
        TbQuaternion result;
        result.x = x-q.x;
        result.y = y-q.y;
        result.z = z-q.z;
        result.w = w-q.w;
        return result;
    }

    // 乗算
    TbQuaternion& operator *= (const TbQuaternion& quaternion);
    TbQuaternion  operator * (const TbQuaternion& quaternion) const;
    TbQuaternion  operator * (const TbFloat32 val) const {
        TbQuaternion result;
        result.x = x*val;
        result.y = y*val;
        result.z = z*val;
        result.w = w*val;
        return result;
    }

    // 割り算
    TbQuaternion  operator/(const TbFloat32 val) const {
        TbQuaternion result;
        result.x = x/val;
        result.y = y/val;
        result.z = z/val;
        result.w = w/val;
        return result;
    }


public:

    //単位クォータニオン化
    void Unit();

    // 内積
    TbFloat32 Dot( const TbQuaternion& q ) const {
        return  x*q.x + 
                y*q.y + 
                z*q.z + 
                w*q.w;
    }

    //設定
    void Set(TbFloat32 vx,TbFloat32 vy,TbFloat32 vz,TbAngle32 angle);

    //正規化
    TbResult Normalize();

    //行列取得
    TbMatrix GetMatrix() const;

    //行列取得
    void GetMatrix( TbMatrix& dst ) const;

public:

    TbFloat32 x;
    TbFloat32 y;
    TbFloat32 z;
    TbFloat32 w;

};

}

#endif