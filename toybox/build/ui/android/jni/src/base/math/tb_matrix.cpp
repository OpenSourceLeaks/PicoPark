/*!
 * 行列クラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_matrix.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

const TbMatrix44 TbMatrix44::IDENTITY;
const TbMatrix43 TbMatrix43::IDENTITY;

#define GET_ELE(row,col) m_Element[row][col]    // row=縦、col=横
#define GET_ELE_MTX(mtx,row,col) mtx.m_Element[row][col]

#define GET_ELE43(row,col) m_Element[row*3+col]    // row=縦、col=横
#define GET_ELE_MTX43(mtx,row,col) mtx.m_Element[row*3+col]

/*!
 * コンストラクタ
 */
TbMatrix44::TbMatrix44(void)
{
    Unit();
}

/*!
 * コンストラクタ
 * @param mtx コピー元
 */
TbMatrix44::TbMatrix44(const TbMatrix44& mtx)
{
    TbMemCopy( this , &mtx , sizeof( *this ) );
}

/*!
 * コンストラクタ
 * @param vx    回転軸x成分
 * @param vy    回転軸y成分
 * @param vz    回転軸z成分
 * @param angle 回転角度
 */
TbMatrix44::TbMatrix44(const TbFloat32 vx,const TbFloat32 vy,const TbFloat32 vz,const TbAngle32 angle)
{
}

/*!
 * コンストラクタ
 */
