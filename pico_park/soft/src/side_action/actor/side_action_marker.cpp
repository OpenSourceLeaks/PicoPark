/*!
 * マーカー
 * @author teco
 * @since 2014.05.25
 */

#include "fwd.h"
#include "side_action_marker.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -32.0f , -64.0f , 64.0f , 64.0f
};

static const TbRectF32Pod SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(64,320,32,32),
    CMN_TEX_UV_RECT(96,320,32,32),
    CMN_TEX_UV_RECT(128,320,32,32),
};

/*!
 * コンストラクタ
 * @author teco
 */
SideActionMarker::SideActionMarker( Type type )
    : Super()
{
    TB_RESQUE(0<=type&&type<TYPE_MAX);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT[type]);
    sprite.SetZ(CMN_SPRITE_Z_BACK_3);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionMarker::~SideActionMarker()
{
}
