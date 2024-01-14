/*!
 * @file
 * @brief 3次元ベクトルクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_VECTOR3_H_
#define _INCLUDED_TB_VECTOR3_H_

namespace toybox
{

/*!
 * 3次元ベクトルクラス
 */
class TbVector3
{
public:

    //ゼロベクトル
    const static TbVector3 ZERO;
    const static TbVector3 AXISX;
    const static TbVector3 AXISY;
    const static TbVector3 AXISZ;

public:

    //コンストラクタ
    TbVector3();
    TbVector3(const TbVector3& vec);
    TbVector3(TbFloat32 x,TbFloat32 y,TbFloat32 z);
    explicit TbVector3(const TbFloat32* array);

    //デストラクタ
    ~TbVector3(void);

public: //演算子

    TbVector3& operator =  (const TbVector3& vec);
    TbVector3& operator =  (const TbFloat32* array);
    TbVector3& operator += (const TbVector3& vec);
    TbVector3& operator -= (const TbVector3& vec);
    TbVector3& operator *= (const TbFloat32 val);
    TbVector3& operator /= (const TbFloat32 val);
    TbVector3 operator + (const TbVector3& vec) const;
    TbVector3 operator - () const;
    TbVector3 operator - (const TbVector3& vec) const;
    TbVector3 operator * (const TbFloat32 val) const;
    TbVector3 operator / (const TbFloat32 val) const;
    TbBool     operator == (const TbVector3& vec) const;
    TbBool     operator != (const TbVector3& vec) const;

public: //クラス関数

    //線形補完
    static TbVector3 Lerp(const TbVector3& left,const TbVector3& right, const TbFloat32 interpolater);

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
    
    //XY値
    void SetXY( TbFloat32 x , TbFloat32 y );
    void SetYZ( TbFloat32 y , TbFloat32 z );
    void SetXZ( TbFloat32 x , TbFloat32 z );

    //クリア
    void Clear();

    //設定
    void Set(TbFloat32 x,TbFloat32 y,TbFloat32 z);

    // 設定
    void Set(const TbFloat32* array) {
        m_X = array[0]; 
        m_Y = array[1]; 
        m_Z = array[2];
    }

    //加算
    void Add(TbFloat32 x,TbFloat32 y,TbFloat32 z);
    
    //正規化
    void Normalize();

    //内積
    TbFloat32 Dot(const TbVector3& vec) const;

    //外積
    TbVector3 Cross(const TbVector3& vec) const;

    //距離
    TbFloat32 Length() const;

    //距離(距離の二乗を返す)
    TbFloat32 LengthSq() const {
        return (m_X * m_X) + (m_Y * m_Y) + (m_Z*m_Z);
    }

    //差分
    TbFloat32 Distance(const TbVector3& vec) const;

private:
    TbFloat32 m_X;  //!< x成分
    TbFloat32 m_Y;  //!< y成分
    TbFloat32 m_Z;  //!< z成分
};

}

#endif