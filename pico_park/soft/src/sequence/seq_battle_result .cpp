/*!
 * バトルリザルトシーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_battle_result.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>

/*!
 * コンストラクタ
 * @author teco
 */
SeqBattleResult::SeqBattleResult( TbSint32 arg )
    : Super(SEQ_WORLD_SELECT,"SeqBattleResult")
    , m_Phase(0)
    , m_PhaseTime(0.0f)
    , m_TotalWinner(0)
    , m_TotalWinnerWindow(UiMessageWindow::TYPE_NORMAL)
{
    m_TotalWinnerWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f));
    m_TotalWinnerWindow.SetAlign(TB_UI_ALIGN_X_CENTER, TB_UI_ALIGN_Y_CENTER);
    m_TotalWinnerWindow.SetEnableCloseButton(TB_FALSE);
    m_TotalWinnerWindow.SetMessage("TOTAL WINNER");
}

/*!
 * デストラクタ
 * @author teco
 */
SeqBattleResult::~SeqBattleResult()
{
}

/*!
 * 更新
 * @author teco
 */
void SeqBattleResult::OnUpdate()
{
    TbFloat32 deltaSec = 1 / 60.0f;
    m_PhaseTime += deltaSec;
    switch (m_Phase)
    {
    case PHASE_BEGIN:
        {
            FadeIn(0.0f);
            m_Slide.Start();
            addPhase();
        }
        break;
    case PHASE_BEGIN+1:
        {
            PlayBgm();
            setPhase(PHASE_RESULT);
        }
        break;
    case PHASE_RESULT:
        {
            if (m_Slide.IsBusy()) {
                break;
            }
            if( BattleManager::GetInstance().GetTotalWinner(&m_TotalWinner) ){
                // 総合優勝者の表彰
                m_TotalWinnerWindow.SetPlayerBit(TB_BIT(m_TotalWinner));
                m_TotalWinnerWindow.Open();
                TbPlaySound("fanf02");
                setPhase(PHASE_TOTAL_WINNER);
            }else{
                SeqManager::GetInstance().RequestSequence(SEQ_BATTLE_SELECT, 0);
            }
        }
        break;
    case PHASE_TOTAL_WINNER:
        {
            static TB_NDEBUG_CONST TbFloat32 WAIT_SEC = 5.0f;
            if (WAIT_SEC < m_PhaseTime) {
                FadeOut(0.5f);
                addPhase();
            }
        }
        break;
    case PHASE_TOTAL_WINNER+1:
        {
            if (IsFading()) {
                break;
            }
            SeqManager::GetInstance().RequestSequence(SEQ_MAIN_MENU, 0);
        }
        break;
    }
    m_Slide.Update(deltaSec);
}

/*!
 * 描画関数
 * @author teco
 */
void SeqBattleResult::OnDraw()
{
    m_Slide.Draw();
}

