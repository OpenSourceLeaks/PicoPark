/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 */

#include <tb_fwd.h>
#include <physics/tb_physics_body_2d.h>
#include <physics/tb_physics_scene_2d.h>

#if TB_PHYSICSAPI_IS_BOX2D

namespace toybox
{

////////////////////////////////////////////
//   TbPhysicsBody2dDepend
////////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsBody2dDepend::TbPhysicsBody2dDepend()
    : m_Body(nullptr)
{
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsBody2dDepend::~TbPhysicsBody2dDepend()
{
}

////////////////////////////////////////////
//   TbPhysicsBody2d
////////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsBody2d::TbPhysicsBody2d()
    : m_Depend()
    , m_Type(TYPE_STATIC)
    , m_Scene(nullptr)
    , m_Shape(nullptr)
    , m_InitAngle(0.0f)
    , m_InitVeloAngle(0.0f)
    , m_DampingAngle(0.0f)
    , m_GravityScale(1.0f)
    , m_Layer(0)
    , m_UserData(nullptr)
{
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsBody2d::~TbPhysicsBody2d()
{
    Exit();
}

/*! 
 * シーンに入る
 * @author teco
 */
void TbPhysicsBody2d::Enter( TbPhysicsScene2d* scene )
{
    TB_RESQUE(scene && m_Shape);
    if( !m_Scene ) {
        b2World& world = scene->GetDepend().GetWorld();
        b2BodyDef def;
        switch( m_Type )
        {
        case TYPE_STATIC:
            def.type = b2_staticBody;
            break;
        case TYPE_DYNAMIC:
            def.type = b2_dynamicBody;
            break;
        };
        def.position.Set( m_InitPos.GetX()*TB_PHYSICS_BOX2D_SCALE , m_InitPos.GetY()*TB_PHYSICS_BOX2D_SCALE );
        def.angle = m_InitAngle;
        def.angularDamping = m_DampingAngle;
        m_Depend.m_Body = world.CreateBody(&def);
        if( m_Depend.m_Body ){
            m_Depend.m_Body->SetUserData(this);
            b2Vec2 velo( m_InitVelo.GetX()*TB_PHYSICS_BOX2D_SCALE,m_InitVelo.GetY()*TB_PHYSICS_BOX2D_SCALE );
            m_Depend.m_Body->SetLinearVelocity( velo );
            m_Depend.m_Body->SetAngularVelocity( m_InitVeloAngle );
            m_Depend.m_Body->SetGravityScale(m_GravityScale);
            m_Scene = scene;
            // 形状適用
            m_Shape->Apply(this);
        }
    }
}

/*! 
 * シーンから外れる
 * @author teco
 */
void TbPhysicsBody2d::Exit()
{
    if( m_Scene ) {
        b2World& world = m_Scene->GetDepend().GetWorld();
        for( b2JointEdge* j = m_Depend.m_Body->GetJointList(); j; j = j->next )
        {
            TbPhysicsJoint2d* joint = reinterpret_cast<TbPhysicsJoint2d*>(j->joint->GetUserData());
            joint->NotifyDestroy();
        }
        world.DestroyBody( m_Depend.m_Body );
        m_Depend.m_Body = nullptr;
        m_Scene = nullptr;
    }
}
    
/*! 
 * 座標取得
 * @author teco
 */
TbVector2 TbPhysicsBody2d::GetPos() const
{
    TbVector2 result;
    if( m_Depend.m_Body ){
        const b2Vec2& pos = m_Depend.m_Body->GetPosition();
        result.Set( pos.x/TB_PHYSICS_BOX2D_SCALE , pos.y/TB_PHYSICS_BOX2D_SCALE );
    }
    return result;
}

/*! 
 * 角度取得
 * @author teco
 */
TbRadian TbPhysicsBody2d::GetAngle() const
{
    TbRadian result = 0.0f;
    if( m_Depend.m_Body ){
        result = m_Depend.m_Body->GetAngle();
    }
    return result;
}

/*! 
 * 速度
 * @author teco
 */
void TbPhysicsBody2d::SetVelo( const TbVector2& velo )
{
    if( m_Depend.m_Body ) {
        m_Depend.m_Body->SetLinearVelocity( b2Vec2( velo.GetX()*TB_PHYSICS_BOX2D_SCALE , velo.GetY()*TB_PHYSICS_BOX2D_SCALE ) );
    }
}

/*! 
 * 回転速度
 * @author teco
 */
void TbPhysicsBody2d::SetVeloAngle( TbRadian rot )
{
    if( m_Depend.m_Body ) {
        m_Depend.m_Body->SetAngularVelocity( rot );
    }
}

/*! 
 * 回転減衰
 * @author teco
 */
void TbPhysicsBody2d::SetDampingAngle( TbFloat32 value )
{
    if( m_Depend.m_Body ) {
        m_Depend.m_Body->SetAngularDamping( value );
    }
    m_DampingAngle = value;
}

/*! 
 * 有効設定
 * @author teco
 */
void TbPhysicsBody2d::SetActive( TbBool isActive )
{
    if( m_Depend.m_Body ) {
        m_Depend.m_Body->SetActive( isActive );
    }
}

/*! 
 * 有効かどうか
 * @author teco
 */
TbBool TbPhysicsBody2d::IsActive() const
{
    if( m_Depend.m_Body ) {
        return m_Depend.m_Body->IsActive();
    }
    return TB_FALSE;
}

/*! 
 * 動いている
 * @author teco
 */
TbBool TbPhysicsBody2d::IsAwake() const
{
    if( m_Depend.m_Body ) {
        return m_Depend.m_Body->IsAwake();
    }
    return TB_FALSE;
}

}

#endif