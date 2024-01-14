/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 * @since 2011.05.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_SHAPE_2D_H_
#define _INCLUDED_TB_PHYSICS_SHAPE_2D_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>

#include "./depend/tb_physics_shape_2d_depend.h"

namespace toybox
{

class TbPhysicsBody2d;

// 基底
class TbPhysicsShapeBase2d
{
public:
    // コンストラクタ
    TbPhysicsShapeBase2d() 
        : m_Density(0.0f) 
        , m_Friction(0.0f) 
        , m_Restitution(0.0f)
        , m_IsSensor(TB_FALSE)
    {}

    // デストラクタ
    virtual ~TbPhysicsShapeBase2d() {}
public:
    // 適用
    virtual void Apply( TbPhysicsBody2d* actor ) = 0;
public:
    // 密度
    void SetDensity( TbFloat32 density ) {
        m_Density = density;
    }
    // 密度取得
    TbFloat32 GetDensity() const {
        return m_Density;
    }
    // 摩擦
    void SetFriction( TbFloat32 friction ) {
        m_Friction = friction;
    }
    // 摩擦取得
    TbFloat32 GetFriction() const {
        return m_Friction;
    }
    // 反発係数
    void SetRestitution( TbFloat32 restitution ) {
        m_Restitution = restitution;
    }
    // 反発係数取得
    TbFloat32 GetRestitution() const {
        return m_Restitution;
    }
    // センサー設定(ぶつからないけど検知はする)
    void SetSensor( TbBool isSensor ) {
        m_IsSensor = isSensor;
    }
    // センサー設定
    TbBool IsSensor() const {
        return m_IsSensor;
    }
private:
    TbFloat32 m_Density;        // 密度 (サイズの大きさとこの値で重さが決まる)
    TbFloat32 m_Friction;       // 摩擦
    TbFloat32 m_Restitution;    // 反発係数
    TbBool    m_IsSensor;
};

// 円形
class TbPhysicsShapeCircle2d : public TbPhysicsShapeBase2d
{
public:
    TbPhysicsShapeCircle2d();
    virtual ~TbPhysicsShapeCircle2d();
public:
    // 半径設定
    void Set( TbFloat32 radius );
public:
    // 適用
    virtual void Apply( TbPhysicsBody2d* actor );
private:
    TbPhysicsShapeCircle2dDepend m_Depend;
};

// 任意頂点数のポリゴン
class TbPhysicsShapePolygon2d : public TbPhysicsShapeBase2d
{
public:
    TbPhysicsShapePolygon2d();
    virtual ~TbPhysicsShapePolygon2d();
public:
    // 頂点設定
    void Set( const TbVector2* vertex , TbSint32 vertexCount );
    // 矩形設定
    void SetRect( const TbRectF32& rect );
    // 矩形設定
    void SetRect( TbFloat32 x , TbFloat32 y , TbFloat32 w , TbFloat32 h ) {
        SetRect( TbRectF32( x , y , w , h ) );
    }
public:
    // 適用
    virtual void Apply( TbPhysicsBody2d* actor );
private:
    TbPhysicsShapePolygon2dDepend   m_Depend;
};

// 任意頂点数のEdge
class TbPhysicsShapeEdge2d : public TbPhysicsShapeBase2d
{
public:
    TbPhysicsShapeEdge2d();
    virtual ~TbPhysicsShapeEdge2d();
public:
    // 頂点設定
    void Set( const TbVector2* vertex , TbSint32 vertexCount ) {
        TB_RESQUE(vertexCount<TB_ARRAY_COUNT_OF(m_Vertexs));
        m_VertexCount = vertexCount;
        for( TbSint32 i = 0; i < m_VertexCount; ++i ){
            m_Vertexs[i] = vertex[i];
        }
    }
    // 矩形設定
    void SetRect( const TbRectF32& rect ) {
        m_VertexCount = 5;
        m_Vertexs[0].Set( rect.x              , rect.y );
        m_Vertexs[1].Set( rect.x              , rect.y + rect.height );
        m_Vertexs[2].Set( rect.x + rect.width , rect.y + rect.height );
        m_Vertexs[3].Set( rect.x + rect.width , rect.y );
        m_Vertexs[4].Set( rect.x              , rect.y );
    }
    // 矩形設定
    void SetRect( TbFloat32 x , TbFloat32 y , TbFloat32 w , TbFloat32 h ) {
        SetRect( TbRectF32( x , y , w , h ) );
    }
public:
    // 適用
    virtual void Apply( TbPhysicsBody2d* actor );
private:
    static const TbSint32 VERTEX_MAX = 8;
private:
    TbSint32                    m_VertexCount;
    TbVector2                   m_Vertexs[VERTEX_MAX];
    TbPhysicsShapeEdge2dDepend  m_Depend;
};

}

#endif
