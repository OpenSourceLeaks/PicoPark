/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 * @since 2015.03.06
 */
 
#include <tb_fwd.h>
#include <physics/tb_physics_scene_2d.h>

#if TB_PHYSICSAPI_IS_BOX2D

namespace toybox
{

///////////////////////////////////////
// TbPhysicsScene2dDepend
///////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsScene2dDepend::TbPhysicsScene2dDepend()
    : m_World(b2Vec2(0.0f,-9.8f))
{
    b2BodyDef def;
    m_BackGround = m_World.CreateBody(&def);
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsScene2dDepend::~TbPhysicsScene2dDepend()
{
    TbSint32 bodyCount = m_World.GetBodyCount();
    TB_ASSERT(1==bodyCount);
    TbSint32 jointCount = m_World.GetJointCount();
    TB_ASSERT(0==jointCount);
    
    m_World.DestroyBody(m_BackGround);
    m_BackGround = nullptr;
}

///////////////////////////////////////
// TbPhysicsScene2d
///////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsScene2d::TbPhysicsScene2d()
{
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsScene2d::~TbPhysicsScene2d()
{
}

/*! 
 * 重力設定
 * @author teco
 */
void TbPhysicsScene2d::SetGravity( const TbVector2& gravity )
{
    b2Vec2 val = b2Vec2(gravity.GetX(),gravity.GetY()*TB_PHYSICS_BOX2D_SCALE);
    m_Depend.m_World.SetGravity( val );
}

/*! 
 * 時間進行
 * @author teco
 */
void TbPhysicsScene2d::Step( float deltaSec )
{
    TbSint32 velocityIterations = 10;
    TbSint32 positionIterations = 10;
    m_Depend.m_World.Step(deltaSec,velocityIterations,positionIterations);
}

/*! 
 * レイを飛ばす
 * @author teco
 */
TbBool TbPhysicsScene2d::RayCast( const TbPhysicsRay2d& ray , TbPhysicsRayCastResult* result )
{
    b2RayCastInput input;
    input.p1.Set( ray.p1.GetX()*TB_PHYSICS_BOX2D_SCALE , ray.p1.GetY()*TB_PHYSICS_BOX2D_SCALE );
    input.p2.Set( ray.p2.GetX()*TB_PHYSICS_BOX2D_SCALE , ray.p2.GetY()*TB_PHYSICS_BOX2D_SCALE );
    input.maxFraction = 1;

    TbFloat32 closestFraction = 2.0f;
    TbBool ret = TB_FALSE;

    for (b2Body* b = m_Depend.m_World.GetBodyList(); b; b = b->GetNext()) {
        for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {  
            b2RayCastOutput output;
            if ( ! f->RayCast( &output , input , 0 ) ) {
                continue;
            }
            if ( output.fraction < closestFraction ) {
                if( result ) {
                    result->pos = ray.p1 + (ray.p2 - ray.p1) * output.fraction;
                    result->normal.Set( output.normal.x , output.normal.y );
                    result->body = reinterpret_cast<TbPhysicsBody2d*>(b->GetUserData());
                }
                closestFraction = output.fraction;
                ret = TB_TRUE;
            }
        }
    }

    return ret;
}

}

#endif
