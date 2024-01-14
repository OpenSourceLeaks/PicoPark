/*!
 * レーザーボール
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LASER_BALL_H_
#define _INCLUDED_SIDE_ACTION_LASER_BALL_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionLaserBall : public CrActionActor
{
public:
    
    typedef SideActionLaserBall Self;
    typedef CrActionActor Super;

    // 消滅時コールバック
    typedef TbStaticFunction32<void ()> DeadCallback;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_SCORE ,
        TYPE_DEAD
    };

public:

    // コンストラクタ
    SideActionLaserBall();

    // デストラクタ
    virtual ~SideActionLaserBall();

public:

    // タイプ設定
    void SetType(Type type)
    {
        m_Type = type;
    }

    // 死亡時コールバック
    void SetDeadCallback( const DeadCallback& callback ) {
        m_DeadCallback = callback;
    }

    // イベントターゲット設定
    void SetEventTarget(const char* target) {
        m_EventTarget = target;
    }

    // スケール設定
    void SetScale(TbFloat32 scale);

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
    virtual void onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision);

private:

    Type                m_Type;
    TbUint32            m_DeadTime;
    DeadCallback        m_DeadCallback;
    TbBitArray32        m_BitArray;
    TbStaticString32    m_EventTarget;

};

#endif
