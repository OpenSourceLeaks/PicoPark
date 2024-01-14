/*!
 * バトル選択シーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_battle_select.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>

/*!
 * コンストラクタ
 * @author teco
 */
SeqBattleSelect::SeqBattleSelect( TbSint32 arg )
    : Super(SEQ_BATTLE_SELECT,"SeqBattleSelect")
    , m_Phase(0)
    , m_PhaseTime(0.0f)
{
    if (0 <= arg) {
        // ステージから戻ってきた
//        m_Slide.Resume(arg);
    }
}

/*!
 * デストラクタ
 * @author teco
 */
SeqBattleSelect::~SeqBattleSelect()
{
}

/*!
 * 更新
 * @author teco
 */
void SeqBattleSelect::OnUpdate()
{
    TbFloat32 deltaSec = 1 / 60.0f;
    m_PhaseTime += deltaSec;
    switch (m_Phase)
    {
    case PHASE_BEGIN:
        {
            if( !GetBgm() ){
                CreateBgm("stage_select");
            }
            FadeIn(0.0f);
            m_Slide.Start();
            addPhase();
            Root::GetInstance().SaveUserData();
        }
        break;
    case PHASE_BEGIN+1:
        {
            if (!IsReadyBgm()) {
                break;
            }
            if (Root::GetInstance().IsBusyUserData()) {
                break;
            }
            PlayBgm();
            setPhase(PHASE_STAGE_SELECT);
        }
        break;
    case PHASE_STAGE_SELECT:
        {
            if (m_Slide.IsBusy()) {
                break;
            }
            FadeOut(0.5f);
            if (!m_Slide.IsCanceled()) {
                FadeOutBgm(0.5f);
            }
            addPhase();
        }
        break;
    case PHASE_STAGE_SELECT +1:
        {
            if (IsFading()) {
                break;
            }
            Root::GetInstance().SetEnableTranslucent(TB_FALSE);
            if (m_Slide.IsCanceled())
            {
                // メインメニューに戻る
                SetEnableDeleteBgm(TB_FALSE); // BGMを引き継ぐ
                SeqManager::GetInstance().RequestSequence(SEQ_MAIN_MENU, 0);
            }
            else
            {
                TbUint32 stage = m_Slide.GetStageId();
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE, stage);
            }
        }
        break;
    }
    m_Slide.Update(deltaSec);
}

/*!
 * 描画関数
 * @author teco
 */
void SeqBattleSelect::OnDraw()
{
    m_Slide.Draw();
}

