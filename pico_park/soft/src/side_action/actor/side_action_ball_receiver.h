/*!
 * ボールのレベル
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_BALL_LEVEL_H_
#define _INCLUDED_SIDE_ACTION_BALL_LEVEL_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action_ball_speed_controller.h"
#include "side_action_key.h"
#include "root.h"

class SideActionBallReceiver : public CrActionActor
{
public:
    
    typedef SideActionBallReceiver Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionBallReceiver();

    // デストラクタ
    virtual ~SideActionBallReceiver();

public:

    // レベルアップ
    void LevelUp();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData); 
        
    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision);

private:

    TbStaticString32    m_Target;
    TbUint32            m_Count;
    TbUint32            m_CountInLevel;
    TbUint32            m_Level;
    SideActionBallSpeedController   m_BallSpeedController;

};


#endif
