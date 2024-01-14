/*!
 * 3次元ベクトルクラスの実装ファイル
 * @author Miayke Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_vector4.h"

#include "base/math/tb_math_util.h"

namespace toybox
{

const TbVector4 TbVector4::ZERO;
const TbVector4 TbVector4::AXISX(1.0f,0.0f,0.0f,1.0f);
const TbVector4 TbVector4::AXISY(0.0f,1.0f,0.0f,1.0f);
const TbVector4 TbVector4::AXISZ(0.0f,0.0f,1.0f,1.0f);

/*!
 * コンストラクタ
 */
TbVector4::TbVector4(void)
    :m_X(0.0f),m_Y(0.0f),m_Z(0.0f),m_W(0.0f)
{
}

/*!
 * コンストラクタ
 */
TbVector4::TbVector4(const TbVector4& vec)
    :m_X(vec.m_X),m_Y(vec.m_Y),m_Z(vec.m_Z),m_W(vec.m_W)
{
}

/*!
 * コンストラクタ
 */
TbVector4::TbVector4(TbFloat32 x,TbFloat32 y,TbFloat32 z)
    :m_X(x),m_Y(y),m_Z(z),m_W(1.0f)
{
}

/*!
 * コンストラクタ
 */
TbVector4::TbVector4(TbFloat32 x,TbFloat32 y,TbFloat32 z,TbFloat32 w)
    :m_X(x),m_Y(y),m_Z(z),m_W(w)
{
}

/*!
 * コンストラクタ
 */
TbVector4::TbVector4(const TbFloat32* array)
    :m_X(array[0]),m_Y(array[1]),m_Z(array[2]),m_W(array[3])
{
}


/*!
 * デストラクタ
 */
