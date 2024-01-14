/*!
 * シーソー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_SEESAW_H_
#define _INCLUDED_SIDE_ACTION_SEESAW_H_

#include <crayon/actor/cr_action_actor.h>
#include <physics/tb_physics_shape_2d.h>
#include <physics/tb_physics_joint_2d.h>

class SideActionSeesaw : public CrActionActor
{
public:
    
    typedef SideActionSeesaw Self;
    typedef CrActionActor Super;

    enum Type
    {
        TYPE_NORMAL , // 普通
        TYPE_LEFT  , // 左に伸びてる
        TYPE_RIGHT , // 右に伸びてる
    };

public:

    // コンストラクタ
    SideActionSeesaw( Type type );

    // デストラクタ
    virtual ~SideActionSeesaw();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // ポスト更新
    virtual void OnPostAction( TbFloat32 deltatime );

public:

    // 初期角速度設定
    void SetInitVeloAngle( TbRadian angle ) {
        m_Body.GetBody().SetInitVeloAngle( angle );
    }

    // 連動シーソーと接続
    void Attach( SideActionSeesaw* seesaw );

    // ボディ取得
    TbPhysicsBody2d* GetBody() {
        return &m_Body.GetBody();
    }

    // ジョイント取得
    TbPhysicsJoint2d* GetJoint() {
        return &m_Joint.GetJoint();
    }

private:

    TbBitArray32                      m_BitArray;
    CrActionActorElasticSprite        m_Sprite;
    CrActionActorPhysicsBody          m_Body;
    TbPhysicsShapePolygon2d           m_Shape;
    CrActionActorPhysicsPinningJoint  m_Joint;
    CrActionActorPhysicsGearJoint     m_GearJoint;

};

class SideActionSeesawParent : public SideActionSeesaw
{
public:
    
    typedef SideActionSeesawParent Self;
    typedef SideActionSeesaw Super;

public:

    // コンストラクタ
    SideActionSeesawParent( SideActionSeesaw::Type type )
        : Super( type )
        , m_TargetAngle(0.0f)
    {}

    // デストラクタ
    virtual ~SideActionSeesawParent() {}

public:

    // 操作範囲
    void SetControllRangeX( TbFloat32 begin , TbFloat32 end ) {}

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    TbRadian m_TargetAngle;

};

#endif
