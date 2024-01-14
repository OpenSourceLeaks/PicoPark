/*!
 * アクション用物理コンポーネント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_ACTION_ACTOR_PHYSICS_H_
#define _INCLUDED_CR_ACTION_ACTOR_PHYSICS_H_

#include <crayon/actor/cr_action_actor_component.h>

#include <physics/tb_physics_body_2d.h>
#include <physics/tb_physics_joint_2d.h>

namespace crayon
{

/*!
 * 物理ボディ
 * @author teco
 */
class CrActionActorPhysicsBody : public CrActionActorComponent
{
public:

    // ボディ取得
    toybox::TbPhysicsBody2d& GetBody() {
        return m_Body;
    }

    // ボディ取得
    const toybox::TbPhysicsBody2d& GetBody() const {
        return m_Body;
    }

public:

    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
        m_Body.SetUserData(GetOwner());
        m_Body.SetInitPos( pos );
        m_Body.SetInitAngle( angle );
        m_Body.Enter(&stage->GetPhysicsScene());
    }

    // シーンから出た
    virtual void OnExit() {
        m_Body.Exit();
    }

    // 速度が設定された
    virtual void OnSetVelo( toybox::TbVector2* velo ) {
        m_Body.SetVelo( *velo );
        velo->Clear();
    }

private:
    toybox::TbPhysicsBody2d    m_Body;
};

/*!
 * ピン止めジョイント
 * @author teco
 */
class CrActionActorPhysicsPinningJoint : public CrActionActorComponent
{
public:

    // コンストラクタ
    CrActionActorPhysicsPinningJoint()
    {
    }

public:

    // 初期化
    void Initialize( toybox::TbPhysicsBody2d* body , const toybox::TbVector2& offset = toybox::TbVector2() )
    {
        m_Joint.Initialize( nullptr , body );
        m_Offset = offset;
    }

    // ジョイント取得
    toybox::TbPhysicsRevoluteJoint2d& GetJoint() {
        return m_Joint;
    }

    // ジョイント取得
    const toybox::TbPhysicsRevoluteJoint2d& GetJoint() const {
        return m_Joint;
    }


public:

    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
        m_Joint.SetAnchor( pos + m_Offset , toybox::TbVector2() );
        m_Joint.Create(&stage->GetPhysicsScene());
    }

    // シーンから出た
    virtual void OnExit() {
        m_Joint.Destroy();
    }

private:
    toybox::TbPhysicsRevoluteJoint2d   m_Joint;
    toybox::TbVector2                  m_Offset;
};

/*!
 * ジョイント
 * @author teco
 */
class CrActionActorPhysicsGearJoint : public CrActionActorComponent
{
public:

    // コンストラクタ
    CrActionActorPhysicsGearJoint()
    {
    }

public:

    // ジョイント取得
    toybox::TbPhysicsGearJoint2d& GetJoint() {
        return m_Joint;
    }

    // ジョイント取得
    const toybox::TbPhysicsGearJoint2d& GetJoint() const {
        return m_Joint;
    }


public:

    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
        m_Joint.Create(&stage->GetPhysicsScene());
    }

    // シーンから出た
    virtual void OnExit() {
        m_Joint.Destroy();
    }

private:
    toybox::TbPhysicsGearJoint2d   m_Joint;
};

}

#endif
