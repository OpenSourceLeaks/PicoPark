/*!
 * @file
 * @brief 2次元ベクトルのテンプレートクラス実装ファイル
 * @author Miyake Shunsuke
 */

#include <tb_fwd.h>
#include <base/math/tb_vector2.h>

namespace toybox
{

const TbVector2 TbVector2::ZERO;
const TbVector2 TbVector2::AXISX((1),(0));
const TbVector2 TbVector2::AXISY((0),(1));

/*!
 * コンストラクタ
 */
TbVector2::TbVector2(void)
    :m_X(0),m_Y(0)
{
}

/*!
 * コンストラクタ
 */
TbVector2::TbVector2(const TbVector2& vec)
    :m_X(vec.m_X),m_Y(vec.m_Y)
{
}

/*!
 * コンストラクタ
 */
TbVector2::TbVector2(TbFloat32 m_X,TbFloat32 m_Y)
    :m_X(m_X),m_Y(m_Y)
{
}

/*!
 * デストラクタ
 */
TbVector2::~TbVector2()
{
}

/*!
 * 代入演算子
 */
TbVector2& TbVector2::operator =  (const TbVector2& vec)
{
    m_X = vec.m_X;
    m_Y = vec.m_Y;
    return *this;
}

/*!
 * 加算代入演算子
 */
TbVector2& TbVector2::operator += (const TbVector2& vec)
{
    m_X += vec.m_X;
    m_Y += vec.m_Y;
    return *this;
}

/*!
 * 引算代入演算子
 */
TbVector2& TbVector2::operator -= (const TbVector2& vec)
{
    m_X -= vec.m_X;
    m_Y -= vec.m_Y;
    return *this;
}


/*!
 * 乗算代入演算子
 */
TbVector2& TbVector2::operator *= (const TbFloat32 val)
{
    m_X *= val;
    m_Y *= val;
    return *this;
}

/*!
 * 割算代入演算子
 */
TbVector2& TbVector2::operator /= (const TbFloat32 val)
{
    m_X /= val;
    m_Y /= val;
    return *this;
}

/*!
 * 加算演算子
 */
TbVector2 TbVector2::operator + (const TbVector2& vec) const
{
    TbVector2 result(m_X+vec.m_X,m_Y+vec.m_Y);
    return result;
}

/*!
 * 引算演算子
 */
TbVector2 TbVector2::operator - (const TbVector2& vec) const
{
    TbVector2 result(m_X-vec.m_X,m_Y-vec.m_Y);
    return result;
}

/*!
 * 引算演算子
 */
TbVector2 TbVector2::operator - () const
{
    TbVector2 result(-m_X,-m_Y);
    return result;
}

/*!
 * 乗算演算子
 */
TbVector2 TbVector2::operator * (const TbFloat32 val) const
{
    TbVector2 result(m_X*val,m_Y*val);
    return result;
}

/*!
 * 割算演算子
 */
TbVector2 TbVector2::operator / (const TbFloat32 val) const
{
    TbVector2 result(m_X/val,m_Y/val);
    return result;
}

/*!
 * ==比較演算子
 */
TbBool     TbVector2::operator == (const TbVector2& vec) const
{
    return (TB_FLOAT32_EQUAL(m_X,vec.m_X) && TB_FLOAT32_EQUAL(m_Y,vec.m_Y));
}

/*!
 * !=比較演算子
 * @param vec ベクトル
 * @retval TB_TRUE  不一致
 * @retval TB_FALSE 一致
 */
TbBool     TbVector2::operator != (const TbVector2& vec) const
{
    return !TB_FLOAT32_EQUAL(m_X,vec.m_X) || !TB_FLOAT32_EQUAL(m_Y,vec.m_Y);
}

/*!
 * クリア
 */
void TbVector2::Clear()
{
    m_X = m_Y = (TbFloat32)0;
}

/*!
 * 設定
 */
void TbVector2::Set(TbFloat32 x,TbFloat32 y)
{
    m_X = x;
    m_Y = y;
}

/*!
 * 設定
 */
void TbVector2::Add(TbFloat32 x,TbFloat32 y)
{
    m_X += x;
    m_Y += y;
}

/*!
 * 乗算
 */
void TbVector2::Multiply(TbFloat32 x, TbFloat32 y )
{
    m_X *= x;
    m_Y *= y;
}

/*!
 * 回転
 * @author Miyake_Shunske
 * @since 2015.01.25
 */ 
void TbVector2::Rotate( TbAngle32 angle )
{
    TbFloat32 rad = TbAng32ToRad(angle);
    TbFloat32 c = TbCos(rad);
    TbFloat32 s = TbSin(rad);
    TbFloat32 x = m_X;
    TbFloat32 y = m_Y;
    m_X = x * c - y * s;
    m_Y = x * s + y * c;
}

/*!
 * 正規化
 */
void TbVector2::Normalize()
{
    TbFloat32 len = Length();
    if(len != 0.0f){
        m_X = ((m_X)/len);
        m_Y = ((m_Y)/len);
    }
}

/*!
 * 内積
 */
TbFloat32 TbVector2::Dot(const TbVector2& vec) const
{
    return ((m_X) * (vec.m_X)) + ((m_Y) * (vec.m_Y));
}

/*!
 * 外積
 */
TbFloat32 TbVector2::Cross(const TbVector2& vec) const
{
    return ((m_X) * (vec.m_Y)) - ((m_Y) * (vec.m_X));
}

/*!
 * 距離
 */
TbFloat32 TbVector2::Length() const
{
    TbFloat32 fx = (m_X);
    TbFloat32 fy = (m_Y);
    return TbSqrt((fx*fx)+(fy*fy));
}

/*!
 * 差分
 */
TbFloat32 TbVector2::Distance(const TbVector2& vec) const
{
    return TbSqrt(TbPow((m_X)-(vec.m_X),2.0f) +
                  TbPow((m_Y)-(vec.m_Y),2.0f));
}

}
