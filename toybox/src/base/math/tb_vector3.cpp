/*!
 * 3次元ベクトルクラスの実装ファイル
 * @author Miayke Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_vector3.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

const TbVector3 TbVector3::ZERO;
const TbVector3 TbVector3::AXISX(1.0f,0.0f,0.0f);
const TbVector3 TbVector3::AXISY(0.0f,1.0f,0.0f);
const TbVector3 TbVector3::AXISZ(0.0f,0.0f,1.0f);

/*!
 * コンストラクタ
 */
TbVector3::TbVector3(void)
    :m_X(0.0f),m_Y(0.0f),m_Z(0.0f)
{
}

/*!
 * コンストラクタ
 */
TbVector3::TbVector3(const TbVector3& vec)
    :m_X(vec.m_X),m_Y(vec.m_Y),m_Z(vec.m_Z)
{
}

/*!
 * コンストラクタ
 */
TbVector3::TbVector3(TbFloat32 x,TbFloat32 y,TbFloat32 z)
    :m_X(x),m_Y(y),m_Z(z)
{
}

/*!
 * コンストラクタ
 */
TbVector3::TbVector3(const TbFloat32* array)
    :m_X(array[0]),m_Y(array[1]),m_Z(array[2])
{
}


/*!
 * デストラクタ
 */
TbVector3::~TbVector3()
{
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
TbVector3& TbVector3::operator = (const TbVector3& vec)
{
    TbMemCopy( this , &vec , sizeof( *this ) );
    return *this;
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
TbVector3& TbVector3::operator=(const TbFloat32* array)
{
    TbMemCopy( this , array , sizeof( *this ) );
    return *this;
}

/*!
 * 加算代入演算子
 */
TbVector3& TbVector3::operator += (const TbVector3& vec)
{
    m_X += vec.m_X;
    m_Y += vec.m_Y;
    m_Z += vec.m_Z;
    return *this;
}

/*!
 * 引算代入演算子
 */
TbVector3& TbVector3::operator -= (const TbVector3& vec)
{
    m_X -= vec.m_X;
    m_Y -= vec.m_Y;
    m_Z -= vec.m_Z;
    return *this;
}


/*!
 * 乗算代入演算子
 */
TbVector3& TbVector3::operator *= (const TbFloat32 val)
{
    m_X *= val;
    m_Y *= val;
    m_Z *= val;
    return *this;
}

/*!
 * 割算代入演算子
 */
TbVector3& TbVector3::operator /= (const TbFloat32 val)
{
    m_X /= val;
    m_Y /= val;
    m_Z /= val;
    return *this;
}

/*!
 * 加算演算子
 */
TbVector3 TbVector3::operator + (const TbVector3& vec) const
{
    TbVector3 result(m_X+vec.m_X,m_Y+vec.m_Y,m_Z+vec.m_Z);
    return result;
}

/*!
 * 引算演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 TbVector3::operator - (const TbVector3& vec) const
{
    TbVector3 result(m_X-vec.m_X,m_Y-vec.m_Y,m_Z-vec.m_Z);
    return result;
}

/*!
 * 単項-演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 TbVector3::operator - () const
{
    TbVector3 result(-m_X,-m_Y,-m_Z);
    return result;
}


/*!
 * 乗算演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 TbVector3::operator * (const TbFloat32 val) const
{
    TbVector3 result(m_X*val,m_Y*val,m_Z*val);
    return result;
}

/*!
 * 割算演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 TbVector3::operator / (const TbFloat32 val) const
{
    TbVector3 result(m_X/val,m_Y/val,m_Z/val);
    return result;
}

/*!
 * ==比較演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbBool     TbVector3::operator == (const TbVector3& vec) const
{
    return (TB_FLOAT32_EQUAL(m_X,vec.m_X) && TB_FLOAT32_EQUAL(m_Y,vec.m_Y) && TB_FLOAT32_EQUAL(m_Z,vec.m_Z));
}

/*!
 * !=比較演算子
 * @param vec ベクトル
 * @retval TB_TRUE  不一致
 * @retval TB_FALSE 一致
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbBool     TbVector3::operator != (const TbVector3& vec) const
{
    return !TB_FLOAT32_EQUAL(m_X,vec.m_X) || 
           !TB_FLOAT32_EQUAL(m_Y,vec.m_Y) || 
           !TB_FLOAT32_EQUAL(m_Z,vec.m_Z);
}

/*!
 * 線形補完
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 Lerp(const TbVector3& left,const TbVector3& right, const TbFloat32 interpolater)
{
    return left + (right-left) * interpolater;
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector3::Clear()
{
    m_X = m_Y = m_Z = 0.0f;
}

/*!
 * X値設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector3::SetX( TbFloat32 x ) 
{ 
    m_X = x; 
}

/*!
 * X座標取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector3::GetX() const 
{ 
    return m_X; 
}

/*!
 * Y値設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector3::SetY( TbFloat32 y ) 
{ 
    m_Y = y; 
}

/*!
 * Y値取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector3::GetY() const 
{ 
    return m_Y; 
}

/*!
 * Z値設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector3::SetZ( TbFloat32 z ) 
{ 
    m_Z = z; 
}

/*!
 * Z値取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector3::GetZ() const 
{ 
    return m_Z; 
}

/*!
 * XY値
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector3::SetXY( TbFloat32 x , TbFloat32 y ) 
{ 
    m_X = x; 
    m_Y = y; 
}

/*!
 * YZ座標設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector3::SetYZ( TbFloat32 y , TbFloat32 z ) 
{ 
    m_Y = y; 
    m_Z = z; 
}

/*!
 * XZ座標設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector3::SetXZ( TbFloat32 x , TbFloat32 z ) 
{ 
    m_X = x; 
    m_Z = z; 
}

/*!
 * 設定
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector3::Set(TbFloat32 x,TbFloat32 y,TbFloat32 z)
{
    m_X = x;
    m_Y = y;
    m_Z = z;
}

/*!
 * 設定
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector3::Add(TbFloat32 x,TbFloat32 y,TbFloat32 z)
{
    m_X += x;
    m_Y += y;
    m_Z += z;
}

/*!
 * 正規化
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector3::Normalize()
{
    TbFloat32 len = Length();
    if(len != 0.0f){
        m_X /= len;
        m_Y /= len;
        m_Z /= len;
    }
}

/*!
 * 内積
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbFloat32 TbVector3::Dot(const TbVector3& vec) const
{
    return (m_X * vec.m_X) + (m_Y * vec.m_Y) + (m_Z * vec.m_Z);
}

/*!
 * 外積
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 TbVector3::Cross(const TbVector3& vec) const
{
    TbVector3 result(m_Y*vec.m_Z-m_Z*vec.m_Y,m_Z*vec.m_X-m_X*vec.m_Z,m_X*vec.m_Y-m_Y*vec.m_X);
    return result;
}

/*!
 * 距離
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbFloat32 TbVector3::Length() const
{
    return TbSqrt((m_X*m_X)+(m_Y*m_Y)+(m_Z*m_Z));
}

/*!
 * 差分
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbFloat32 TbVector3::Distance(const TbVector3& vec) const
{
    return TbSqrt(TbPow(m_X-vec.m_X,2.0f) + TbPow(m_Y-vec.m_Y,2.0f) + TbPow(m_Z-vec.m_Z,2.0f));
}

/*!
 * 線形補完
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector3 TbVector3::Lerp(const TbVector3& left,const TbVector3& right, const TbFloat32 interpolater)
{
    return  ((right-left) * interpolater) + left;
}

}
