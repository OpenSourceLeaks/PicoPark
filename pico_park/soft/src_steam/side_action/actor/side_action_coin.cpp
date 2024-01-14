/*!
 * コイン
 * @author teco
 */

#include "fwd.h"
#include "side_action_coin.h"
#include "side_action/actor/side_action_anim_data.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include <sound/tb_sound.h>

static const TbRectF32Pod SPRITE_RECT = 
{
    -12.0f , -20.0f , 24.0f , 40.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};


static TbFloat32 SPEED = 1.0f;

//////////////////////////////////
//  SideActionCoin
//////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCoin::SideActionCoin()
    : Super()
{
    // スプライト設定
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SideActionAnimData::COIN_UV;
    m_SpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        animInitParam.param.name = "default";
        animInitParam.param.uvOffsetAnim = &SideActionAnimData::COIN_ANIM;
        m_SpriteAnim.Initialize(1,&animInitParam);
    }
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(&m_SpriteNode,&m_SpriteAnim);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // コリジョン
    ActionCollision* collision = CreateCollision( COLLISION_RECT , 0 , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
    collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided) );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCoin::~SideActionCoin()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionCoin::OnUpdateAction( TbFloat32 deltatime )
{
}

/*! 
 * 接触開始コールバック
 * @author teco
 * @since 2013.10.14
 */
void SideActionCoin::onCollided(ActionCollision* target)
{
    TbPlaySound("coin");
    Exit();
}

//////////////////////////////////
//  SideActionCoinObserver
//////////////////////////////////

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionCoinObserver::OnUpdateAction( TbFloat32 deltatime )
{
    TbBool isFinished = TB_TRUE;
    TbForIterator( ite , m_Coins ) {
        if( (*ite)->IsEntered() ){
            isFinished = TB_FALSE;
            break;
        }
    }
    if( isFinished ){
        GetStage()->NotifyEvent( "Key" , SIDE_ACTION_EVENT_ACTIVATE , nullptr );
    }
}
