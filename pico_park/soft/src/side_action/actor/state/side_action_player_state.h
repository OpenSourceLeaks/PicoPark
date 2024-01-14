/*!
 * プレイヤーステート
 * @author teco
 * @since 2013.10.14
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_H_
#define _INCLUDED_ACTION_PLAYER_STATE_H_

#include <crayon/collision/cr_action_collision.h>
#include <graphics/sprite/tb_sprite.h>
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
    
    // 初期化
    virtual void Initialize(SideActionPlayer& player){}

    // 後始末
    virtual void Finalize(SideActionPlayer& player) {}

    // 更新
    virtual void Update( SideActionPlayer& player , TbFloat32 deltatime ) = 0;

    // 後処理
    virtual void Post( SideActionPlayer& player , TbFloat32 deltatime ) {}

    // イベント通知
    virtual TbSint32 NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param ) { return 0; }

    // 接触開始コールバック
    virtual void OnContactAdded( SideActionPlayer& player , const TbVector2& normal ,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info) {}

    // 前景描画
    virtual void OnDrawText(SideActionPlayer& player,
                            const TbMatrix& poseMatrix,
                            TbSpriteMaterial* material){}

};

#endif
