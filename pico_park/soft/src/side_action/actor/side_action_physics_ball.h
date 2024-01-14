/*!
 * 物理ボール
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_PHYSICS_BALL_H_
#define _INCLUDED_SIDE_ACTION_PHYSICS_BALL_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionPhysicsBall : public CrActionActor
{
public:
    
    typedef SideActionPhysicsBall Self;
    typedef CrActionActor Super;

    // 消滅時コールバック
    typedef TbStaticFunction32<void ()> DeadCallback;

public:

    // コンストラクタ
    SideActionPhysicsBall();

    // デストラクタ
    virtual ~SideActionPhysicsBall();

public:

    // 死亡時コールバック
    void SetDeadCallback( const DeadCallback& callback ) {
        m_DeadCallback = callback;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // ポスト更新
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_DISABLE_DEAD_FADE , 
        FLAG_MAX
    };

private:

    TbFloat32               m_Friction;
    TbUint32                m_DeadTime;
    DeadCallback            m_DeadCallback;
    TbBitArray32            m_BitArray;

    CrActionActorPhysicsBody  m_Body;
    TbPhysicsShapeCircle2d  m_CircleShape;

};

#endif
