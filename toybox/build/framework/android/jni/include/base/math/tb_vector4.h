/*!
 * @file
 * @brief 3次元ベクトルクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_VECTOR4_H_
#define _INCLUDED_TB_VECTOR4_H_

namespace toybox
{

/*!
 * 3次元ベクトルクラス
 */
class TbVector4
{
public:

    //ゼロベクトル
    const static TbVector4 ZERO;
    const static TbVector4 AXISX;
    const static TbVector4 AXISY;
    const static TbVector4 AXISZ;

public:

    //コンストラクタ
    TbVector4();
    TbVector4(const TbVector4& vec);
    TbVector4(TbFloat32 x,TbFloat32 y,TbFloat32 z);
    TbVector4(TbFloat32 x,TbFloat32 y,TbFloat32 z,TbFloat32 w);
    explicit TbVector4(const TbFloat32* array);

    //デストラクタ
    ~TbVector4(void);

public: //演算子

    TbVector4& operator =  (const TbVector4& vec);
    TbVector4& operator =  (const TbFloat32* array);
    TbVector4& operator += (const TbVector4& vec);
    TbVector4& operator -= (const TbVector4& vec);
    TbVector4& operator *= (const TbFloat32 val);
    TbVector4& operator /= (const TbFloat32 val);
    TbVector4 operator + (const TbVector4& vec) const;
    TbVector4 operator - () const;
    TbVector4 operator - (const TbVector4& vec) const;
    TbVector4 operator * (const TbFloat32 val) const;
    TbVector4 operator / (const TbFloat32 val) const;
    TbBool     operator == (const TbVector4& vec) const;
    TbBool     operator != (const TbVector4& vec) const;

public: //クラス関数

    //線形補完
    static TbVector4 Lerp(const TbVector4& left,const TbVector4& right, const TbFloat32 interpolater);

public:

    //X値取得
    void SetX( TbFloat32 x );
    TbFloat32 GetX() const;

    //Y値取得
    void SetY( TbFloat32 y );
    TbFloat32 GetY() const;

    //Z値取得
    void SetZ( TbFloat32 z );
    TbFloat32 GetZ() const;
    
    // W値取得
    void SetW( TbFloat32 w );
    TbFloat32 GetW() const;

    //XY値
    void SetXY( TbFloat32 x , TbFloat32 y );
    void SetYZ( TbFloat32 y , TbFloat32 z );
    void SetXZ( TbFloat32 x , TbFloat32 z );

    //クリア
    void Clear();

    //設定
    void Set(TbFloat32 x,TbFloat32 y,TbFloat32 z);

    //設定
    void Set(TbFloat32 x,TbFloat32 y,TbFloat32 z,TbFloat32 w);

    //加算
    void Add(TbFloat32 x,TbFloat32 y,TbFloat32 z);
    
    //加算
    void Scale(TbFloat32 x,TbFloat32 y,TbFloat32 z) {
        m_X *= x;
        m_Y *= y;
        m_Z *= z;
    }

    //正規化
    void Normalize();

    //内積
    TbFloat32 Dot(const TbVector4& vec) const;

    //外積
    TbVector4 Cross(const TbVector4& vec) const;

    //距離
    TbFloat32 Length() const;

    //距離(距離の二乗を返す)
    TbFloat32 LengthSq() const {
        return (m_X * m_X) + (m_Y * m_Y) + (m_Z*m_Z);
    }

    //差分
    TbFloat32 Distance(const TbVector4& vec) const;

private:
    TbFloat32 m_X;  //!< x成分
    TbFloat32 m_Y;  //!< y成分
    TbFloat32 m_Z;  //!< z成分
    TbFloat32 m_W;  //!< w成分
};

}

#endif