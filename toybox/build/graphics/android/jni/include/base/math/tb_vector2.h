/*!
 * @file
 * @brief 2次元ベクトルクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_VECTOR2_H_
#define _INCLUDED_TB_VECTOR2_H_

#include <base/math/tb_math_util.h>

namespace toybox
{

class TbVector2
{
public:

    //ゼロベクトル
    const static TbVector2 ZERO;
    const static TbVector2 AXISX;
    const static TbVector2 AXISY;

public:

    //コンストラクタ
    TbVector2();
    TbVector2(const TbVector2& vec);
    TbVector2(TbFloat32 x,TbFloat32 y);

    //デストラクタ
    ~TbVector2(void);

public: //演算子

    TbVector2& operator =  (const TbVector2& vec);
    TbVector2& operator += (const TbVector2& vec);
    TbVector2& operator -= (const TbVector2& vec);
    TbVector2& operator *= (const TbFloat32 val);
    TbVector2& operator /= (const TbFloat32 val);

    TbVector2 operator + (const TbVector2& vec) const;
    TbVector2 operator - (const TbVector2& vec) const;
    TbVector2 operator - () const;
    TbVector2 operator * (const TbFloat32 val) const;
    TbVector2 operator / (const TbFloat32 val) const;

    TbBool     operator == (const TbVector2& vec) const;
    TbBool     operator != (const TbVector2& vec) const;

public:

    //クリア
    void Clear();

    // X成分設定
    void SetX( TbFloat32 x ) {
        m_X  = x;
    }

    // Y成分設定
    void SetY( TbFloat32 y ) {
        m_Y  = y;
    }

    //! X成分取得
    TbFloat32 GetX() const {
        return m_X;
    }

    //! Y成分取得
    TbFloat32 GetY() const {
        return m_Y;
    }

    //設定
    void Set(TbFloat32 x,TbFloat32 y);

    //加算
    void Add(TbFloat32 x,TbFloat32 y);

    // 乗算
    void Multiply( TbFloat32 x , TbFloat32 y );

    // 回転
    void Rotate( TbAngle32 angle );
    
    //正規化
    void Normalize();

    // 正規化ベクトル取得
    TbVector2 GetNormal() const {
        TbVector2 vec = *this;
        vec.Normalize();
        return vec;
    }

    //内積
    TbFloat32 Dot(const TbVector2& vec) const;

    //外積
    TbFloat32 Cross(const TbVector2& vec) const;

    //距離
    TbFloat32 Length() const;

    //距離(距離の二乗を返す)
    TbFloat32 LengthSq() const {
        return (m_X * m_X) + (m_Y * m_Y);
    }
    
    // 値が0か
    TbFloat32 IsZero() const {
        return TB_FLOAT64_EQUAL_ZERO(m_X) && TB_FLOAT64_EQUAL_ZERO(m_Y);
    }

    //差分
    TbFloat32 Distance(const TbVector2& vec) const;

    // 非数を持っているか
    TbBool HasNan() const {
        return TbIsNan(m_X) || TbIsNan(m_Y); 
    }

    // 小数点切り捨て
    void Floor()
    {
        m_X = TbFloor(m_X);
        m_Y = TbFloor(m_Y);
    }

    // 小数点切り上げ
    void Ceil()
    {
        m_X = TbCeil(m_X);
        m_Y = TbCeil(m_Y);
    }

public:

    TbFloat32 m_X;  // X成分
    TbFloat32 m_Y;  // Y成分

};

}

#endif