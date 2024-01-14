/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/actor/side_action_warp.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionWarp::SideActionWarp( TbFloat32 width , TbFloat32 height )
    : Super()
    , m_IsDestInitPos(TB_FALSE)
    , m_IsTargetAll(TB_FALSE)
{
    TbRectF32 rect(0.0f,0.0f,width,height);
    ActionCollision* collision = CreateCollision( rect , 0 , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionWarp::~SideActionWarp()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionWarp::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionWarp::onCollided(ActionCollision* target)
{
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            if(m_IsDestInitPos )
            {
                GetActionStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_WARP_INIT_POS,nullptr);
            }
            else
            {
                SideActionPlayer* player = static_cast<SideActionPlayer*>(target->GetActor());
                TbVector2 dest = m_Dest;
                if( TB_FLOAT32_EQUAL_ZERO(dest.LengthSq()) ){
                    dest.SetX( player->GetPosX());
                    dest.SetY( player->GetPosY());
                }else{
                    if (TB_FLOAT32_EQUAL_ZERO(dest.GetX())) {
                        dest.SetX(player->GetPosX());
                    }
                }
                if (m_IsTargetAll) {
                    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
                    if (stage) {
                        TbUint32 count = stage->GetPlayerCount();
                        TbVector2 dst = dest;
                        for (TbUint32 i = 0; i < count; ++i) {
                            dst += m_DestOffset * i;
                            stage->GetPlayer(i)->NotifyEvent(SIDE_ACTION_EVENT_WARP, &dst );
                        }
                    }
                }
                else {
                    dest += m_DestOffset * static_cast<TbFloat32>(player->GetPlayerIndex());
                    player->NotifyEvent(SIDE_ACTION_EVENT_WARP, &dest);
                }
            }
        }
        break;
    }
}
