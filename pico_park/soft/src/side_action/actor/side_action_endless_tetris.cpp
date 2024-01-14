/*!
 * マーカー
 * @author teco
 */

#include "fwd.h"
#include "side_action_endless_tetris.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionEndlessTetris::SideActionEndlessTetris()
    : Super()
{
    m_Score = new SideActionScoreBase();
    SetNorma(0);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionEndlessTetris::~SideActionEndlessTetris()
{
    TB_SAFE_RELEASE(m_Score);
}

// セットアップ
void SideActionEndlessTetris::Setup(const CrActorFactory::UserData& userData)
{
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode*    node = script.GetVM(0)->GetNode(0);
    InitializeByScript(userData.AsString(0));
}

// ゲームオーバー処理
void SideActionEndlessTetris::OnGameOver()
{
    TbSint32 id = GetStage()->GetId();
    TbUint32 score = GetLineCount();
    if (SaveFile::GetInstance().SetHiscore(id, Root::GetInstance().GetPlayerCount(), score))
    {
        // ハイスコア更新
        m_Score->SetNewRecord();
    }
    m_Score->SetResultScore(score);
    m_Score->NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON, nullptr);
}

// シーンに入った
void SideActionEndlessTetris::OnEnter(void* userData)
{
    Super::OnEnter(userData);
    GetActionStage()->AddActor(m_Score,TbVector2());
}

// シーンに入った
void SideActionEndlessTetris::OnExit()
{
    m_Score->Exit();
    Super::OnExit();
}


