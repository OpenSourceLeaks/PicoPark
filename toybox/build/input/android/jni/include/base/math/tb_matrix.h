/*!
 * @file
 * @brief 行列クラスのヘッダーファイルq
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_MATRIX_H_
#define _INCLUDED_TB_MATRIX_H_

#include <base/math/tb_vector3.h>
#include <base/math/tb_vector4.h>

namespace toybox
{

/*!
 * 行列クラス
 * ベクトルを列ベクトルと見る
 */
class TbMatrix44
{
public:

    friend class TbMatrix43;

    //単位行列
    const static TbMatrix44 IDENTITY;

public:

    //コンストラクタ
    TbMatrix44(void);
    TbMatrix44(const TbMatrix44& mtx);
    TbMatrix44(const TbFloat32 vx,const TbFloat32 vy,const TbFloat32 vz,const TbAngle32 angle);
    TbMatrix44(const TbVector4& axisX,const TbVector4& axisY,const TbVector4& axisZ) {
        Set(axisX,axisY,axisZ);
    }
    TbMatrix44(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,TbFloat32 m03,
             TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,TbFloat32 m13,
             TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,TbFloat32 m23,
             TbFloat32 m30,TbFloat32 m31,TbFloat32 m32,TbFloat32 m33);
    //コンストラクタ
    ~TbMatrix44(void);

public:// 演算子

    TbMatrix44& operator=(const TbMatrix44& mtx);
    TbMatrix44& operator+=(const TbMatrix44& mtx);
    TbMatrix44& operator-=(const TbMatrix44& mtx);
    TbMatrix44& operator*=(const TbMatrix44& mtx);
    TbMatrix44  operator*(const TbMatrix44& mtx) const ;
    TbMatrix44  operator+(const TbMatrix44& mtx) const ;
    TbMatrix44  operator-(const TbMatrix44& mtx) const ;
    TbVector4    operator*(const TbVector4& vector ) const;
    TbVector3    operator*(const TbVector3& vector ) const;
    TbBool      operator==(const TbMatrix44& mtx);
    TbBool      operator!=(const TbMatrix44& mtx);

public:

    //単位行列
    void Unit();

    //要素取得
    TbFloat32 Get(TbUint32 i,TbUint32 j) const;

    //設定
    void Set(TbUint32 i , TbUint32 j , const TbFloat32 value );

    //設定
    void Set(const TbFloat32 vx,const TbFloat32 vy,const TbFloat32 vz,const TbAngle32 angle);

    //設定
    void Set(const TbVector4& axisX,const TbVector4& axisY,const TbVector4& axisZ);

    //設定
    void Set(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,TbFloat32 m03,
             TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,TbFloat32 m13,
             TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,TbFloat32 m23,
             TbFloat32 m30,TbFloat32 m31,TbFloat32 m32,TbFloat32 m33);

    // 移動量設定
    void SetTranslation( TbFloat32 x,TbFloat32 y,TbFloat32 z );

    // 移動量設定
    void SetTranslation( const TbVector4& vec ) {
        SetTranslation( vec.GetX() , vec.GetY() , vec.GetZ() );
    }

    // 移動量設定
    void SetTranslation( const TbVector3& vec ) {
        SetTranslation( vec.GetX() , vec.GetY() , vec.GetZ() );
    }

    // 移動量加算
    void AddTranslation( const TbVector3& vec ) {
        TbVector3 t;
        GetTranslation(t);
        t += vec;
        SetTranslation( t );
    }

    // 移動量取得
    void GetTranslation( TbVector3& dst ) const;

    // 移動量取得
    void GetTranslation( TbVector4& dst ) const;

    // 回転成分取得
    TbMatrix44 GetRotation() const;

public:

    //逆行列化
    TbBool Invert();

    //逆行列結果を取得
    TbBool Invert(TbMatrix44& dst) const;

    // 転置
    void Transpose();

    // 転置結果を取得
    void Transpose(TbMatrix44& dst) const;

public:

    //平行移動
    void Translate(TbFloat32 x,TbFloat32 y,TbFloat32 z);

    //平行移動
    void Translate(const TbVector4& vec) {
        Translate( vec.GetX() , vec.GetY() , vec.GetZ() );
    }

    //平行移動
    void Translate(const TbVector3& vec) {
        Translate( vec.GetX() , vec.GetY() , vec.GetZ() );
    }

    //X軸回転
    void RotateX(const TbAngle32 angle);

    //Y軸回転
    void RotateY(const TbAngle32 angle);

    //Z軸回転
    void RotateZ(const TbAngle32 angle);

    //任意軸回転
    void RotateAxis(const TbFloat32 vx,const TbFloat32 vy,const TbFloat32 vz,const TbAngle32 angle);
    void RotateAxis(const TbVector3& vec,const TbAngle32 angle) {
        RotateAxis( vec.GetX() , vec.GetY() , vec.GetZ() , angle );
    }
    void RotateAxis(const TbVector4& vec,const TbAngle32 angle) {
        RotateAxis( vec.GetX() , vec.GetY() , vec.GetZ() , angle );
    }

    //拡大
    void Scale(TbFloat32 x,TbFloat32 y,TbFloat32 z);

public:

    //X軸ベクトル取得
    TbVector4 GetAxisX() const;

    //Y軸ベクトル取得
    TbVector4 GetAxisY() const;

    //Z軸ベクトル取得
    TbVector4 GetAxisZ() const;

    // オイラー角に変換(TbAngle32)
    TbVector3 GetEulerAngXYZ() const;

    // オイラー角に変換(ラジアン)
    TbVector3 GetEulerRadXYZ() const;

    // オイラー角に変換(Degree)
    TbVector3 GetEulerDegXYZ() const;

private:
    TbFloat32 m_Element[4][4];
};

/*!
 * 行列クラス
 */
class TbMatrix43
{
public:

    //単位行列
    const static TbMatrix43 IDENTITY;

public:

    //コンストラクタ
    TbMatrix43(void);
    TbMatrix43(const TbMatrix43& mtx);
    TbMatrix43(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,
               TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,
               TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,
               TbFloat32 m30,TbFloat32 m31,TbFloat32 m32);
    //コンストラクタ
    ~TbMatrix43(void);

public:// 演算子

    TbMatrix43& operator=(const TbMatrix43& mtx);
    TbMatrix43& operator=(const TbMatrix44& mtx);
    TbVector3    operator*(const TbVector4& vector ) const;
    TbVector3    operator*(const TbVector3& vector ) const;

public:

    //設定
    void Set(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,
             TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,
             TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,
             TbFloat32 m30,TbFloat32 m31,TbFloat32 m32);

    //単位行列
    void Unit();

public:

    // 転置(強引に3x4になるので注意)
    void Transpose();

    // 転置結果を取得
    void Transpose(TbMatrix43& dst) const;

private:
    TbFloat32 m_Element[12];
};

//無印行列は4行4列にしておく
typedef TbMatrix44 TbMatrix;

}

#endif