/*!
 * プレイヤー用磁石
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_MAGNET_H_
#define _INCLUDED_ACTION_PLAYER_MAGNET_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/actor/side_action_player.h"

class SideActionPlayer;

// ワープ銃
class SideActionPlayerMagnet : public CrActionActor
{
public:

    typedef SideActionPlayerMagnet Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerMagnet( SideActionPlayer* player );

    // デストラクタ
    virtual ~SideActionPlayerMagnet();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // プレイヤー番号取得
    TbUint32 GetPlayerIndex() const {
        return m_Player->GetPlayerIndex();
    }

    // ターゲット取得
    CrActionActor* GetTarget() {
        return m_Target;
    }

    // ターゲット取得
    const CrActionActor* GetTarget() const {
        return m_Target;
    }

    // ターゲットプレイヤー取得
    TbSint32 GetTargetPlayerIndex() {
        if (m_Target) {
            return m_Target->NotifyEvent(SIDE_ACTION_EVENT_GET_PLAYERINDEX,nullptr)-1;
        }
        return -1;
    }

    // ターゲット設定
    void SetTarget(CrActionActor* target);

    // ターゲット設定
    void ResetTarget() {
        SetTarget( nullptr );
    }

    // 可視判定
    TbBool IsVisible() const {
        if (m_Player) {
            return m_Player->IsVisible();
        }
        return TB_FALSE;
    }

    // 浮いている
    TbBool IsFloating() {
        const SideActionPlayer* targetPlayer = GetTargetPlayer();
        if (targetPlayer) {
            return targetPlayer->IsFloating();
        }
        return TB_TRUE;
    }

    // 
    const SideActionPlayer* GetTargetPlayer() const {
        if (m_Target) {
            SideActionPlayer* targetPlayer = TbDynamicCast<SideActionPlayer*>(m_Target);
            return targetPlayer;
        }
        return nullptr;
    }

protected:

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param ) TB_OVERRIDE;

    // 接触による移動
    virtual void OnMoveByContact(const TbVector2& mov, const toybox::TbVector2& normal, ActorSet* set, const MoveContactOption& option);

private:

    static const TbUint32 CANDIDATE_MAX = 8;

    enum
    {
        FLAG_CATCH ,
        FLAG_ACTIVE
    };

    struct Particle
    {
        Particle() : alpha(0.5f) , isActive(TB_TRUE) {}
        TbVector2 pos;
        TbFloat32 alpha;
        TbBool    isActive;
    };
    static const TbUint32 PARTICLE_MAX = 16;

private:

    // 衝突時コールバック
    void onCollided(CrActionCollision*);

    // 対象の目標座標取得
    TbVector2 getTargetDestPos() const;

    // パーティクル描画
    void onDrawParticle(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

    // パーティクル更新
    void updateParticle();

    // パーティクル生成
    void createParticle();

    // 入力ONかどうか
    TbBool isInputOn() const;

    // 対象と相互に掴みあっているか
    TbBool isCatchedRelative() const;

    SideActionPlayer* getTargetPlayer() {
        if (m_Target) {
            SideActionPlayer* targetPlayer = TbDynamicCast<SideActionPlayer*>(m_Target);
            return targetPlayer;
        }
        return nullptr;
    }

private:

    TbBitArray32                                    m_BitArray;
    SideActionPlayer*                               m_Player;
    TbStaticArray<Particle,PARTICLE_MAX>            m_Particle;

private:

     CrActionActor*                                 m_Target;
     TbStaticArray<CrActionActor*,CANDIDATE_MAX>    m_Candidate;
     CmnSpritePlayerMaterial                        m_Material;
     CrActionActorSpriteHooker                      m_SpriteHooker;
     TbUint32                                       m_OffsetUpCounter;
     TbFloat32                                      m_TimingSec;
};

#endif
