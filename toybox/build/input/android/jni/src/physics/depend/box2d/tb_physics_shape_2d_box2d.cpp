/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 */

#include <tb_fwd.h>
#include <physics/tb_physics_body_2d.h>
#include <physics/tb_physics_shape_2d.h>
#include <physics/tb_physics_scene_2d.h>

#if TB_PHYSICSAPI_IS_BOX2D

namespace toybox
{

///////////////////////////////////////////
//  TbPhysicsShapeCircle2d
///////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsShapeCircle2d::TbPhysicsShapeCircle2d()
{
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsShapeCircle2d::~TbPhysicsShapeCircle2d()
{
}

/*! 
 * 半径設定
 * @author teco
 */
void TbPhysicsShapeCircle2d::Set( TbFloat32 radius )
{
    m_Depend.circle.m_radius = radius;
}

/*! 
 * 適用
 * @author teco
 */
void TbPhysicsShapeCircle2d::Apply( TbPhysicsBody2d* actor )
{
    TB_RESQUE(actor);
    b2Body* body = actor->GetDepend().GetBody();
    TB_RESQUE(body);
    b2FixtureDef def;
    def.density = GetDensity();
    def.friction = GetFriction();
    def.restitution = GetRestitution();
    def.isSensor = IsSensor();
    b2CircleShape shape = m_Depend.circle;
    shape.m_radius *= TB_PHYSICS_BOX2D_SCALE;
    def.shape = &shape;
    body->CreateFixture(&def);

}

///////////////////////////////////////////
//  TbPhysicsShapePolygon2d
///////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsShapePolygon2d::TbPhysicsShapePolygon2d()
{
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsShapePolygon2d::~TbPhysicsShapePolygon2d()
{
}

/*! 
 * 頂点設定
 * @author teco
 */
void TbPhysicsShapePolygon2d::Set( const TbVector2* vertex , TbSint32 vertexCount )
{
    b2Vec2 dst[8];
    TB_RESQUE(0 <= vertexCount && vertexCount<TB_ARRAY_COUNT_OF(dst));
    for( TbSint32 i = 0; i < vertexCount; ++i ) {
        dst[i].Set( vertex[i].GetX()*TB_PHYSICS_BOX2D_SCALE , vertex[i].GetY()*TB_PHYSICS_BOX2D_SCALE );
    }
}

/*! 
 * 矩形設定
 * @author teco
 */
void TbPhysicsShapePolygon2d::SetRect( const TbRectF32& rect )
{
    b2Vec2 vertex[4];
    vertex[0].Set( rect.x            , -(rect.y+rect.height) );
    vertex[1].Set( rect.x+rect.width , -(rect.y+rect.height) );
    vertex[2].Set( rect.x+rect.width , -(rect.y) );
    vertex[3].Set( rect.x , -(rect.y) );
    m_Depend.polygon.Set( vertex , TB_ARRAY_COUNT_OF(vertex) );
}

/*! 
 * 適用
 * @author teco
 */
void TbPhysicsShapePolygon2d::Apply( TbPhysicsBody2d* actor )
{
    TB_RESQUE(actor);
    b2Body* body = actor->GetDepend().GetBody();
    TB_RESQUE(body);
    b2FixtureDef def;
    def.density = GetDensity();
    def.friction = GetFriction();
    def.isSensor = IsSensor();
    def.restitution = GetRestitution();
    b2PolygonShape shape = m_Depend.polygon;
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(shape.m_vertices); ++i ) {
        shape.m_vertices[i] *= TB_PHYSICS_BOX2D_SCALE;
    }
    def.shape = &shape;
    body->CreateFixture(&def);
}

///////////////////////////////////////////
//  TbPhysicsShapeEdge2d
///////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsShapeEdge2d::TbPhysicsShapeEdge2d()
    : m_VertexCount(0)
    , m_Depend()
{
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsShapeEdge2d::~TbPhysicsShapeEdge2d()
{
}

/*! 
 * 適用
 * @author teco
 */
void TbPhysicsShapeEdge2d::Apply( TbPhysicsBody2d* actor )
{
    TB_RESQUE( 2 <= m_VertexCount );
    
    b2Body* body = actor->GetDepend().GetBody();
    TB_RESQUE(body);
    b2FixtureDef def;
    def.density = GetDensity();
    def.isSensor = IsSensor();
    def.friction = GetFriction();
    def.restitution = GetRestitution();

    b2Vec2 v1;
    b2Vec2 v2;
    for( TbSint32 i = 0; i < m_VertexCount-1; ++ i )
    {
        b2EdgeShape shape;
        if( i != 0 ) {
            shape.m_hasVertex0 = true;
            shape.m_vertex0 = v2;
        }
        if( i < m_VertexCount-2 ) {
            shape.m_hasVertex3 = true;
            shape.m_vertex3.Set( m_Vertexs[i+2].GetX() * TB_PHYSICS_BOX2D_SCALE ,
                                 m_Vertexs[i+2].GetY() * TB_PHYSICS_BOX2D_SCALE );
        }
        v1.Set( m_Vertexs[i].GetX()*TB_PHYSICS_BOX2D_SCALE , m_Vertexs[i].GetY()*TB_PHYSICS_BOX2D_SCALE );
        v2.Set( m_Vertexs[i+1].GetX()*TB_PHYSICS_BOX2D_SCALE , m_Vertexs[i+1].GetY()*TB_PHYSICS_BOX2D_SCALE );
        shape.Set( v1 , v2 );

        def.shape = &shape;
        body->CreateFixture(&def);
   }
}

}

#endif
