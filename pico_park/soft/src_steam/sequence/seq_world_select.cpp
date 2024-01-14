/*!
 * ワールド選択シーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_world_select.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>

/*!
 * コンストラクタ
 * @author teco
 */
SeqWorldSelect::SeqWorldSelect( TbUint32 arg )
    : Super(SEQ_WORLD_SELECT,"SeqWorldSelect")
    , m_Phase(0)
    , m_PhaseTime(0.0f)
{
    if (0 < arg) {
        // ステージから戻ってきた
        m_Slide.SetPage(1);
        m_Slide.SetStage(arg-1);
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
            addPhase();
        }
        break;
    case PHASE_WORLD_SELECT+1:
        {
            if (IsFading()) {
                break;
            }
            if (m_Slide.IsCanceled())
            {
                // メインメニューに戻る
                SetEnableDeleteBgm(TB_FALSE); // BGMを引き継ぐ
                SeqManager::GetInstance().RequestSequence(SEQ_MAIN_MENU, 0);
            }
            else
            {
                TbUint32 stage = m_Slide.GetStageIndex();
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
    {
        static TB_NDEBUG_CONST TbFloat32 TEXT_Y = 90.0f;
        static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 96.0f;
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        font->PushSetting();
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        font->SetFontSize(FONT_SIZE);

        TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
        font->Printf(x, TEXT_Y, "TRIAL");
        font->PopSetting();
    }
    m_Slide.Draw();
}

