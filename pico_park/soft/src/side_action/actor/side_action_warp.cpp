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
    , m_Counter(0)
{
    TbRectF32 rect(0.0f,0.0f,width,height);
    CrActionCollision* collision = CreateCollision( rect , 0 , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
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
void SideActionWarp::onCollided(CrActionCollision* target)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            TbUint32 playerCount = stage->GetPlayerCount();
            if( m_IsDestInitPos )
            {
                if (m_IsTargetAll) {
                    GetActionStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_WARP_INIT_POS, nullptr);
                }else {
                    SideActionPlayer* player = static_cast<SideActionPlayer*>(target->GetActor());
                    player->NotifyEvent(SIDE_ACTION_EVENT_WARP_INIT_POS, nullptr);
                }
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
                    if (stage) {
                        TbVector2 dst = dest;
                        for (TbUint32 i = 0; i < playerCount; ++i) {
                            dst += m_DestOffset * i;
                            stage->GetPlayer(i)->NotifyEvent(SIDE_ACTION_EVENT_WARP, &dst );
                        }
                    }
                } else {
                    dest += m_DestOffset * m_Counter;
                    player->NotifyEvent(SIDE_ACTION_EVENT_WARP, &dest);
                    m_Counter = (m_Counter++) % playerCount;
                }
            }
        }
        break;
    case SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT:
        {
            SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
            if (m_IsDestInitPos) {
                if (m_IsTargetAll) {
                    GetActionStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_WARP_INIT_POS, nullptr);
                }
                else {
                    target->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_WARP_INIT_POS, nullptr);
                }
            } else {
                target->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_WARP, &m_Dest);
            }

        }
        break;
    }
}
