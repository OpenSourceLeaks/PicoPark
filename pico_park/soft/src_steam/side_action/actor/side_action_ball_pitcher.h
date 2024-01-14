/*!
 * バウンドボールピッチャー
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_BALL_PITCHER_H_
#define _INCLUDED_SIDE_ACTION_BALL_PITCHER_H_

#include "common/actor/action_actor.h"

class SideActionBallPitcher : public ActionActor
{
public:
    
    typedef SideActionBallPitcher Self;
    typedef ActionActor Super;

    enum Type
    {
        TYPE_BOUND , 
        TYPE_LASER ,
        TYPE_PHYSICS
    };

public:

    // コンストラクタ
    SideActionBallPitcher( Type type , TbFloat32 rotDeg );

    // デストラクタ
    virtual ~SideActionBallPitcher();

public:

    // 速度設定
    void SetSpeed( TbFloat32 speed ){
        m_Speed = speed;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    Type                    m_Type;
    TbFloat32               m_CreateTimer;
    TbFloat32               m_Speed;
    ActionActorSprite       m_BaseSprite;;
    ActionActor*            m_Ball;
    TbVector2               m_Dir;

};

#endif
