/*!
 * プレイヤー用銃
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_GUN_H_
#define _INCLUDED_ACTION_PLAYER_GUN_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/actor/side_action_player.h"

class SideActionPlayer;
class SideActionPlayerWarpGunBullet;

// ワープ銃
class SideActionPlayerWarpGun : public ActionActor
{
public:

    typedef SideActionPlayerWarpGun Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerWarpGun( SideActionPlayer* player );

    // デストラクタ
    virtual ~SideActionPlayerWarpGun();

public:

    // プレイヤー番号取得
    TbUint32 GetPlayerIndex() const {
        return m_Player->GetPlayerIndex();
    }

    // ターゲット取得
    ActionActor* GetTarget() {
        return m_Target;
    }

    // ターゲットプレイヤー取得
    TbSint32 GetTargetPlayerIndex() {
        if (m_Target) {
            return m_Target->NotifyEvent(SIDE_ACTION_EVENT_GET_PLAYERINDEX,nullptr)-1;
        }
        return -1;
    }

    // ターゲットプレイヤー設定
    void SetTarget(ActionActor* target);

    // ターゲットプレイヤー設定
    void ResetTarget() {
        SetTarget( nullptr );
    }

public:

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    TbBitArray32                    m_BitArray;
    SideActionPlayer*               m_Player;
    SideActionPlayerWarpGunBullet*  m_Bullet;
    CmnSpritePlayerMaterial         m_Material;

private:

    ActionActor*        m_Target;

};

// ワープ銃
class SideActionPlayerWarpGunBullet : public ActionActor
{
public:

    typedef SideActionPlayerWarpGunBullet Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerWarpGunBullet(SideActionPlayerWarpGun* gun);

    // デストラクタ
    virtual ~SideActionPlayerWarpGunBullet();

public:

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal, ActionCollision::ContactTarget target, const ActionCollisionInfo& collision);

private:

    TbBitArray32                m_BitArray;
    SideActionPlayerWarpGun*    m_Gun;
    CmnSpritePlayerMaterial     m_Material;
    TbFloat32                   m_DeadSec;

};

#endif
