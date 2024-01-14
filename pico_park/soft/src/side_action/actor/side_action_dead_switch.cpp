/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_dead_switch.h"
#include "side_action/side_action_types.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"
#include <sound/tb_sound.h>

static const TbRectF32Pod SPRITE_RECT = 
{
    -72.0f , -54.0f , 144.0f , 54.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(368,320,48,18)
};


/*!
 * コンストラクタ
 * @author teco
 */
SideActionDeadSwitch::SideActionDeadSwitch()
    : Super(SideActionSwitch::TYPE_NORMAL)
{
    AddComponent(&m_WarnSprite);
    m_WarnSprite.GetSprite().SetZ(CMN_SPRITE_Z_BACK_1);
    m_WarnSprite.GetSprite().Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    m_WarnSprite.SetLocalPos(TbVector2(0.0f,-26.0f));
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionDeadSwitch::~SideActionDeadSwitch()
{
}

/*!
 * スイッチ状態変化
 * @author teco
 */
void SideActionDeadSwitch::OnChangeSwitchState( TbBool isOn )
{
    if( isOn )
    {
        // プレイヤー全員死亡
         Root::GetInstance().Flash();
         TbPlaySound("hit");
         SideActionStage* stage = static_cast<SideActionStage*>(GetActionStage());
         TbUint32 count = stage->GetPlayerCount();
         for( TbUint32 i = 0; i < count; ++i )
         {
             SideActionPlayer* player = stage->GetPlayer(i);
             player->NotifyEvent(SIDE_ACTION_EVENT_DEAD,nullptr);
         }
    }
}
