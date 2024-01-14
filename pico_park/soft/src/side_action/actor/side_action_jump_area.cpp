/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/actor/side_action_jump_area.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionJumpArea::SideActionJumpArea()
    : Super()
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionJumpArea::~SideActionJumpArea()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionJumpArea::Setup(const CrActorFactory::UserData& userData) 
{
    TbRectF32 rect(0.0f, 0.0f, userData.AsFloat32(0), userData.AsFloat32(1));
    CrActionCollision* collision = CreateCollision(rect, 0, TB_TRUE);
    if (TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
        collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
    }
    TbVector2 velo(userData.AsFloat32(2), userData.AsFloat32(3));
    SetJumpVelo(velo);
}


/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionJumpArea::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionJumpArea::onCollided(CrActionCollision* target)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            SideActionPlayer* player = static_cast<SideActionPlayer*>(target->GetActor());
            player->NotifyEvent(SIDE_ACTION_EVENT_JUMP, &m_JumpVelo);
        }
    }
}
