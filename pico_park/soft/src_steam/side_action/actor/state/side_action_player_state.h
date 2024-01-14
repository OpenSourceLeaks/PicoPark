/*!
 * プレイヤーステート
 * @author teco
 * @since 2013.10.14
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_H_
#define _INCLUDED_ACTION_PLAYER_STATE_H_

#include "common/collision/action_collision.h"
#include "side_action/side_action_types.h"

class SideActionPlayer;

class SideActionPlayerState
{
public:

    // コンストラクタ
    SideActionPlayerState() {}

    // デストラクタ
    virtual ~SideActionPlayerState() {}

public:
    
    // 更新
    virtual void Update( SideActionPlayer& player , TbFloat32 deltatime ) = 0;

    // 後処理
    virtual void Post( SideActionPlayer& player , TbFloat32 deltatime ) {}

    // イベント通知
    virtual void NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param ) {}

    // 接触開始コールバック
    virtual void OnContactAdded( SideActionPlayer& player , const TbVector2& normal ,ActionCollision::ContactTarget target,const ActionCollisionInfo& info) {}

};

#endif
