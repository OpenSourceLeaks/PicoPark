/*!
 * 3次元多角形
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_POLYGON_H_
#define _INCLUDED_POLYGON_H_

#include <base/math/tb_vector4.h>
#include <base/math/tb_matrix.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

template <TbUint32 COUNT>
class TbPolygon
{
public:
    
    // コンストラクタ
    TbPolygon() {
        for( TbUint32 i = 0; i < COUNT; ++i ){
            m_Pos[i].SetW(1.0f);
        }
    }
    
    // コピーコンストラクタ
    TbPolygon( const TbPolygon& polygon ) {
        for( TbUint32 i = 0; i < COUNT; ++i ){
            m_Pos[i] = polygon.m_Pos[i];
        }
        m_Base = polygon.m_Base;
    }
    
    // デストラクタ
    ~TbPolygon() {}

public:

    TbPolygon<COUNT>& operator+=( const TbVector4& vec ) {
        for( TbUint32 i = 0; i < COUNT; ++i ){
            m_Pos[i] += vec;
        }
        return *this;
    }
    
public:
    
    // 座標設定
    void SetPos( TbUint32 index , const TbFloat32 x , TbFloat32 y ) {
        if( index < COUNT ){
            m_Pos[index].SetXY( x , y );
        }
    }

    // 座標設定
    void SetPos( TbUint32 index , const TbFloat32 x , TbFloat32 y , TbFloat32 z ) {
        if( index < COUNT ){
            m_Pos[index].Set( x , y );
        }
    }

    // 座標設定
    void SetPos( TbUint32 index , const TbVector4& pos ) {
        if( index < COUNT ){
            m_Pos[index] = pos;
        }
    }
    
    // 座標取得
    const TbVector4& GetPos( TbUint32 index ) {
        if( index < COUNT ){
            return m_Pos;
        }
        return TbVector4::IDENTITY;
    }
    
    // 各頂点のUV座標取得
    const toybox::TbVector4 (&GetPosArray() const )[COUNT] {
        return m_Pos;
    }

    // 座標設定
    void SetBase( const TbVector4& base ) {
        m_Base = pos;
    }
    
    // XY平面で全頂点が格納可能な矩形取得
    TbRectF32 GetRectXY() const {
        TbRectF32 result;
        TbFloat32 left = m_Pos[0].GetX();
        TbFloat32 right = m_Pos[0].GetX();
        TbFloat32 top = m_Pos[0].GetY();
        TbFloat32 bottom = m_Pos[0].GetY();
        for( TbSizeT i = 1; i < COUNT; ++i ) {
            left = TbMin<TbFloat32>(left,m_Pos[i].GetX());
            right = TbMax<TbFloat32>(right,m_Pos[i].GetX());
            top = TbMax<TbFloat32>(top,m_Pos[i].GetY());
            bottom = TbMin<TbFloat32>(bottom,m_Pos[i].GetY());
        }
        return TbRectF32( left , bottom , right - left , top - bottom );
    }

private:

    TbVector4 m_Pos[COUNT]; // 各頂点
    TbVector4 m_Base;       // 基準点

public:
    
    // 多角形と行列との乗算演算子をフレンド関数とする
    template <TbUint32 COUNT>
    friend TbPolygon<COUNT> operator*(const TbMatrix& mtx , const TbPolygon<COUNT>& polygon );
    
};

// 三角形
typedef TbPolygon<3> TbTriPolygon;

// 四角形
typedef TbPolygon<4> TbQuadPolygon;

// 多角形と行列との乗算演算子
template <TbUint32 COUNT>
inline TbPolygon<COUNT> operator*(const TbMatrix& mtx , const TbPolygon<COUNT>& polygon )
{
    TbPolygon<COUNT> result( polygon );
    for( TbUint32 i = 0; i < COUNT; ++i ) {
        result.m_Pos[i] -= result.m_Base;
        result.m_Pos[i] = mtx * result.m_Pos[i];
        result.m_Pos[i] += result.m_Base;
    }
    return result;
}

}

#endif
