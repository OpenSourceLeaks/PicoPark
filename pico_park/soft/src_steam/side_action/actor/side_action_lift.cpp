/*!
 * 踏むと倒れるエネミー
 * @author teco
 */

#include "fwd.h"
#include "side_action_lift.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -98.0f , -2.0f , 196.0f , 20.0f
};
static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(351,384.5,98,10)
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+3 , SPRITE_RECT.width-2.0f , 20.0f-2.0f 
};

/*!
 * コンストラクタ
 * @author teco
 */
SideActionLift::SideActionLift()
    : Super()
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetFixedPos(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionLift::~SideActionLift()
{
}
