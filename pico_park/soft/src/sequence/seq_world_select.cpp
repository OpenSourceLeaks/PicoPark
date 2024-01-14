/*!
 * ワールド選択シーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_world_select.h"
#include "root.h"
#include "base/time/tb_tick.h"
#include <framework/sub/tb_framework_simple.h>

static const TbSint32 ARG_OPEN_ALL_WORLD = -99;

/*!
 * コンストラクタ
 * @author teco
 */
SeqWorldSelect::SeqWorldSelect( TbSint32 arg )
    : Super(SEQ_WORLD_SELECT,"SeqWorldSelect")
    , m_Phase(0)
    , m_PhaseTime(0.0f)
{
    Root::GetInstance().GetGlobalData().ResetCheckPoint();
    if ( arg == ARG_OPEN_ALL_WORLD ) {
        m_Slide.SetOpenAllWorldMode();
    } else if (0 <= arg) {
        TbBool isNextStage = SeqManager::GetInstance().GetRequestUserDataU32(USER_DATA_NEXT_STAGE);
        // ステージから戻ってきた
        m_Slide.Resume(arg, isNextStage);
        Root::GetInstance().SaveUserData();
    }
}

/*!
 * デストラクタ
 * @author teco
 */
SeqWorldSelect::~SeqWorldSelect()
{
}

/*!
 * 更新
 * @author teco
 */
void SeqWorldSelect::OnUpdate()
{
    TbFloat32 deltaSec = 1 / 60.0f;
//    deltaSec = TbTick::GetFrameSpeedSeconds();
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
            if (!m_Slide.IsReadyBgm())
            {
                if (!m_Slide.IsBusy()) {
                    setPhase(PHASE_WORLD_SELECT);
                }
                break;
            }
            PlayBgm();
            setPhase(PHASE_WORLD_SELECT);
        }
        break;
    case PHASE_WORLD_SELECT:
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
    case PHASE_WORLD_SELECT+1:
        {
            if (IsFading()) {
                break;
            }
            if (m_Slide.IsOpenAllWorld())
            {
                SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT, ARG_OPEN_ALL_WORLD);
            }
            else if (m_Slide.IsCanceled())
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
void SeqWorldSelect::OnDraw()
{
    m_Slide.Draw();
}

