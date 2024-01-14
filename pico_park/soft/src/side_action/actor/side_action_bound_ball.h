/*!
 * バウンドボール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_BOUND_BALL_H_
#define _INCLUDED_SIDE_ACTION_BOUND_BALL_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionBoundBall : public CrActionActor
{
public:
    
    typedef SideActionBoundBall Self;
    typedef CrActionActor Super;

    // 消滅時コールバック
    typedef TbStaticFunction32<void ()> DeadCallback;

public:

    // コンストラクタ
    SideActionBoundBall();

    // デストラクタ
    virtual ~SideActionBoundBall();

public:

    // 死亡時コールバック
    void SetDeadCallback( const DeadCallback& callback ) {
        m_DeadCallback = callback;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_DISABLE_DEAD_FADE , 
        FLAG_MAX
    };

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision);

private:

    TbFloat32           m_Friction;
    TbUint32            m_DeadTime;
    DeadCallback        m_DeadCallback;
    TbBitArray32        m_BitArray;

};

#endif
