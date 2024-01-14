/*!
 * メインメニューシーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_main_menu.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "system/sys_input.h"
#include <input/tb_pad.h>
#include <input/tb_mouse.h>
#include <framework/tb_framework.h>

static TbUint32  PLAYER_MIN = 2U;
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_main_menu.lua";

/*!
 * コンストラクタ
 * @author teco
 */
SeqMainMenu::SeqMainMenu()
    : Super(SEQ_TITLE,"SeqMainMenu")
    , m_Phase(PHASE_BEGIN)
    , m_PhaseTime(0.0f)
    , m_Stage(nullptr)
    , m_PlayerCount(CMN_PLAYER_COUNT)
{
    // 元に戻すキャラ入力を
    for( size_t i = 0; i < CMN_PLAYER_MAX; ++i ) {
        SysInput::SetPlayerPadIndex(i,i);
    }

    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);
    TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");
    res = node->DoFileSync(SCRIPT_FILE);
    node->GoToAbsolutely("stage_main_menu");
    m_Stage = new SideActionStage(node);
    node->GoToAbsolutely("");
}

/*!
 * デストラクタ
 * @author teco
 */
SeqMainMenu::~SeqMainMenu()
{
    TB_SAFE_DELETE(m_Stage);
}

/*!
 * 更新
 * @author teco
 */
void SeqMainMenu::OnUpdate()
{
    TbFloat32 deltaSec = 1 / 60.0f;
    m_PhaseTime += deltaSec;
    switch( m_Phase )
    {
    case PHASE_BEGIN:
        if (!GetBgm()) {
            CreateBgm("stage_select");
        }
        m_Stage->SetActive(TB_TRUE);
        m_Stage->SetPause(TB_FALSE);
        m_Slide.Start();
        FadeIn(0.0f);
        addPhase();
        break;
    case PHASE_BEGIN+1:
        if (!IsReadyBgm()) {
            break;
        }
        PlayBgm();
        setPhase(PHASE_WAIT_PAGE);
        break;
    case PHASE_WAIT_PAGE:
        if ( m_Slide.IsBusy() ) {
            break;
        }
        FadeOut(0.5f);
        addPhase();
        break;
    case PHASE_WAIT_PAGE+1:
        {
            if (IsFading()) {
                break;
            }
            if (m_Slide.IsCanceled())
            {
                SeqManager::GetInstance().RequestSequence(SEQ_TITLE, 1);
            }
            else
            {
                // プレイヤー人数決定
                Root::GetInstance().SetPlayerCount(m_Slide.GetPlayerCount());
                // ワールド選択シーケンスに移動
                SetEnableDeleteBgm(TB_FALSE); // BGMを引き継ぐ
                SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT, 0);
            }
        }
        break;
    }
    m_Stage->Update();
    m_Slide.Update(deltaSec);
}

/*!
 * 描画関数
 * @author teco
 */
void SeqMainMenu::OnDraw()
{
    m_Stage->Draw();
    m_Slide.Draw();
}