TbMatrix44::TbMatrix44(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,TbFloat32 m03,
         TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,TbFloat32 m13,
         TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,TbFloat32 m23,
         TbFloat32 m30,TbFloat32 m31,TbFloat32 m32,TbFloat32 m33)
{
    Set(m00,m01,m02,m03,
        m10,m11,m12,m13,
        m20,m21,m22,m23,
        m30,m31,m32,m33);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix44::~TbMatrix44(void)
{
}

/*! 
 * 代入演算子
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix44& TbMatrix44::operator=(const TbMatrix44& mtx)
{
    TbMemCopy( this , &mtx , sizeof( mtx ) );
    return *this;
}

/*!
 * 加算代入演算子
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix44& TbMatrix44::operator+=(const TbMatrix44& mtx)
{
    GET_ELE(0,0) += GET_ELE_MTX(mtx,0,0);
    GET_ELE(0,1) += GET_ELE_MTX(mtx,0,1);
    GET_ELE(0,2) += GET_ELE_MTX(mtx,0,2);
    GET_ELE(0,3) += GET_ELE_MTX(mtx,0,3);
    GET_ELE(1,0) += GET_ELE_MTX(mtx,1,0);
    GET_ELE(1,1) += GET_ELE_MTX(mtx,1,1);
    GET_ELE(1,2) += GET_ELE_MTX(mtx,1,2);
    GET_ELE(1,3) += GET_ELE_MTX(mtx,1,3);
    GET_ELE(2,0) += GET_ELE_MTX(mtx,2,0);
    GET_ELE(2,1) += GET_ELE_MTX(mtx,2,1);
    GET_ELE(2,2) += GET_ELE_MTX(mtx,2,2);
    GET_ELE(2,3) += GET_ELE_MTX(mtx,2,3);
    GET_ELE(3,0) += GET_ELE_MTX(mtx,3,0);
    GET_ELE(3,1) += GET_ELE_MTX(mtx,3,1);
    GET_ELE(3,2) += GET_ELE_MTX(mtx,3,2);
    GET_ELE(3,3) += GET_ELE_MTX(mtx,3,3);
    return *this;
}

/*!
 * 引算代入演算子
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix44& TbMatrix44::operator-=(const TbMatrix44& mtx)
{
    GET_ELE(0,0) -= GET_ELE_MTX(mtx,0,0);
    GET_ELE(0,1) -= GET_ELE_MTX(mtx,0,1);
    GET_ELE(0,2) -= GET_ELE_MTX(mtx,0,2);
    GET_ELE(0,3) -= GET_ELE_MTX(mtx,0,3);
    GET_ELE(1,0) -= GET_ELE_MTX(mtx,1,0);
    GET_ELE(1,1) -= GET_ELE_MTX(mtx,1,1);
    GET_ELE(1,2) -= GET_ELE_MTX(mtx,1,2);
    GET_ELE(1,3) -= GET_ELE_MTX(mtx,1,3);
    GET_ELE(2,0) -= GET_ELE_MTX(mtx,2,0);
    GET_ELE(2,1) -= GET_ELE_MTX(mtx,2,1);
    GET_ELE(2,2) -= GET_ELE_MTX(mtx,2,2);
    GET_ELE(2,3) -= GET_ELE_MTX(mtx,2,3);
    GET_ELE(3,0) -= GET_ELE_MTX(mtx,3,0);
    GET_ELE(3,1) -= GET_ELE_MTX(mtx,3,1);
    GET_ELE(3,2) -= GET_ELE_MTX(mtx,3,2);
    GET_ELE(3,3) -= GET_ELE_MTX(mtx,3,3);
    return *this;
}

/*! 
 * 乗算代入演算子
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix44& TbMatrix44::operator*=(const TbMatrix44& mtx)
{
    (*this) = (*this) * mtx;
    return *this;
}


/*!
 * 加算演算子
 */
TbMatrix44 TbMatrix44::operator+(const TbMatrix44& mtx) const
{
    TbMatrix44 result(
        GET_ELE(0,0) + GET_ELE_MTX(mtx,0,0),
        GET_ELE(0,1) + GET_ELE_MTX(mtx,0,1),
        GET_ELE(0,2) + GET_ELE_MTX(mtx,0,2),
        GET_ELE(0,3) + GET_ELE_MTX(mtx,0,3),
        GET_ELE(1,0) + GET_ELE_MTX(mtx,1,0),
        GET_ELE(1,1) + GET_ELE_MTX(mtx,1,1),
        GET_ELE(1,2) + GET_ELE_MTX(mtx,1,2),
        GET_ELE(1,3) + GET_ELE_MTX(mtx,1,3),
        GET_ELE(2,0) + GET_ELE_MTX(mtx,2,0),
        GET_ELE(2,1) + GET_ELE_MTX(mtx,2,1),
        GET_ELE(2,2) + GET_ELE_MTX(mtx,2,2),
        GET_ELE(2,3) + GET_ELE_MTX(mtx,2,3),
        GET_ELE(3,0) + GET_ELE_MTX(mtx,3,0),
        GET_ELE(3,1) + GET_ELE_MTX(mtx,3,1),
        GET_ELE(3,2) + GET_ELE_MTX(mtx,3,2),
        GET_ELE(3,3) + GET_ELE_MTX(mtx,3,3)
    );
    return result;
}

/*!
 * 引算演算子
 */
TbMatrix44 TbMatrix44::operator-(const TbMatrix44& mtx) const
{
    TbMatrix44 result(
        GET_ELE(0,0) - GET_ELE_MTX(mtx,0,0),
        GET_ELE(0,1) - GET_ELE_MTX(mtx,0,1),
        GET_ELE(0,2) - GET_ELE_MTX(mtx,0,2),
        GET_ELE(0,3) - GET_ELE_MTX(mtx,0,3),
        GET_ELE(1,0) - GET_ELE_MTX(mtx,1,0),
        GET_ELE(1,1) - GET_ELE_MTX(mtx,1,1),
        GET_ELE(1,2) - GET_ELE_MTX(mtx,1,2),
        GET_ELE(1,3) - GET_ELE_MTX(mtx,1,3),
        GET_ELE(2,0) - GET_ELE_MTX(mtx,2,0),
        GET_ELE(2,1) - GET_ELE_MTX(mtx,2,1),
        GET_ELE(2,2) - GET_ELE_MTX(mtx,2,2),
        GET_ELE(2,3) - GET_ELE_MTX(mtx,2,3),
        GET_ELE(3,0) - GET_ELE_MTX(mtx,3,0),
        GET_ELE(3,1) - GET_ELE_MTX(mtx,3,1),
        GET_ELE(3,2) - GET_ELE_MTX(mtx,3,2),
        GET_ELE(3,3) - GET_ELE_MTX(mtx,3,3)
    );
    return result;
}

/*!
 * 乗算演算子
 */
TbMatrix44 TbMatrix44::operator*(const TbMatrix44& mtx) const
{
    TbMatrix44 result;
    const TbMatrix44& left = *this;
    const TbMatrix44& right = mtx;
    GET_ELE_MTX(result,0,0) = GET_ELE_MTX(right,0,0) * GET_ELE_MTX(left,0,0) + GET_ELE_MTX(right,0,1) * GET_ELE_MTX(left,1,0) + GET_ELE_MTX(right,0,2) * GET_ELE_MTX(left,2,0) + GET_ELE_MTX(right,0,3) * GET_ELE_MTX(left,3,0);
    GET_ELE_MTX(result,0,1) = GET_ELE_MTX(right,0,0) * GET_ELE_MTX(left,0,1) + GET_ELE_MTX(right,0,1) * GET_ELE_MTX(left,1,1) + GET_ELE_MTX(right,0,2) * GET_ELE_MTX(left,2,1) + GET_ELE_MTX(right,0,3) * GET_ELE_MTX(left,3,1);
    GET_ELE_MTX(result,0,2) = GET_ELE_MTX(right,0,0) * GET_ELE_MTX(left,0,2) + GET_ELE_MTX(right,0,1) * GET_ELE_MTX(left,1,2) + GET_ELE_MTX(right,0,2) * GET_ELE_MTX(left,2,2) + GET_ELE_MTX(right,0,3) * GET_ELE_MTX(left,3,2);
    GET_ELE_MTX(result,0,3) = GET_ELE_MTX(right,0,0) * GET_ELE_MTX(left,0,3) + GET_ELE_MTX(right,0,1) * GET_ELE_MTX(left,1,3) + GET_ELE_MTX(right,0,2) * GET_ELE_MTX(left,2,3) + GET_ELE_MTX(right,0,3) * GET_ELE_MTX(left,3,3);
    GET_ELE_MTX(result,1,0) = GET_ELE_MTX(right,1,0) * GET_ELE_MTX(left,0,0) + GET_ELE_MTX(right,1,1) * GET_ELE_MTX(left,1,0) + GET_ELE_MTX(right,1,2) * GET_ELE_MTX(left,2,0) + GET_ELE_MTX(right,1,3) * GET_ELE_MTX(left,3,0);
    GET_ELE_MTX(result,1,1) = GET_ELE_MTX(right,1,0) * GET_ELE_MTX(left,0,1) + GET_ELE_MTX(right,1,1) * GET_ELE_MTX(left,1,1) + GET_ELE_MTX(right,1,2) * GET_ELE_MTX(left,2,1) + GET_ELE_MTX(right,1,3) * GET_ELE_MTX(left,3,1);
    GET_ELE_MTX(result,1,2) = GET_ELE_MTX(right,1,0) * GET_ELE_MTX(left,0,2) + GET_ELE_MTX(right,1,1) * GET_ELE_MTX(left,1,2) + GET_ELE_MTX(right,1,2) * GET_ELE_MTX(left,2,2) + GET_ELE_MTX(right,1,3) * GET_ELE_MTX(left,3,2);
    GET_ELE_MTX(result,1,3) = GET_ELE_MTX(right,1,0) * GET_ELE_MTX(left,0,3) + GET_ELE_MTX(right,1,1) * GET_ELE_MTX(left,1,3) + GET_ELE_MTX(right,1,2) * GET_ELE_MTX(left,2,3) + GET_ELE_MTX(right,1,3) * GET_ELE_MTX(left,3,3);
    GET_ELE_MTX(result,2,0) = GET_ELE_MTX(right,2,0) * GET_ELE_MTX(left,0,0) + GET_ELE_MTX(right,2,1) * GET_ELE_MTX(left,1,0) + GET_ELE_MTX(right,2,2) * GET_ELE_MTX(left,2,0) + GET_ELE_MTX(right,2,3) * GET_ELE_MTX(left,3,0);
    GET_ELE_MTX(result,2,1) = GET_ELE_MTX(right,2,0) * GET_ELE_MTX(left,0,1) + GET_ELE_MTX(right,2,1) * GET_ELE_MTX(left,1,1) + GET_ELE_MTX(right,2,2) * GET_ELE_MTX(left,2,1) + GET_ELE_MTX(right,2,3) * GET_ELE_MTX(left,3,1);
    GET_ELE_MTX(result,2,2) = GET_ELE_MTX(right,2,0) * GET_ELE_MTX(left,0,2) + GET_ELE_MTX(right,2,1) * GET_ELE_MTX(left,1,2) + GET_ELE_MTX(right,2,2) * GET_ELE_MTX(left,2,2) + GET_ELE_MTX(right,2,3) * GET_ELE_MTX(left,3,2);
    GET_ELE_MTX(result,2,3) = GET_ELE_MTX(right,2,0) * GET_ELE_MTX(left,0,3) + GET_ELE_MTX(right,2,1) * GET_ELE_MTX(left,1,3) + GET_ELE_MTX(right,2,2) * GET_ELE_MTX(left,2,3) + GET_ELE_MTX(right,2,3) * GET_ELE_MTX(left,3,3);
    GET_ELE_MTX(result,3,0) = GET_ELE_MTX(right,3,0) * GET_ELE_MTX(left,0,0) + GET_ELE_MTX(right,3,1) * GET_ELE_MTX(left,1,0) + GET_ELE_MTX(right,3,2) * GET_ELE_MTX(left,2,0) + GET_ELE_MTX(right,3,3) * GET_ELE_MTX(left,3,0);
    GET_ELE_MTX(result,3,1) = GET_ELE_MTX(right,3,0) * GET_ELE_MTX(left,0,1) + GET_ELE_MTX(right,3,1) * GET_ELE_MTX(left,1,1) + GET_ELE_MTX(right,3,2) * GET_ELE_MTX(left,2,1) + GET_ELE_MTX(right,3,3) * GET_ELE_MTX(left,3,1);
    GET_ELE_MTX(result,3,2) = GET_ELE_MTX(right,3,0) * GET_ELE_MTX(left,0,2) + GET_ELE_MTX(right,3,1) * GET_ELE_MTX(left,1,2) + GET_ELE_MTX(right,3,2) * GET_ELE_MTX(left,2,2) + GET_ELE_MTX(right,3,3) * GET_ELE_MTX(left,3,2);
    GET_ELE_MTX(result,3,3) = GET_ELE_MTX(right,3,0) * GET_ELE_MTX(left,0,3) + GET_ELE_MTX(right,3,1) * GET_ELE_MTX(left,1,3) + GET_ELE_MTX(right,3,2) * GET_ELE_MTX(left,2,3) + GET_ELE_MTX(right,3,3) * GET_ELE_MTX(left,3,3);
    return result;
}

/*! 
 * ベクトル演算
 * @author Miyake Shunsuke
 * @since 2011.04.05
 */
TbVector4    TbMatrix44::operator*(const TbVector4& vector ) const
{
    TbVector4 result;
    result.SetX( vector.GetX() * GET_ELE(0,0) + vector.GetY() * GET_ELE(1,0) + vector.GetZ() * GET_ELE(2,0) + vector.GetW() * GET_ELE(3,0) );
    result.SetY( vector.GetX() * GET_ELE(0,1) + vector.GetY() * GET_ELE(1,1) + vector.GetZ() * GET_ELE(2,1) + vector.GetW() * GET_ELE(3,1) );
    result.SetZ( vector.GetX() * GET_ELE(0,2) + vector.GetY() * GET_ELE(1,2) + vector.GetZ() * GET_ELE(2,2) + vector.GetW() * GET_ELE(3,2) );
    result.SetW( vector.GetX() * GET_ELE(0,3) + vector.GetY() * GET_ELE(1,3) + vector.GetZ() * GET_ELE(2,3) + vector.GetW() * GET_ELE(3,3) );
    return result;
}

/*! 
 * ベクトル演算
 * @author Miyake Shunsuke
 * @since 2011.04.05
 */
TbVector3    TbMatrix44::operator*(const TbVector3& vector ) const
{
    TbVector3 result;
    result.SetX( vector.GetX() * GET_ELE(0,0) + vector.GetY() * GET_ELE(1,0) + vector.GetZ() * GET_ELE(2,0) + 1.0f * GET_ELE(3,0) );
    result.SetY( vector.GetX() * GET_ELE(0,1) + vector.GetY() * GET_ELE(1,1) + vector.GetZ() * GET_ELE(2,1) + 1.0f * GET_ELE(3,1) );
    result.SetZ( vector.GetX() * GET_ELE(0,2) + vector.GetY() * GET_ELE(1,2) + vector.GetZ() * GET_ELE(2,2) + 1.0f * GET_ELE(3,2) );
    return result;
}

/*! 
 * ==比較演算
 * @author Miyake Shunsuke
 * @since 2011.04.05
 */
TbBool      TbMatrix44::operator==(const TbMatrix44& mtx)
{
    return 0==TbMemCmp(this,&mtx,sizeof(*this));
}

/*! 
 * !=比較演算
 * @author Miyake Shunsuke
 * @since 2011.04.05
 */
TbBool      TbMatrix44::operator!=(const TbMatrix44& mtx)
{
    return 0!=TbMemCmp(this,&mtx,sizeof(*this));
}

/*!
 * 要素取得
 */
TbFloat32 TbMatrix44::Get(TbUint32 i,TbUint32 j) const
{
#if 0
    if( i >= 4 || j >= 4){
        TB_ASSERT("invalid index");
        return 0.0f;
    }
#endif
    return GET_ELE(i,j);
}

/*!
 * 設定
 * @author Miyake Shunsuke
 * @since 2011.04.03
 */
void TbMatrix44::Set(TbUint32 i , TbUint32 j , const TbFloat32 value )
{
    GET_ELE(i,j) = value;
}

/*!
 * 設定
 * 任意軸から
 */
void TbMatrix44::Set(const TbFloat32 vx,const TbFloat32 vy,const TbFloat32 vz,const TbAngle32 angle)
{
    Unit();
    RotateAxis( vx , vy , vz , angle );
}

/*! 
 * 設定
 * 各軸から設定
 */ 
void TbMatrix44::Set(const TbVector4& axisX,const TbVector4& axisY,const TbVector4& axisZ)
{
    GET_ELE(0,0) = axisX.GetX();
    GET_ELE(0,1) = axisX.GetY();
    GET_ELE(0,2) = axisX.GetZ();
    GET_ELE(0,3) = 0.0f;
    GET_ELE(1,0) = axisY.GetX();
    GET_ELE(1,1) = axisY.GetY();
    GET_ELE(1,2) = axisY.GetZ();
    GET_ELE(1,3) = 0.0f;
    GET_ELE(2,0) = axisZ.GetX();
    GET_ELE(2,1) = axisZ.GetY();
    GET_ELE(2,2) = axisZ.GetZ();
    GET_ELE(2,3) = 0.0f;
    GET_ELE(3,0) = 0.0f;
    GET_ELE(3,1) = 0.0f;
    GET_ELE(3,2) = 0.0f;
    GET_ELE(3,3) = 1.0f;
}

/*!
 * 設定
 */
void TbMatrix44::Set(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,TbFloat32 m03,
                     TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,TbFloat32 m13,
                     TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,TbFloat32 m23,
                     TbFloat32 m30,TbFloat32 m31,TbFloat32 m32,TbFloat32 m33)
{
    m_Element[0][0] = m00;
    m_Element[0][1] = m01;
    m_Element[0][2] = m02;
    m_Element[0][3] = m03;
    m_Element[1][0] = m10;
    m_Element[1][1] = m11;
    m_Element[1][2] = m12;
    m_Element[1][3] = m13;
    m_Element[2][0] = m20;
    m_Element[2][1] = m21;
    m_Element[2][2] = m22;
    m_Element[2][3] = m23;
    m_Element[3][0] = m30;
    m_Element[3][1] = m31;
    m_Element[3][2] = m32;
    m_Element[3][3] = m33;
}

/*!
 * 単位行列
 */
void TbMatrix44::Unit()
{
    Set( 1.0f , 0.0f , 0.0f , 0.0f ,
         0.0f , 1.0f , 0.0f , 0.0f ,
         0.0f , 0.0f , 1.0f , 0.0f ,
         0.0f , 0.0f , 0.0f , 1.0f );
}

/*!
 * 逆行列化
 */
TbBool TbMatrix44::Invert()
{
    TbMatrix44 mtx = *this;
    return mtx.Invert( *this );
}

/*!
 * 逆行列結果を取得
 * @param mtx 結果格納先
 */
TbBool TbMatrix44::Invert(TbMatrix44& dst) const
{
    // 逆行列可能か
    TbFloat32 a = (GET_ELE(0,0) * GET_ELE(1,1) * GET_ELE(2,2) * GET_ELE(3,3) ) + 
                  (GET_ELE(0,0) * GET_ELE(1,2) * GET_ELE(2,3) * GET_ELE(3,1) ) + 
                  (GET_ELE(0,0) * GET_ELE(1,3) * GET_ELE(2,1) * GET_ELE(3,2) ) + 

                  (GET_ELE(0,1) * GET_ELE(1,0) * GET_ELE(2,3) * GET_ELE(3,2) ) + 
                  (GET_ELE(0,1) * GET_ELE(1,2) * GET_ELE(2,0) * GET_ELE(3,3) ) + 
                  (GET_ELE(0,1) * GET_ELE(1,3) * GET_ELE(2,2) * GET_ELE(3,0) ) + 

                  (GET_ELE(0,2) * GET_ELE(1,0) * GET_ELE(2,1) * GET_ELE(3,3) ) + 
                  (GET_ELE(0,2) * GET_ELE(1,1) * GET_ELE(2,3) * GET_ELE(3,0) ) + 
                  (GET_ELE(0,2) * GET_ELE(1,3) * GET_ELE(2,0) * GET_ELE(3,1) ) + 

                  (GET_ELE(0,3) * GET_ELE(1,0) * GET_ELE(2,2) * GET_ELE(3,1) ) + 
                  (GET_ELE(0,3) * GET_ELE(1,1) * GET_ELE(2,0) * GET_ELE(3,2) ) + 
                  (GET_ELE(0,3) * GET_ELE(1,2) * GET_ELE(2,1) * GET_ELE(3,0) ) - 

                  (GET_ELE(0,0) * GET_ELE(1,1) * GET_ELE(2,3) * GET_ELE(3,2) ) - 
                  (GET_ELE(0,0) * GET_ELE(1,2) * GET_ELE(2,1) * GET_ELE(3,3) ) - 
                  (GET_ELE(0,0) * GET_ELE(1,3) * GET_ELE(2,2) * GET_ELE(3,1) ) - 

                  (GET_ELE(0,1) * GET_ELE(1,0) * GET_ELE(2,2) * GET_ELE(3,3) ) - 
                  (GET_ELE(0,1) * GET_ELE(1,2) * GET_ELE(2,3) * GET_ELE(3,0) ) - 
                  (GET_ELE(0,1) * GET_ELE(1,3) * GET_ELE(2,0) * GET_ELE(3,2) ) - 

                  (GET_ELE(0,2) * GET_ELE(1,0) * GET_ELE(2,3) * GET_ELE(3,1) ) - 
                  (GET_ELE(0,2) * GET_ELE(1,1) * GET_ELE(2,0) * GET_ELE(3,3) ) - 
                  (GET_ELE(0,2) * GET_ELE(1,3) * GET_ELE(2,1) * GET_ELE(3,0) ) - 

                  (GET_ELE(0,3) * GET_ELE(1,0) * GET_ELE(2,1) * GET_ELE(3,2) ) - 
                  (GET_ELE(0,3) * GET_ELE(1,1) * GET_ELE(2,2) * GET_ELE(3,0) ) -
                  (GET_ELE(0,3) * GET_ELE(1,2) * GET_ELE(2,0) * GET_ELE(3,1) );

    if( TbAbs(a) > TB_EPSILON )
    {

        // 計算
        GET_ELE_MTX(dst,0,0) = (GET_ELE(1,1)*GET_ELE(2,2)*GET_ELE(3,3)) + 
                       (GET_ELE(1,2)*GET_ELE(2,3)*GET_ELE(3,1)) +
                       (GET_ELE(1,3)*GET_ELE(2,1)*GET_ELE(3,2)) -
                       (GET_ELE(1,1)*GET_ELE(2,3)*GET_ELE(3,2)) -
                       (GET_ELE(1,2)*GET_ELE(2,1)*GET_ELE(3,3)) -
                       (GET_ELE(1,3)*GET_ELE(2,2)*GET_ELE(3,1));
        GET_ELE_MTX(dst,0,0) /= a;

        GET_ELE_MTX(dst,0,1) = (GET_ELE(0,1)*GET_ELE(2,3)*GET_ELE(3,2)) + 
                       (GET_ELE(0,2)*GET_ELE(2,1)*GET_ELE(3,3)) +
                       (GET_ELE(0,3)*GET_ELE(2,2)*GET_ELE(3,1)) -
                       (GET_ELE(0,1)*GET_ELE(2,2)*GET_ELE(3,3)) -
                       (GET_ELE(0,2)*GET_ELE(2,3)*GET_ELE(3,1)) -
                       (GET_ELE(0,3)*GET_ELE(2,1)*GET_ELE(3,2));
        GET_ELE_MTX(dst,0,1) /= a;

        GET_ELE_MTX(dst,0,2) = (GET_ELE(0,1)*GET_ELE(1,2)*GET_ELE(3,3)) + 
                       (GET_ELE(0,2)*GET_ELE(1,3)*GET_ELE(3,1)) +
                       (GET_ELE(0,3)*GET_ELE(1,1)*GET_ELE(3,2)) -
                       (GET_ELE(0,1)*GET_ELE(1,3)*GET_ELE(3,2)) -
                       (GET_ELE(0,2)*GET_ELE(1,1)*GET_ELE(3,3)) -
                       (GET_ELE(0,3)*GET_ELE(1,2)*GET_ELE(3,1));
        GET_ELE_MTX(dst,0,2) /= a;

        GET_ELE_MTX(dst,0,3) = (GET_ELE(0,1)*GET_ELE(1,3)*GET_ELE(2,2)) + 
                       (GET_ELE(0,2)*GET_ELE(1,1)*GET_ELE(2,3)) +
                       (GET_ELE(0,3)*GET_ELE(1,2)*GET_ELE(2,1)) -
                       (GET_ELE(0,1)*GET_ELE(1,2)*GET_ELE(2,3)) -
                       (GET_ELE(0,2)*GET_ELE(1,3)*GET_ELE(2,1)) -
                       (GET_ELE(0,3)*GET_ELE(1,1)*GET_ELE(2,2));
        GET_ELE_MTX(dst,0,3) /= a;

        GET_ELE_MTX(dst,1,0) = (GET_ELE(1,0)*GET_ELE(2,3)*GET_ELE(3,2)) + 
                       (GET_ELE(1,2)*GET_ELE(2,0)*GET_ELE(3,3)) +
                       (GET_ELE(1,3)*GET_ELE(2,2)*GET_ELE(3,0)) -
                       (GET_ELE(1,0)*GET_ELE(2,2)*GET_ELE(3,3)) -
                       (GET_ELE(1,2)*GET_ELE(2,3)*GET_ELE(3,0)) -
                       (GET_ELE(1,3)*GET_ELE(2,0)*GET_ELE(3,2));
        GET_ELE_MTX(dst,1,0) /= a;

        GET_ELE_MTX(dst,1,1) = (GET_ELE(0,0)*GET_ELE(2,2)*GET_ELE(3,3)) + 
                       (GET_ELE(0,2)*GET_ELE(2,3)*GET_ELE(3,0)) +
                       (GET_ELE(0,3)*GET_ELE(2,0)*GET_ELE(3,2)) -
                       (GET_ELE(0,0)*GET_ELE(2,3)*GET_ELE(3,2)) -
                       (GET_ELE(0,2)*GET_ELE(2,0)*GET_ELE(3,3)) -
                       (GET_ELE(0,3)*GET_ELE(2,2)*GET_ELE(3,0));
        GET_ELE_MTX(dst,1,1) /= a;

        GET_ELE_MTX(dst,1,2) = (GET_ELE(0,0)*GET_ELE(1,3)*GET_ELE(3,2)) + 
                       (GET_ELE(0,2)*GET_ELE(1,0)*GET_ELE(3,3)) +
                       (GET_ELE(0,3)*GET_ELE(1,2)*GET_ELE(3,0)) -
                       (GET_ELE(0,0)*GET_ELE(1,2)*GET_ELE(3,3)) -
                       (GET_ELE(0,2)*GET_ELE(1,3)*GET_ELE(3,0)) -
                       (GET_ELE(0,3)*GET_ELE(1,0)*GET_ELE(3,2));
        GET_ELE_MTX(dst,1,2) /= a;

        GET_ELE_MTX(dst,1,3) = (GET_ELE(0,0)*GET_ELE(1,2)*GET_ELE(2,3)) + 
                       (GET_ELE(0,2)*GET_ELE(1,3)*GET_ELE(2,0)) +
                       (GET_ELE(0,3)*GET_ELE(1,0)*GET_ELE(2,2)) -
                       (GET_ELE(0,0)*GET_ELE(1,3)*GET_ELE(2,2)) -
                       (GET_ELE(0,2)*GET_ELE(1,0)*GET_ELE(2,3)) -
                       (GET_ELE(0,3)*GET_ELE(1,2)*GET_ELE(2,0));
        GET_ELE_MTX(dst,1,3) /= a;

        GET_ELE_MTX(dst,2,0) = (GET_ELE(1,0)*GET_ELE(2,1)*GET_ELE(3,3)) + 
                       (GET_ELE(1,1)*GET_ELE(2,3)*GET_ELE(3,0)) +
                       (GET_ELE(1,3)*GET_ELE(2,0)*GET_ELE(3,1)) -
                       (GET_ELE(1,0)*GET_ELE(2,3)*GET_ELE(3,1)) -
                       (GET_ELE(1,1)*GET_ELE(2,0)*GET_ELE(3,3)) -
                       (GET_ELE(1,3)*GET_ELE(2,1)*GET_ELE(3,0));
        GET_ELE_MTX(dst,2,0) /= a;

        GET_ELE_MTX(dst,2,1) = (GET_ELE(0,0)*GET_ELE(2,3)*GET_ELE(3,1)) + 
                       (GET_ELE(0,1)*GET_ELE(2,0)*GET_ELE(3,3)) +
                       (GET_ELE(0,3)*GET_ELE(2,1)*GET_ELE(3,0)) -
                       (GET_ELE(0,0)*GET_ELE(2,1)*GET_ELE(3,3)) -
                       (GET_ELE(0,1)*GET_ELE(2,3)*GET_ELE(3,0)) -
                       (GET_ELE(0,3)*GET_ELE(2,0)*GET_ELE(3,1));
        GET_ELE_MTX(dst,2,1) /= a;

        GET_ELE_MTX(dst,2,2) = (GET_ELE(0,0)*GET_ELE(1,1)*GET_ELE(3,3)) + 
                       (GET_ELE(0,1)*GET_ELE(1,3)*GET_ELE(3,0)) +
                       (GET_ELE(0,3)*GET_ELE(1,0)*GET_ELE(3,1)) -
                       (GET_ELE(0,0)*GET_ELE(1,3)*GET_ELE(3,1)) -
                       (GET_ELE(0,1)*GET_ELE(1,0)*GET_ELE(3,3)) -
                       (GET_ELE(0,3)*GET_ELE(1,1)*GET_ELE(3,0));
        GET_ELE_MTX(dst,2,2) /= a;

        GET_ELE_MTX(dst,2,3) = (GET_ELE(0,0)*GET_ELE(1,3)*GET_ELE(2,1)) + 
                       (GET_ELE(0,1)*GET_ELE(1,0)*GET_ELE(2,3)) +
                       (GET_ELE(0,3)*GET_ELE(1,1)*GET_ELE(2,0)) -
                       (GET_ELE(0,0)*GET_ELE(1,1)*GET_ELE(2,3)) -
                       (GET_ELE(0,1)*GET_ELE(1,3)*GET_ELE(2,0)) -
                       (GET_ELE(0,3)*GET_ELE(1,0)*GET_ELE(2,1));
        GET_ELE_MTX(dst,2,3) /= a;

        GET_ELE_MTX(dst,3,0) = (GET_ELE(1,0)*GET_ELE(2,2)*GET_ELE(3,1)) + 
                       (GET_ELE(1,1)*GET_ELE(2,0)*GET_ELE(3,2)) +
                       (GET_ELE(1,2)*GET_ELE(2,1)*GET_ELE(3,0)) -
                       (GET_ELE(1,0)*GET_ELE(2,1)*GET_ELE(3,2)) -
                       (GET_ELE(1,1)*GET_ELE(2,2)*GET_ELE(3,0)) -
                       (GET_ELE(1,2)*GET_ELE(2,0)*GET_ELE(3,1));
        GET_ELE_MTX(dst,3,0) /= a;

        GET_ELE_MTX(dst,3,1) = (GET_ELE(0,0)*GET_ELE(2,1)*GET_ELE(3,2)) + 
                       (GET_ELE(0,1)*GET_ELE(2,2)*GET_ELE(3,0)) +
                       (GET_ELE(0,2)*GET_ELE(2,0)*GET_ELE(3,1)) -
                       (GET_ELE(0,0)*GET_ELE(2,2)*GET_ELE(3,1)) -
                       (GET_ELE(0,1)*GET_ELE(2,0)*GET_ELE(3,2)) -
                       (GET_ELE(0,2)*GET_ELE(2,1)*GET_ELE(3,0));
        GET_ELE_MTX(dst,3,1) /= a;

        GET_ELE_MTX(dst,3,2) = (GET_ELE(0,0)*GET_ELE(1,2)*GET_ELE(3,1)) + 
                       (GET_ELE(0,1)*GET_ELE(1,0)*GET_ELE(3,2)) +
                       (GET_ELE(0,2)*GET_ELE(1,1)*GET_ELE(3,0)) -
                       (GET_ELE(0,0)*GET_ELE(1,1)*GET_ELE(3,2)) -
                       (GET_ELE(0,1)*GET_ELE(1,2)*GET_ELE(3,0)) -
                       (GET_ELE(0,2)*GET_ELE(1,0)*GET_ELE(3,1));
        GET_ELE_MTX(dst,3,2) /= a;

        GET_ELE_MTX(dst,3,3) = (GET_ELE(0,0)*GET_ELE(1,1)*GET_ELE(2,2)) + 
                       (GET_ELE(0,1)*GET_ELE(1,2)*GET_ELE(2,0)) +
                       (GET_ELE(0,2)*GET_ELE(1,0)*GET_ELE(2,1)) -
                       (GET_ELE(0,0)*GET_ELE(1,2)*GET_ELE(2,1)) -
                       (GET_ELE(0,1)*GET_ELE(1,0)*GET_ELE(2,2)) -
                       (GET_ELE(0,2)*GET_ELE(1,1)*GET_ELE(2,0));
        GET_ELE_MTX(dst,3,3) /= a;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 転置
 * @author Miyake Shunsuke
 * @since 2011.11.11
 */
void TbMatrix44::Transpose()
{
    Set( GET_ELE(0,0) , GET_ELE(1,0) , GET_ELE(2,0) , GET_ELE(3,0) , 
         GET_ELE(0,1) , GET_ELE(1,1) , GET_ELE(2,1) , GET_ELE(3,1) ,
         GET_ELE(0,2) , GET_ELE(1,2) , GET_ELE(2,2) , GET_ELE(3,2) ,
         GET_ELE(0,3) , GET_ELE(1,3) , GET_ELE(2,3) , GET_ELE(3,3) );
}

/*!
 * 転置結果を取得
 * @author Miyake_Shunsuke
 * @since 2014.04.02
 */
void TbMatrix44::Transpose(TbMatrix44& dst) const
{
    dst.Set( GET_ELE(0,0) , GET_ELE(1,0) , GET_ELE(2,0) , GET_ELE(3,0) , 
             GET_ELE(0,1) , GET_ELE(1,1) , GET_ELE(2,1) , GET_ELE(3,1) ,
             GET_ELE(0,2) , GET_ELE(1,2) , GET_ELE(2,2) , GET_ELE(3,2) ,
             GET_ELE(0,3) , GET_ELE(1,3) , GET_ELE(2,3) , GET_ELE(3,3) );
}

/*!
 * 移動量直接設定
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMatrix44::SetTranslation( TbFloat32 x,TbFloat32 y,TbFloat32 z )
{
    GET_ELE(3,0) = x;
    GET_ELE(3,1) = y;
    GET_ELE(3,2) = z;
    GET_ELE(3,3) = 1.0f;
}

/*!
 * 移動量取得
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
void TbMatrix44::GetTranslation( TbVector3& dst ) const
{
    dst.Set( GET_ELE(3,0) , 
             GET_ELE(3,1) ,
             GET_ELE(3,2) );
}

/*!
 * 移動量取得
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
void TbMatrix44::GetTranslation( TbVector4& dst ) const
{
    dst.Set( GET_ELE(3,0) , 
             GET_ELE(3,1) ,
             GET_ELE(3,2) ,
             1.0f );
}

/*!
 * 平行移動
 */
void TbMatrix44::Translate(TbFloat32 x,TbFloat32 y,TbFloat32 z)
{
    TbMatrix44 mtx( 1.0f , 0.0f , 0.0f , 0.0f,
                    0.0f , 1.0f , 0.0f , 0.0f,
                    0.0f , 0.0f , 1.0f , 0.0f,
                    x    , y    , z    , 1.0f );
    (*this) *= mtx;

}

/*!
 * X軸回転
 */
void TbMatrix44::RotateX(const TbAngle32 angle)
{
    TbFloat32 c = TbCos(TbAng32ToRad(angle));
    TbFloat32 s = TbSin(TbAng32ToRad(angle));
    TbMatrix44 mtx( 1.0f , 0.0f , 0.0f , 0.0f,
                    0.0f , c , s , 0.0f,
                    0.0f , -s , c , 0.0f,
                    0.0f , 0.0f , 0.0f , 1.0f );
    (*this) *= mtx;
}

/*!
 * Y軸回転
 */
void TbMatrix44::RotateY(const TbAngle32 angle)
{
    TbFloat32 rad = TbAng32ToRad(angle);
    TbFloat32 c = TbCos(rad);
    TbFloat32 s = TbSin(rad);
    TbMatrix44 mtx( c , 0.0f , -s , 0.0f,
                    0.0f , 1.0f , 0.0f , 0.0f,
                    s , 0.0f , c , 0.0f,
                    0.0f , 0.0f , 0.0f , 1.0f );
    (*this) *= mtx;
}

/*!
 * Z軸回転
 */
void TbMatrix44::RotateZ(const TbAngle32 angle)
{
    TbFloat32 c = TbCos(TbAng32ToRad(angle));
    TbFloat32 s = TbSin(TbAng32ToRad(angle));
    TbMatrix44 mtx( c , s , 0.0f , 0.0f,
                    -s , c , 0.0f , 0.0f,
                    0.0f , 0.0f , 1.0f , 0.0f,
                    0.0f , 0.0f , 0.0f , 1.0f );
    (*this) *= mtx;
}

/*!
 * 任意軸回転
 */
void TbMatrix44::RotateAxis(const TbFloat32 vx,const TbFloat32 vy,const TbFloat32 vz,const TbAngle32 angle)
{
    TbFloat32 c = TbCos(TbAng32ToRad(angle));
    TbFloat32 s = TbSin(TbAng32ToRad(angle));
    TbMatrix44 mtx( c+vx*vx*(1.0f-c) , vx*vy*(1.0f-c)+vz*s , vx*vz*(1.0f-c)-vy*s , 0.0f,
                    vx*vy*(1.0f-c)-vz*s , c+vy*vy*(1.0f-c) , vy*vz*(1.0f-c)+vz*s , 0.0f,
                    vx*vz*(1.0f-c)+vy*s , vy*vz*(1-c)-vx*s , c+vz*vz*(1.0f-c) , 0.0f,
                    0.0f , 0.0f , 0.0f , 1.0f );
    (*this) *= mtx;
}

/*!
 * 回転成分取得
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
TbMatrix44 TbMatrix44::GetRotation() const
{
    TbMatrix44 mtx( GET_ELE(0,0) , GET_ELE(0,1) , GET_ELE(0,2) , 0.0f,
                    GET_ELE(1,0) , GET_ELE(1,1) , GET_ELE(1,2) , 0.0f,
                    GET_ELE(2,0) , GET_ELE(2,1) , GET_ELE(2,2) , 0.0f,
                    0.0f , 0.0f , 0.0f , 1.0f );
    return mtx;
}

/*!
 * 拡大
 * @param x x座標
 * @param y y座標
 * @param z z座標
 * @author Miyake Shunsuke
 */
void TbMatrix44::Scale(TbFloat32 x,TbFloat32 y,TbFloat32 z)
{
    TbMatrix44 mtx( x    , 0.0f , 0.0f , 0.0f,
                    0.0f , y    , 0.0f , 0.0f,
                    0.0f , 0.0f , z    , 0.0f,
                    0.0f , 0.0f , 0.0f , 1.0f );
    (*this) *= mtx;
}

/*!
 * X軸ベクトル取得
 * @author Miyake Shunsuke
 */
TbVector4 TbMatrix44::GetAxisX() const
{
    return TbVector4( GET_ELE(0,0) , GET_ELE(0,1) , GET_ELE(0,2) , 0.0f );
}

/*!
 * Y軸ベクトル取得
 * @author Miyake Shunsuke
 */
TbVector4 TbMatrix44::GetAxisY() const
{
    return TbVector4( GET_ELE(1,0) , GET_ELE(1,1) , GET_ELE(1,2) , 0.0f );
}

/*!
 * Z軸ベクトル取得
 * @author Miyake Shunsuke
 */
TbVector4 TbMatrix44::GetAxisZ() const
{
    return TbVector4( GET_ELE(2,0) , GET_ELE(2,1) , GET_ELE(2,2) , 0.0f );
}


/*!
 * オイラー角に変換
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
TbVector3 TbMatrix44::GetEulerRadXYZ() const
{
    TbVector3 result;
    TbFloat32 x = 0.0f;
    TbFloat32 y = 0.0f;
    TbFloat32 z = 0.0f;
    z = TbArcTan2( GET_ELE(0,1) , GET_ELE(0,0) );
    y = TbArcSin( -GET_ELE(0,2) );
    x = TbArcSin( GET_ELE(1,2)/TbCos(y) );
    if( GET_ELE(2,2) < 0.0f ) {
        x = TB_PI - x;
    }
    result.Set(x,y,z);
    return result;
}

/*!
 * オイラー角に変換
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
TbVector3 TbMatrix44::GetEulerAngXYZ() const
{
    TbVector3 result = GetEulerRadXYZ();
    result.Set(TbRadToAng32(result.GetX()),
               TbRadToAng32(result.GetY()),
               TbRadToAng32(result.GetZ()));
    return result;
}

/*!
 * オイラー角に変換
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
TbVector3 TbMatrix44::GetEulerDegXYZ() const
{
    TbVector3 result = GetEulerRadXYZ();
    result.Set(TbRadToDeg(result.GetX()),
               TbRadToDeg(result.GetY()),
               TbRadToDeg(result.GetZ()));
    return result;
}

////////////////////////////
//  TbMatrix43
////////////////////////////

/*!
 * コンストラクタ
 */
TbMatrix43::TbMatrix43(void)
{
    Unit();
}

/*!
 * コンストラクタ
 * @param mtx コピー元
 */
TbMatrix43::TbMatrix43(const TbMatrix43& mtx)
{
    TbMemCopy( this , &mtx , sizeof( *this ) );
}

/*!
 * コンストラクタ
 */
TbMatrix43::TbMatrix43(  TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,
                         TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,
                         TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,
                         TbFloat32 m30,TbFloat32 m31,TbFloat32 m32)
{
    Set(m00,m01,m02,
        m10,m11,m12,
        m20,m21,m22,
        m30,m31,m32);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix43::~TbMatrix43(void)
{
}

/*! 
 * 代入演算子
 * @author Miyake Shunsuke
 * @since 2010.12.02
 */
TbMatrix43& TbMatrix43::operator=(const TbMatrix43& mtx)
{
    TbMemCopy( this , &mtx , sizeof( mtx ) );
    return *this;
}

/*! 
 * 代入演算子
 * @author Miyake Shunsuke
 * @since 2014.4.02
 */
TbMatrix43& TbMatrix43::operator=(const TbMatrix44& mtx)
{
    Set(GET_ELE_MTX(mtx,0,0),GET_ELE_MTX(mtx,0,1),GET_ELE_MTX(mtx,0,2),
        GET_ELE_MTX(mtx,1,0),GET_ELE_MTX(mtx,1,1),GET_ELE_MTX(mtx,1,2),
        GET_ELE_MTX(mtx,2,0),GET_ELE_MTX(mtx,2,1),GET_ELE_MTX(mtx,2,2),
        GET_ELE_MTX(mtx,3,0),GET_ELE_MTX(mtx,3,1),GET_ELE_MTX(mtx,3,2));
    return *this;
}

/*! 
 * ベクトル演算
 * @author Miyake Shunsuke
 * @since 2011.04.05
 */
TbVector3    TbMatrix43::operator*(const TbVector4& vector ) const
{
    TbVector3 result;
    result.SetX( vector.GetX() * GET_ELE43(0,0) + vector.GetY() * GET_ELE43(1,0) + vector.GetZ() * GET_ELE43(2,0) + vector.GetW() * GET_ELE43(3,0) );
    result.SetY( vector.GetX() * GET_ELE43(0,1) + vector.GetY() * GET_ELE43(1,1) + vector.GetZ() * GET_ELE43(2,1) + vector.GetW() * GET_ELE43(3,1) );
    result.SetZ( vector.GetX() * GET_ELE43(0,2) + vector.GetY() * GET_ELE43(1,2) + vector.GetZ() * GET_ELE43(2,2) + vector.GetW() * GET_ELE43(3,2) );
    return result;
}

/*! 
 * ベクトル演算
 * @author Miyake Shunsuke
 * @since 2011.04.05
 */
TbVector3    TbMatrix43::operator*(const TbVector3& vector ) const
{
    TbVector3 result;
    result.SetX( vector.GetX() * GET_ELE43(0,0) + vector.GetY() * GET_ELE43(1,0) + vector.GetZ() * GET_ELE43(2,0) + 1.0f * GET_ELE43(3,0) );
    result.SetY( vector.GetX() * GET_ELE43(0,1) + vector.GetY() * GET_ELE43(1,1) + vector.GetZ() * GET_ELE43(2,1) + 1.0f * GET_ELE43(3,1) );
    result.SetZ( vector.GetX() * GET_ELE43(0,2) + vector.GetY() * GET_ELE43(1,2) + vector.GetZ() * GET_ELE43(2,2) + 1.0f * GET_ELE43(3,2) );
    return result;
}


/*!
 * 設定
 */
void TbMatrix43::Set(TbFloat32 m00,TbFloat32 m01,TbFloat32 m02,
                     TbFloat32 m10,TbFloat32 m11,TbFloat32 m12,
                     TbFloat32 m20,TbFloat32 m21,TbFloat32 m22,
                     TbFloat32 m30,TbFloat32 m31,TbFloat32 m32)
{
    GET_ELE43(0,0) = m00;
    GET_ELE43(0,1) = m01;
    GET_ELE43(0,2) = m02;
    GET_ELE43(1,0) = m10;
    GET_ELE43(1,1) = m11;
    GET_ELE43(1,2) = m12;
    GET_ELE43(2,0) = m20;
    GET_ELE43(2,1) = m21;
    GET_ELE43(2,2) = m22;
    GET_ELE43(3,0) = m30;
    GET_ELE43(3,1) = m31;
    GET_ELE43(3,2) = m32;
}

/*!
 * 単位行列
 */
void TbMatrix43::Unit()
{
    Set( 1.0f , 0.0f , 0.0f , 
         0.0f , 1.0f , 0.0f , 
         0.0f , 0.0f , 1.0f , 
         0.0f , 0.0f , 0.0f );
}

/*!
 * 転置
 * @author Miyake Shunsuke
 * @since 2011.11.11
 */
void TbMatrix43::Transpose()
{
    Set( GET_ELE43(0,0) , GET_ELE43(1,0) , GET_ELE43(2,0) , GET_ELE43(3,0) ,
         GET_ELE43(0,1) , GET_ELE43(1,1) , GET_ELE43(2,1) , GET_ELE43(3,1) ,
         GET_ELE43(0,2) , GET_ELE43(1,2) , GET_ELE43(2,2) , GET_ELE43(3,2) );
}

/*!
 * 転置結果を取得
 * @author Miyake_Shunsuke
 * @since 2014.04.02
 */
void TbMatrix43::Transpose(TbMatrix43& dst) const
{
    dst.Set( GET_ELE43(0,0) , GET_ELE43(1,0) , GET_ELE43(2,0) , GET_ELE43(3,0) ,
             GET_ELE43(0,1) , GET_ELE43(1,1) , GET_ELE43(2,1) , GET_ELE43(3,1) ,
             GET_ELE43(0,2) , GET_ELE43(1,2) , GET_ELE43(2,2) , GET_ELE43(3,2) );
}

}
