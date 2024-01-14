/*!
 * タイトル
 * @author teco
 */

#include "fwd.h"
#include "side_action_logo.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionLogo::SideActionLogo()
{
    static const TbRectF32Pod SPRITE_RECT =
    {
        -29 , -10 , 58 , 20 ,
    };
    static const TbRectF32Pod SPRITE_UV =
    {
        CMN_TEX_UV_RECT(320,320,29,10) ,
    };
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT, SPRITE_UV);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_3);
};

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionLogo::~SideActionLogo()
{
}

/*! 
 * セットアップ
 * @author teco
 */
void SideActionLogo::Setup(const CrActorFactory::UserData& userData)
{
    if ( 0 < userData.GetCount() )
    {
        TbFloat32 scale = userData.AsFloat32(0);
        TbSprite& sprite = GetMainSprite();
        sprite.SetScale(scale);
    }
}