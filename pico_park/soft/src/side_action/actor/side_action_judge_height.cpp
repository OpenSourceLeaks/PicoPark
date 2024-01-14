/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_judge_height.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "common/cmn_util.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionJudgeHeight::SideActionJudgeHeight()
    : Super()
    , m_Phase(PHASE_IDLE)
    , m_PhaseSec(0.0f)
    , m_PrizeTargetY(CMN_WINDOW_HEIGHT)
{
    SetFinishMessage("TIME UP");
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDraw));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionJudgeHeight::~SideActionJudgeHeight()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionJudgeHeight::OnPostAction( TbFloat32 deltatime )
{
    Super::OnPostAction(deltatime);
    if( !IsActive() ) {
        return;
    }

    switch( m_Phase )
    {
    case PHASE_TIMEUP:
        if( IsWaitingResult() ){
            setPhase(PHASE_JUDGE);
        }
        break;
    case PHASE_JUDGE:
        {
            TbFloat32 y = GetPosY();
            if( m_PrizeTargetY < y ){
                y -= (CMN_WINDOW_HEIGHT/10.0f) * deltatime;
            }
            if( y <= m_PrizeTargetY ){
                y = m_PrizeTargetY;
                setPhase(m_Phase+1);
            }
            SetPos(0.0f,y);
            
        }
        break;
    case PHASE_JUDGE+1:
        {
            if( 0.5f < m_PhaseSec  )
            {
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
                TbUint32 playerCount = stage->GetPlayerCount();
                
                // 一位を決める
                TbFloat32 targetY = CMN_WINDOW_HEIGHT;
                TbSint32 result = -1;
                for( TbUint32 i = 0; i < playerCount; ++i )
                {
                    SideActionPlayer* player = stage->GetPlayer(i);
                    const TbVector4& pos = player->GetPos();
                    if( pos.GetY() < targetY ){
                        targetY = pos.GetY();
                        result = i;
                    }else if( TB_FLOAT32_EQUAL(pos.GetY(),targetY) ){
                        result = -1;
                    }
                }
                SetPrizePlayerIndex( result );
                setPhase(m_Phase+1);
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
TbSint32 SideActionJudgeHeight::NotifyEvent( TbUint32 event , void* param )
{
    Super::NotifyEvent(event,param);
    if( SIDE_ACTION_EVENT_SWITCH_ON != event ) {
        return 0;
    }
    // プレイヤーの動きを止める
    GetStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_SUSPEND,nullptr);
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    // 一位を決める
    static const TbFloat32 OFFSET_Y = 10.0f;
    static const TbFloat32 HELMENT_OFFSET_Y = -5.0f;
    for( TbUint32 i = 0; i < playerCount; ++i )
    {
        SideActionPlayer* player = stage->GetPlayer(i);
        const TbVector2& pos = player->GetHeadPos();
        if( pos.GetY() < m_PrizeTargetY ){
            m_PrizeTargetY = pos.GetY();
        }
    }
    m_PrizeTargetY += OFFSET_Y;
    if (SaveFile::GetInstance().IsVisiblePlayerNumber())
    {
        m_PrizeTargetY += HELMENT_OFFSET_Y;
    }
    m_PrizeTargetY *= stage->GetScale();
    SetPos(0.0f,CMN_WINDOW_HEIGHT);
    setPhase(PHASE_TIMEUP);
    return 0;
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionJudgeHeight::onDraw( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    static const TbRectF32 LINE_RECT(0.0f,0.0f,CMN_WINDOW_WIDTH,5.0f);
    static const TbRectF32 LINE_UV_RECT(CMN_TEX_UV_RECT(496,0,16,16));

    if( !IsActive() ) {
        return;
    }

    if( PHASE_JUDGE <= m_Phase ){
        const TbVector2& drawPos = GetDrawPos();
        TbMatrix44 pose;
        pose.SetTranslation(drawPos.GetX(),drawPos.GetY(),0.0f);
        material->Draw(LINE_RECT,LINE_UV_RECT,CmnUtil::GetMainColor(),pose);
    }

}
