/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_warp_switch.h"
#include "side_action/side_action_types.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionWarpSwitch::SideActionWarpSwitch()
    : Super(SideActionSwitch::TYPE_NORMAL)
    , m_TargetCount(-1)
{
    SetEnableRepeat(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionWarpSwitch::~SideActionWarpSwitch()
{
}

/*!
 * スイッチ状態変化
 * @author teco
 */
void SideActionWarpSwitch::OnChangeSwitchState( TbBool isOn )
{
    if( isOn && !m_TargetName.IsEmpty() )
    {
        // プレイヤー全員死亡
         SideActionStage* stage = static_cast<SideActionStage*>(GetActionStage());
         if (m_TargetCount <= 0)
         {
            CrActor* actor = stage->FindActor(m_TargetName.GetBuffer());
            if (actor) {
                actor->NotifyEvent(SIDE_ACTION_EVENT_WARP_INIT_POS, nullptr);
            }
         }
         else
         {
             for( TbSint32 i = 0; i < m_TargetCount; ++i )
             {
                 TbStaticString128 name = TbStaticString128::Format("%s%d",m_TargetName.GetBuffer(),i+1);
                 CrActor* actor = stage->FindActor(m_TargetName.GetBuffer());
                 if (actor) {
                     actor->NotifyEvent(SIDE_ACTION_EVENT_WARP_INIT_POS,nullptr);
                 }
             }
         }
    }
}
