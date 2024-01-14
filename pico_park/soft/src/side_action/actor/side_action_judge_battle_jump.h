/*!
 * ジャンプバトル用ジャッジ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_JUDGE_BATTLE_JUMP_H_
#define _INCLUDED_SIDE_ACTION_JUDGE_BATTLE_JUMP_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/actor/side_action_judge_base.h"
#include "side_action_ball_speed_controller.h"
#include "side_action/side_action_types.h"

class SideActionJudgeBattleJump : public SideActionJudgeBase
{
public:

    typedef SideActionJudgeBattleJump Self;
    typedef SideActionJudgeBase Super;

public:

    // コンストラクタ
    SideActionJudgeBattleJump();

    // デストラクタ
    virtual ~SideActionJudgeBattleJump();

public:

    // エンター処理
    virtual void OnEnter(void* userData) TB_OVERRIDE;

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

    // イベント通知
    TbSint32 NotifyEvent(TbUint32 event, void* param);

private:

    enum
    {
        PHASE_TUTORIAL, // チュートリアル
        PHASE_ACTIVE = 10, // 開始
        PHASE_SUSPEND = 20, // 
    };

private:

    // 前景描画
    void onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

private:

    CrActionActorSpriteHooker       m_SpriteHooker;
    TbUint32                        m_Phase;
    TbBool                          m_HasTutorial;
    TbUint32                        m_Count;
    TbUint32                        m_ActiveRefCount;
    TbStaticString32                m_TargetName;
    TbSint32                        m_BestPlayer;
    TbUint32                        m_Level;
    TbUint32                        m_CountInLevel;
    SideActionBallSpeedController   m_SpeedController;

};

#endif
