/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 */

#include <tb_fwd.h>
#include <physics/tb_physics_joint_2d.h>
#include <physics/tb_physics_body_2d.h>
#include <physics/tb_physics_scene_2d.h>

#if TB_PHYSICSAPI_IS_BOX2D

namespace toybox
{

///////////////////////////////////////////
//  TbPhysicsJoint2d
///////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsJoint2d::TbPhysicsJoint2d()
    : m_Depend()
{
    TbFillNull(m_Bodys);
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsJoint2d::~TbPhysicsJoint2d()
{
    m_Depend.Reset(TB_TRUE);
    ResetBody();
}

/*! 
 * 外部からの破棄通知
 * @author teco
 */
void TbPhysicsJoint2d::NotifyDestroy()
{
    m_Depend.Reset(TB_FALSE);
}

/*! 
 * 生成済み
 * @author teco
 */
TbBool TbPhysicsJoint2d::IsCreated() const
{
    return m_Depend.IsValid();
}

/*! 
 * 依存アクター追加
 * @author teco
 */
void TbPhysicsJoint2d::SetBody( TbUint32 index , TbPhysicsBody2d* body ) 
{
    TB_RESQUE(index<TB_ARRAY_COUNT_OF(m_Bodys));
    if( m_Bodys[index] == body ) {
        // 多重登録は許可しない
        return;
    }
    m_Bodys[index] = body;
}

/*! 
 * 依存アクター除去
 * @author teco
 */
void TbPhysicsJoint2d::ResetBody() 
{
    TB_ASSERT(!IsCreated());
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Bodys); ++i ) {
        if( m_Bodys[i] ) {
            m_Bodys[i] = nullptr;
        }
    }
}

///////////////////////////////////////////
//  TbPhysicsRevoluteJoint2d
///////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsRevoluteJoint2d::TbPhysicsRevoluteJoint2d()
{
    m_Depend.jointDef.collideConnected = false;
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsRevoluteJoint2d::~TbPhysicsRevoluteJoint2d()
{
}

/*! 
 * アクター設定
 * @author teco
 */
void TbPhysicsRevoluteJoint2d::Initialize( TbPhysicsBody2d* body1 , TbPhysicsBody2d* body2 )
{
    TB_RESQUE(body2);
    ResetBody();
    SetBody(0,body1);
    SetBody(1,body2);
}

/*! 
 * 制限
 * @author teco
 */
void TbPhysicsRevoluteJoint2d::SetLimit( TbFloat32 minRad , TbFloat32 maxRad )
{
    m_Depend.jointDef.enableLimit = true;
    m_Depend.jointDef.lowerAngle = minRad;
    m_Depend.jointDef.upperAngle = maxRad;
}

/*! 
 * アンカー
 * @param pos1 actor1のどの部分にactor2を貼り付けるか
 * @param pos2 actor2のどの部分をpos1に合わせるか
 * @author teco
 */
void TbPhysicsRevoluteJoint2d::SetAnchor( const TbVector2& pos1 , const TbVector2& pos2 )
{
    m_Depend.jointDef.localAnchorA.Set( pos1.GetX()*TB_PHYSICS_BOX2D_SCALE , pos1.GetY()*TB_PHYSICS_BOX2D_SCALE );
    m_Depend.jointDef.localAnchorB.Set( pos2.GetX()*TB_PHYSICS_BOX2D_SCALE , pos2.GetY()*TB_PHYSICS_BOX2D_SCALE );
}

/*! 
 * 生成
 * @author teco
 */
void TbPhysicsRevoluteJoint2d::Create( TbPhysicsScene2d* scene )
{
    if( IsCreated() ){
        TB_ASSERT(0);
        return;
    }
    TB_RESQUE(GetBody(1));

    b2World& world = scene->GetDepend().GetWorld();
    if( GetBody(0) ){
        m_Depend.jointDef.bodyA = GetBody(0)->GetDepend().GetBody();
    }else{
        m_Depend.jointDef.bodyA = scene->GetDepend().GetBackGround();
    }
    m_Depend.jointDef.bodyB = GetBody(1)->GetDepend().GetBody();
    m_Depend.jointDef.collideConnected = IsCollideConnected() ? true : false; 
    b2Joint* joint = world.CreateJoint( &m_Depend.jointDef );;
    if( joint ) {
        Super::GetDepend().Set( joint , &world , this );
    }
}

/*! 
 * 破棄
 * @author teco
 */
void TbPhysicsRevoluteJoint2d::Destroy()
{
    if( !IsCreated() ){
        return;
    }
    Super::GetDepend().Reset(TB_TRUE);
}

///////////////////////////////////////////
//  TbPhysicsGearJoint2d
///////////////////////////////////////////

/*! 
 * コンストラクタ
 * @author teco
 */
TbPhysicsGearJoint2d::TbPhysicsGearJoint2d()
{
    TbFillNull(m_Joints);
    m_Depend.jointDef.collideConnected = false;
}

/*! 
 * デストラクタ
 * @author teco
 */
TbPhysicsGearJoint2d::~TbPhysicsGearJoint2d()
{
}

/*! 
 * アクター設定(連動するJoint)
 * @author teco
 */
void TbPhysicsGearJoint2d::Initialize( TbPhysicsBody2d* body1 , TbPhysicsBody2d* body2 ,
                                       TbPhysicsJoint2d* joint1 , TbPhysicsJoint2d* joint2 )
{
    if( IsCreated() ){
        TB_ASSERT(0);
        return;
    }
    TB_RESQUE(body1);
    TB_RESQUE(body2);
    TB_RESQUE(joint1);
    TB_RESQUE(joint2);
    ResetBody();
    m_Joints[0] = joint1;
    m_Joints[1] = joint2;
    SetBody( 0 , body1 );
    SetBody( 1 , body2 );
}

/*! 
 * 適用率設定
 * @author teco
 */
void TbPhysicsGearJoint2d::SetRatio( TbFloat32 ratio )
{
    m_Depend.jointDef.ratio = ratio;
}

/*! 
 * 適用
 * @author teco
 */
void TbPhysicsGearJoint2d::Create( TbPhysicsScene2d* scene )
{
    if( IsCreated() ){
        TB_ASSERT(0);
        return;
    }    
    b2World& world = scene->GetDepend().GetWorld();
    m_Depend.jointDef.collideConnected = IsCollideConnected() ? true : false;
    m_Depend.jointDef.bodyA = GetBody(0)->GetDepend().GetBody();
    m_Depend.jointDef.bodyB = GetBody(1)->GetDepend().GetBody();
    m_Depend.jointDef.joint1 = m_Joints[0]->GetDepend().GetJoint();
    m_Depend.jointDef.joint2 = m_Joints[1]->GetDepend().GetJoint();
    b2Joint* joint = world.CreateJoint( &m_Depend.jointDef );;
    if( joint ) {
        Super::GetDepend().Set( joint , &world , this );
    }
}

/*! 
 * 破棄
 * @author teco
 */
void TbPhysicsGearJoint2d::Destroy()
{
    if( !IsCreated() ){
        return;
    }
    Super::GetDepend().Reset(TB_TRUE);
}

}

#endif