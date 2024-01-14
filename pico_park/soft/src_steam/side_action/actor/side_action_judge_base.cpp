/*!
 * バトルモード判定ベース
 * @author teco
 */

#include "fwd.h"
#include "side_action_judge_base.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/side_action_types.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionJudgeBase::SideActionJudgeBase()
    : Super()
    , m_Phase(PHASE_IDLE)
    , m_PhaseSec(0.0f)
    , m_PrizePlayerIndex(0)
    , m_FinishMessageX(0.0f)
    , m_IsEven(TB_FALSE)
    , m_MessageWindow(UiMessageWindow::TYPE_NORMAL)
{
    m_FinishMessage = "TIME UP";
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDraw));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );

    // 判定結果ウィンドウ
    m_MessageWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJudgeBase::~SideActionJudgeBase()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionJudgeBase::OnPostAction( TbFloat32 deltatime )
{
    if( !IsActive() ) {
        return;
    }

    switch( m_Phase )
    {
    case PHASE_TIMEUP:
        m_FinishMessageX += (CMN_WINDOW_WIDTH/3.0f) * deltatime;
        if( 1.5f*CMN_WINDOW_WIDTH < m_FinishMessageX ){
            setPhase(PHASE_WAIT_RESULT);
        }
        break;
    case PHASE_WAIT_RESULT:
        break;
    case PHASE_RESULT:
        {
            {
                if( m_IsEven ){
                    m_MessageWindow.SetVisiblePlayerImage(TB_FALSE);
                    m_MessageWindow.SetMessage("DRAW");
                }else{
                    m_MessageWindow.SetPlayerBit(TB_BIT(m_PrizePlayerIndex));
                    m_MessageWindow.SetMessage("WINNER");
                }
                m_MessageWindow.Open();
                m_MessageWindow.ReserverClose(10.0f);
                setPhase(m_Phase+1);
            }
        }
        break;
    case PHASE_RESULT+1:
        {
            if( m_MessageWindow.IsOpened() ){
                break;
            }
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_SELECT,GetActionStage()->GetId());
        }
        break;
    }

    m_PhaseSec += deltatime;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionJudgeBase::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_SWITCH_ON != event ) {
        return 0;
    }
    // プレイヤーの動きを止める
    GetStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_SUSPEND,nullptr);
    setPhase(PHASE_TIMEUP);
    return 0;
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionJudgeBase::onDraw( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    if( !IsActive() ) {
        return;
    }

    switch( m_Phase )
    {
    case PHASE_TIMEUP:
        {
            TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
            fontBuffer->PushSetting();
            fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
            fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

            // 数
            fontBuffer->Printf(m_FinishMessageX,CMN_WINDOW_HEIGHT/2,m_FinishMessage.GetBuffer());

            fontBuffer->PopSetting();
            fontBuffer->Flush();
        }
        break;
    }
}
