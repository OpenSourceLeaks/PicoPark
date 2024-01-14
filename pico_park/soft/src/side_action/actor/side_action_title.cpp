/*!
 * タイトル
 * @author teco
 */

#include "fwd.h"
#include "side_action_title.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionTitle::SideActionTitle()
{
    static const TbRectF32Pod RECT = 
    {
        -330 , -80 , 660 , 160
    };
    static const TbRectF32Pod COLLISION_RECT = 
    {
        -310 , 48 , 620 , 10
    };
    static const TbRectF32Pod UV_RECT = 
    {
        CMN_TEX_UV_RECT(0,912,464,112)
    };
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(RECT,UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_3);

    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_ENEMY);
    }
};

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionTitle::~SideActionTitle()
{
}

/*! 
 * セットアップ
 * @author teco
 */
void SideActionTitle::Setup(const CrActorFactory::UserData& userData)
{
    if ( 0 < userData.GetCount() )
    {
        TbBool isActiveCollision =  userData.AsSint32(0);
        CrActionCollision* collision = GetCollision();
        if (TB_VERIFY(collision)) {
            collision->SetActive(isActiveCollision);
        }
    }
}