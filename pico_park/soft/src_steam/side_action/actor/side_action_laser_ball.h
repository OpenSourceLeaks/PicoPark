/*!
 * レーザーボール
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LASER_BALL_H_
#define _INCLUDED_SIDE_ACTION_LASER_BALL_H_

#include "common/actor/action_actor.h"

class SideActionLaserBall : public ActionActor
{
public:
    
    typedef SideActionLaserBall Self;
    typedef ActionActor Super;

    // 消滅時コールバック
    typedef TbStaticFunction32<void ()> DeadCallback;

public:

    // コンストラクタ
    SideActionLaserBall();

    // デストラクタ
    virtual ~SideActionLaserBall();

public:

    // 死亡時コールバック
    void SetDeadCallback( const DeadCallback& callback ) {
        m_DeadCallback = callback;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum Flag
    {
        FLAG_DISABLE_DEAD_FADE , 
        FLAG_MAX
    };

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& collision);

private:

    TbFloat32           m_Friction;
    TbUint32            m_DeadTime;
    DeadCallback        m_DeadCallback;
    TbBitArray32        m_BitArray;

};

#endif