TbVector4::~TbVector4()
{
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
TbVector4& TbVector4::operator = (const TbVector4& vec)
{
    TbMemCopy( this , &vec , sizeof( *this ) );
    return *this;
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
TbVector4& TbVector4::operator=(const TbFloat32* array)
{
    TbMemCopy( this , array , sizeof( *this ) );
    return *this;
}

/*!
 * 加算代入演算子
 */
TbVector4& TbVector4::operator += (const TbVector4& vec)
{
    m_X += vec.m_X;
    m_Y += vec.m_Y;
    m_Z += vec.m_Z;
    return *this;
}

/*!
 * 引算代入演算子
 */
TbVector4& TbVector4::operator -= (const TbVector4& vec)
{
    m_X -= vec.m_X;
    m_Y -= vec.m_Y;
    m_Z -= vec.m_Z;
    return *this;
}


/*!
 * 乗算代入演算子
 */
TbVector4& TbVector4::operator *= (const TbFloat32 val)
{
    m_X *= val;
    m_Y *= val;
    m_Z *= val;
    return *this;
}

#if 0
/*!
 * 行列乗算
 */
TbVector4& TbVector4::operator *= (const TbMatrix44& mtx)
{
    TbFloat32 resultX = mtx.Get(0,0) * m_X + mtx.Get(1,0) * m_Y + mtx.Get(2,0) * m_Z + mtx.Get(3,0);
    TbFloat32 resultY = mtx.Get(0,1) * m_X + mtx.Get(1,1) * m_Y + mtx.Get(2,1) * m_Z + mtx.Get(3,1);
    TbFloat32 resultZ = mtx.Get(0,2) * m_X + mtx.Get(1,2) * m_Y + mtx.Get(2,2) * m_Z + mtx.Get(3,2);
    TbFloat32 resultW = mtx.Get(0,3) * m_X + mtx.Get(1,3) * m_Y + mtx.Get(2,3) * m_Z + mtx.Get(3,3);
    m_X = resultX / resultW;
    m_Y = resultY / resultW;
    m_Z = resultZ / resultW;
    return *this;
}
#endif

/*!
 * 割算代入演算子
 */
TbVector4& TbVector4::operator /= (const TbFloat32 val)
{
    m_X /= val;
    m_Y /= val;
    m_Z /= val;
    return *this;
}

/*!
 * 加算演算子
 */
TbVector4 TbVector4::operator + (const TbVector4& vec) const
{
    TbVector4 result(m_X+vec.m_X,m_Y+vec.m_Y,m_Z+vec.m_Z);
    return result;
}

/*!
 * 引算演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::operator - (const TbVector4& vec) const
{
    TbVector4 result(m_X-vec.m_X,m_Y-vec.m_Y,m_Z-vec.m_Z);
    return result;
}

/*!
 * 単項-演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::operator - () const
{
    TbVector4 result(-m_X,-m_Y,-m_Z);
    return result;
}


/*!
 * 乗算演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::operator * (const TbFloat32 val) const
{
    TbVector4 result(m_X*val,m_Y*val,m_Z*val);
    return result;
}

#if 0
/*!
 * 行列乗算
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::operator * (const TbMatrix44& mtx) const
{
    TbVector4 result;
    TbFloat32 resultX = mtx.Get(0,0) * m_X + mtx.Get(1,0) * m_Y + mtx.Get(2,0) * m_Z + mtx.Get(3,0);
    TbFloat32 resultY = mtx.Get(0,1) * m_X + mtx.Get(1,1) * m_Y + mtx.Get(2,1) * m_Z + mtx.Get(3,1);
    TbFloat32 resultZ = mtx.Get(0,2) * m_X + mtx.Get(1,2) * m_Y + mtx.Get(2,2) * m_Z + mtx.Get(3,2);
    TbFloat32 resultW = mtx.Get(0,3) * m_X + mtx.Get(1,3) * m_Y + mtx.Get(2,3) * m_Z + mtx.Get(3,3);
    result.m_X = resultX / resultW;
    result.m_Y = resultY / resultW;
    result.m_Z = resultZ / resultW;
    return result;
}
#endif

/*!
 * 割算演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::operator / (const TbFloat32 val) const
{
    TbVector4 result(m_X/val,m_Y/val,m_Z/val);
    return result;
}

/*!
 * ==比較演算子
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbBool     TbVector4::operator == (const TbVector4& vec) const
{
    return (m_X==vec.m_X && m_Y==vec.m_Y && m_Z==vec.m_Z);
}

/*!
 * !=比較演算子
 * @param vec ベクトル
 * @retval TB_TRUE  不一致
 * @retval TB_FALSE 一致
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbBool     TbVector4::operator != (const TbVector4& vec) const
{
    return (m_X!=vec.m_X || m_Y!=vec.m_Y || m_Z!=vec.m_Z || m_W!=vec.m_W);
}

/*!
 * 線形補完
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 Lerp(const TbVector4& left,const TbVector4& right, const TbFloat32 interpolater)
{
    return left + (right-left) * interpolater;
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector4::Clear()
{
    m_X = m_Y = m_Z = 0.0f;
}

/*!
 * X値設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetX( TbFloat32 x ) 
{ 
    m_X = x; 
}

/*!
 * X座標取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector4::GetX() const 
{ 
    return m_X; 
}

/*!
 * Y値設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetY( TbFloat32 y ) 
{ 
    m_Y = y; 
}

/*!
 * Y値取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector4::GetY() const 
{ 
    return m_Y; 
}

/*!
 * Z値設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetZ( TbFloat32 z ) 
{ 
    m_Z = z; 
}

/*!
 * Z値取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector4::GetZ() const 
{ 
    return m_Z; 
}
    
/*!
 * W値取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetW( TbFloat32 w ) 
{ 
    m_W = w; 
}

/*!
 * W座標取得
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbFloat32 TbVector4::GetW() const 
{ 
    return m_W; 
}

/*!
 * XY値
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetXY( TbFloat32 x , TbFloat32 y ) 
{ 
    m_X = x; 
    m_Y = y; 
}

/*!
 * YZ座標設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetYZ( TbFloat32 y , TbFloat32 z ) 
{ 
    m_Y = y; 
    m_Z = z; 
}

/*!
 * XZ座標設定
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbVector4::SetXZ( TbFloat32 x , TbFloat32 z ) 
{ 
    m_X = x; 
    m_Z = z; 
}

/*!
 * 設定
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector4::Set(TbFloat32 x,TbFloat32 y,TbFloat32 z)
{
    m_X = x;
    m_Y = y;
    m_Z = z;
}

/*!
 * 設定
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
void TbVector4::Set(TbFloat32 x,TbFloat32 y,TbFloat32 z,TbFloat32 w)
{
    m_X = x;
    m_Y = y;
    m_Z = z;
    m_W = w;
}

/*!
 * 設定
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
void TbVector4::Add(TbFloat32 x,TbFloat32 y,TbFloat32 z)
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
void TbVector4::Normalize()
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
TbFloat32 TbVector4::Dot(const TbVector4& vec) const
{
    return (m_X * vec.m_X) + (m_Y * vec.m_Y) + (m_Z * vec.m_Z);
}

/*!
 * 外積
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::Cross(const TbVector4& vec) const
{
    TbVector4 result(m_Y*vec.m_Z-m_Z*vec.m_Y,m_Z*vec.m_X-m_X*vec.m_Z,m_X*vec.m_Y-m_Y*vec.m_X);
    return result;
}

/*!
 * 距離
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbFloat32 TbVector4::Length() const
{
    return TbSqrt((m_X*m_X)+(m_Y*m_Y)+(m_Z*m_Z));
}

/*!
 * 差分
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbFloat32 TbVector4::Distance(const TbVector4& vec) const
{
    return TbSqrt(TbPow(m_X-vec.m_X,2.0f) + TbPow(m_Y-vec.m_Y,2.0f) + TbPow(m_Z-vec.m_Z,2.0f));
}

/*!
 * 線形補完
 * @author Miyake Shunsuke
 * @since 2010.12.01
 */
TbVector4 TbVector4::Lerp(const TbVector4& left,const TbVector4& right, const TbFloat32 interpolater)
{
    return  ((right-left) * interpolater) + left;
}

}
