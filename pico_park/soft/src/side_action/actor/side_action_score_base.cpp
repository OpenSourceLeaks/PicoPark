/*!
 * スコアベース
 * @author teco
 */

#include "fwd.h"
#include "side_action_score_base.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static const TbRectF32Pod ICON_UV_RECT = { 
    CMN_TEX_UV_RECT(368,352, 23, 13)
};
static TbFloat32 ICON_OFFSET_Y = -60.0f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionScoreBase::SideActionScoreBase()
    : Super()
    , m_Type(TYPE_SCORE)
    , m_Result(0)
    , m_IsNewRecord(TB_FALSE)
    , m_Phase(PHASE_IDLE)
    , m_PhaseSec(0.0f)
    , m_FinishMessageX(0.0f)
    , m_MessageWindow(UiMessageWindow::TYPE_OK)
    , m_ConfirmWindow(UiMessageWindow::TYPE_YESNO)
{
    m_FinishMessage = "FINISH";
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDraw));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );

//    m_IconNew.Enter(this);
    m_IconNew.SetAlignX(TB_UI_ALIGN_X_LEFT);
    m_IconNew.SetBasePos(TbUiWidget::BASE_POS_LEFT_CENTER);
    m_IconNew.SetSize(46.0f*2.0f,26.0f*2.0f);
    m_IconNew.SetPos(8.0f, ICON_OFFSET_Y);
    m_IconNew.SetImage(nullptr, ICON_UV_RECT);

    // 判定結果ウィンドウ
    m_MessageWindow.SetEnableCloseButton(TB_FALSE);
    m_MessageWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
    m_MessageWindow.SetTextAlignX(TB_UI_ALIGN_X_CENTER);
    m_ConfirmWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f));
    m_ConfirmWindow.SetTextAlignX(TB_UI_ALIGN_X_CENTER);
    m_ConfirmWindow.SetMessage("RETRY?");
    m_ConfirmWindow.SetFocusResult(UiMessageWindow::RESULT_YES);
    m_ConfirmWindow.SetEnableInput(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionScoreBase::~SideActionScoreBase()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionScoreBase::OnPostAction( TbFloat32 deltatime )
{
    if( !IsActive() ) {
        return;
    }

    switch( m_Phase )
    {
    case PHASE_FINISH:
        m_FinishMessageX += (CMN_WINDOW_WIDTH/3.0f) * deltatime;
        if( 1.5f*CMN_WINDOW_WIDTH < m_FinishMessageX ){
            setPhase(PHASE_RESULT);
        }
        break;
    case PHASE_RESULT:
        {
            m_MessageWindow.SetPlayerBit(0xFFFF);
            m_MessageWindow.SetVisiblePlayerImage(TB_FALSE);
            TbStaticString128 text;
            switch (m_Type)
            {
            case TYPE_SCORE:
                {
                    text.SetFormatedString("RECORD:%d",m_Result);
                }
                break;
            case TYPE_TIME:
                {
                    TbFloat32 result = static_cast<TbFloat32>(m_Result)/100.0f;
                    TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(result), 0, 99);
                    TbSint32 msec = static_cast<TbSint32>(result*100.0f) % 100;
                    text.SetFormatedString("RECORD %d:%d", sec , msec);
                }
                break;
            }
            if (m_IsNewRecord)
            {
                TbStaticString128 tmp = text;
                text.SetFormatedString("     %s" , tmp.GetBuffer());
                m_IconNew.Enter(&m_MessageWindow);
//                    TbPlaySound("coin");
            }
            m_MessageWindow.SetMessage(text.GetBuffer());
            m_MessageWindow.Open();
            m_MessageWindow.ReserverClose(20.0f);
            m_MessageWindow.SetEnableInput(TB_FALSE);
            setPhase(m_Phase+1);
        }
        break;
    case PHASE_RESULT+1:
        {
            if (m_MessageWindow.IsOpened() && m_PhaseSec < 0.5f) {
                break;
            }
            m_MessageWindow.SetEnableInput(TB_TRUE);
            setPhase(m_Phase+1);
        }
        break;
    case PHASE_RESULT + 2:
        {
            if( m_MessageWindow.IsOpened() ){
                break;
            }
            TbSint32 inputPlayer = m_MessageWindow.GetInputPlayer();
            if (0 <= inputPlayer) {
                m_ConfirmWindow.SetPlayerBit(TB_BIT(inputPlayer));
            }
            m_ConfirmWindow.Open();
            setPhase(m_Phase + 1);
        }
        break;
    case PHASE_RESULT + 3:
        {
            if (m_ConfirmWindow.IsOpened()) {
                break;
            }
            if (m_ConfirmWindow.GetResult() == UiMessageWindow::RESULT_CANCEL) {
                setPhase(PHASE_RESULT);
                break;
            }
            TbFloat32 fadeSec = 0.5f;
            SeqManager::GetInstance().FadeOut(fadeSec);
            SeqManager::GetInstance().FadeOutBgm(fadeSec);
            setPhase(m_Phase + 1);
        }
        break;
    case PHASE_RESULT + 4:
        {
            if (SeqManager::GetInstance().IsFading()) {
                break;
            }
            if (UiMessageWindow::RESULT_YES == m_ConfirmWindow.GetResult())
            {
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE, GetActionStage()->GetId());
            }
            else
            {
                SeqManager::GetInstance().RequestSequence(SEQ_ENDLESS_SELECT, GetActionStage()->GetId());
            }
        }
        break;
    }

    m_PhaseSec += deltatime;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionScoreBase::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_SWITCH_ON != event ) {
        return 0;
    }
    SideActionStage* saStage = TbDynamicCast<SideActionStage*>(GetStage());
    if (saStage) {
        saStage->SetEnablePause(TB_FALSE);
        saStage->SetScrollType(SideActionStage::SCROLL_NONE);
        TbUint32 playerCount = saStage->GetPlayerCount();
        saStage->NotifyEvent(nullptr,SIDE_ACTION_EVENT_SUSPEND,nullptr);
        // 死んでいるやつだけ動かす
        for (TbUint32 i = 0; i < playerCount; ++i)
        {
            SideActionPlayer* player = saStage->GetPlayer(i);
            if (!player->IsAlive()) {
                player->Resume();
            }
        }
    }
    setPhase(PHASE_RESULT);
    return 0;
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionScoreBase::onDraw( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    if( !IsActive() ) {
        return;
    }

    switch( m_Phase )
    {
    case PHASE_FINISH:
        {
            TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
            fontBuffer->PushSetting();
            fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
            fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
            fontBuffer->SetEnableBorder(TB_TRUE);
            fontBuffer->SetPoseMatrix(TbMatrix::IDENTITY);

            // 数
            fontBuffer->Printf(m_FinishMessageX,CMN_WINDOW_HEIGHT/2,m_FinishMessage.GetBuffer());

            fontBuffer->PopSetting();
            fontBuffer->Flush();
        }
        break;
    }
}
