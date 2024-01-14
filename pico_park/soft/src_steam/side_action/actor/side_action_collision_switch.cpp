/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/actor/side_action_collision_switch.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCollisionSwitch::SideActionCollisionSwitch( TbFloat32 width , TbFloat32 height )
    : Super()
    , m_IsEnd(TB_FALSE)
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
SideActionCollisionSwitch::~SideActionCollisionSwitch()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionCollisionSwitch::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * 描画
 * @author teco
 * @since 2013.09.19
 */
void SideActionCollisionSwitch::OnDraw( TbFloat32 deltatime )
{
#if 0
    TbDraw2dBegin();
    TbDrawRectParam param;
    param.SetColor(TbColor::Red());
    TbRectF32 rect = GetCollision()->GetRect();
    TbVector2 pos = GetDrawPos();
    param.SetRect(pos.GetX()+rect.x,pos.GetY()+rect.y,rect.width,rect.height);
    TbDrawRect(param);
    TbDraw2dEnd();
#endif
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionCollisionSwitch::onCollided(ActionCollision* target)
{
    if( m_IsEnd ) {
        return;
    }
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            if( !m_TargetName.IsEmpty() ){
                GetStage()->NotifyEvent(m_TargetName.GetBuffer(),SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
            }
        }
        break;
    }
    m_IsEnd = TB_TRUE;
}
