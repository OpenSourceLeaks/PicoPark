/*!
 * メインメニューシーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_main_menu.h"
#include "root.h"
#include "common/cmn_util.h"
#include <framework/sub/tb_framework_simple.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include <crayon/input/cr_input.h>
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
    : Super(SEQ_MAIN_MENU,"SeqMainMenu")
    , m_Phase(PHASE_BEGIN)
    , m_PhaseTime(0.0f)
    , m_Stage(nullptr)
{
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
    m_Slide.Update(deltaSec);
    switch( m_Phase )
    {
    case PHASE_BEGIN:
        Root::GetInstance().SaveUserData();
        addPhase();
        break;
    case PHASE_BEGIN+1:
        if (!GetBgm()) {
            CreateBgm("stage_select");
        }
        m_Stage->SetActive(TB_TRUE);
        m_Slide.Start();
        FadeIn(0.0f);
        addPhase();
        break;
    case PHASE_BEGIN + 2:
        if (!IsReadyBgm()) {
            break;
        }
        if (Root::GetInstance().IsBusyUserData()) {
            break;
        }
        PlayBgm();
        setPhase(PHASE_WAIT_PAGE);
        break;

    case PHASE_BEGIN_RESUME:
        {
            m_Slide.Restart();
            FadeIn(0.0f);
            if (!GetBgm()) {
                CreateBgm("stage_select");
                setPhase(PHASE_BEGIN + 1);
            }else {
                setPhase(PHASE_WAIT_PAGE);
            }
        }
        break;

    case PHASE_WAIT_PAGE:
        if( m_Slide.IsBusy() ) {
            break;
        }
        setPhase(PHASE_FINISH);
        break;
    case PHASE_FINISH:
        FadeOut(0.5f);
        if (m_Slide.IsCanceled())
        {
            FadeOutBgm(0.5f);
        }
        addPhase();
        break;
    case PHASE_FINISH +1:
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
                // ワールド選択シーケンスに移動
                SetEnableDeleteBgm(TB_FALSE); // BGMを引き継ぐ
                SeqManager::GetInstance().AddCacheSeq(this);
                switch (m_Slide.GetMode())
                {
                case MainMenuModeSelectPage::MODE_COOP:
                    SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT,-1);
                    break;
                case MainMenuModeSelectPage::MODE_BATTLE:
                    BattleManager::GetInstance().Clear();
                    SeqManager::GetInstance().RequestSequence(SEQ_BATTLE_SELECT,-1);
                    break;
                case MainMenuModeSelectPage::MODE_CHALLENGE:
                    SeqManager::GetInstance().RequestSequence(SEQ_ENDLESS_SELECT, -1);
                    break;
                case MainMenuModeSelectPage::MODE_OPTION:
                    // ここには来ない
                    TB_ASSERT(!"invalid");
                    break;
                }
            }
        }
        break;
    }
    m_Stage->Update();
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

/*!
 * リセット 
 * @author teco
 */
void  SeqMainMenu::OnReset()
{
    setPhase(PHASE_BEGIN_RESUME);
    SetEnableDeleteBgm(TB_TRUE);
}
