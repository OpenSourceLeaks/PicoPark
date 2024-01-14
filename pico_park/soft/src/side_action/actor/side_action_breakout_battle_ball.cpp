/*!
 * ブロック崩し用ボール
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_breakout_battle_ball.h"
#include "side_action_judge_breakout.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"
#include "common/cmn_util.h"

#include "side_action/side_action_types.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBreakoutBattleBall::SideActionBreakoutBattleBall()
    : Super(TB_TRUE)
    , m_PlayerIndex(-1)
    , m_Type(TYPE_BREAKOUT)
{
    SetEnableDead(TB_FALSE);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionBreakoutBattleBall::~SideActionBreakoutBattleBall()
{
}

/*!
 * 対象プレイヤー設定
 * @author teco
 */
void SideActionBreakoutBattleBall::SetPlayerIndex( TbUint32 playerIndex )
{
    m_PlayerIndex = playerIndex;
    TbColor color = CmnUtil::GetPlayerColor(m_PlayerIndex) * 0.7f;
    color.SetAlphaF32(1.0f);
    SetColor( color );
}

/*!
 * イベント通知
 * @author teco
 * @since 2013.10.15
 */
TbSint32 SideActionBreakoutBattleBall::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_SUSPEND == event ) {
        Suspend();
    }
    return 0;
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBreakoutBattleBall::OnUpdateAction( TbFloat32 deltatime )
{
    Super::OnUpdateAction(deltatime);
    if( TYPE_TERRITORY == m_Type && 0 <= m_PlayerIndex )
    {
        CrActionStage* stage = GetActionStage();
        const CrActionMap& map = stage->GetMap();
        TbFloat32 chipSize = static_cast<TbFloat32>(map.GetChipSize());

        const TbVector4& pos = GetPos();
        TbSint32 x = pos.GetX()/chipSize;
        TbSint32 y = pos.GetY()/chipSize;
        SideActionJudgeBreakout::RegistInfo info;
        info.x = x;
        info.y = y;
        info.playerIndex = m_PlayerIndex;
        stage->NotifyEvent("TerritoryBallJudge",SIDE_ACTION_EVENT_REGISTER,&info);
    }
}

/*!
 * マップチップヒット
 * @author teco
 */
void SideActionBreakoutBattleBall::OnHitMapChip( TbSint32 x , TbSint32 y )
{
    CrActionStage* stage = TbDynamicCast<CrActionStage*>(GetStage());
    if( stage && 0 <= m_PlayerIndex ){
        SideActionJudgeBreakout::RegistInfo info;
        info.x = x;
        info.y = y;
        info.playerIndex = m_PlayerIndex;
        stage->NotifyEvent("BreakoutJudge",SIDE_ACTION_EVENT_REGISTER,&info);
    }
}

/*!
 * コリジョンヒット
 * @author teco
 */
void SideActionBreakoutBattleBall::OnHitCollision( CrActionCollision* collision )
{
    if( collision->GetActor() )
    {
        TbSint32 playerIndex = collision->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_GET_PLAYERINDEX,nullptr)-1;
        if( 0 <= playerIndex ){
            SetPlayerIndex(playerIndex);
        }
    }
}

