/*!
 * アクション用物理コンポーネント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_ACTOR_PHYSICS_H_
#define _INCLUDED_ACTION_ACTOR_PHYSICS_H_

#include "common/actor/action_actor_component.h"

#include <physics/tb_physics_body_2d.h>
#include <physics/tb_physics_joint_2d.h>

/*!
 * 物理ボディ
 * @author teco
 */
class ActionActorPhysicsBody : public ActionActorComponent
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
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
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
    virtual void OnSetVelo( TbVector2* velo ) {
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
class ActionActorPhysicsPinningJoint : public ActionActorComponent
{
public:

    // コンストラクタ
    ActionActorPhysicsPinningJoint()
    {
    }

public:

    // 初期化
    void Initialize( TbPhysicsBody2d* body , const TbVector2& offset = TbVector2() )
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
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
        m_Joint.SetAnchor( pos + m_Offset , TbVector2() );
        m_Joint.Create(&stage->GetPhysicsScene());
    }

    // シーンから出た
    virtual void OnExit() {
        m_Joint.Destroy();
    }

private:
    toybox::TbPhysicsRevoluteJoint2d   m_Joint;
    TbVector2                          m_Offset;
};

/*!
 * ジョイント
 * @author teco
 */
class ActionActorPhysicsGearJoint : public ActionActorComponent
{
public:

    // コンストラクタ
    ActionActorPhysicsGearJoint()
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
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
        m_Joint.Create(&stage->GetPhysicsScene());
    }

    // シーンから出た
    virtual void OnExit() {
        m_Joint.Destroy();
    }

private:
    toybox::TbPhysicsGearJoint2d   m_Joint;
};

#endif
