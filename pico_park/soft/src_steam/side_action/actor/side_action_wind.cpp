/*!
 * 点滅ブロック
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_wind.h"
#include "side_action/side_action_types.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_anim_data.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -32.0f , -76.0f , 64.0f , 76.0f
};
static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(464,330,32,38) ,
};
static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1.0f , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};

static const TbRectF32Pod SUB_SPRITE_RECT = 
{
    -64.0f , -68.0f , 24.0f , 34.0f
};

static const TbRectF32Pod WIND_COLLISION_RECT = 
{
    -CMN_WINDOW_WIDTH , -60.0f , CMN_WINDOW_WIDTH , 4.0f
};


/*!
 * コンストラクタ
 * @author teco
 */
SideActionWind::SideActionWind()
    : m_Power(0.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    AddComponent(&m_SubSprite);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // サブスプライト
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SUB_SPRITE_RECT; //
    initParam.param.uvRect = SideActionAnimData::WIND_UV;
    m_SubSpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        animInitParam.param.name = "default";
        animInitParam.param.uvOffsetAnim = &SideActionAnimData::WIND_ANIM;
        m_SubSpriteAnim.Initialize(1,&animInitParam);
    }
    TbSprite& subSprite = m_SubSprite.GetSprite();
    subSprite.Initialize(&m_SubSpriteNode,&m_SubSpriteAnim);
    subSprite.SetEnableLoop(TB_TRUE);
    subSprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    {
        TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
        ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    }

    // 風とのあたりはんてい
    {
        ActionCollision* windCollision = CreateCollision( WIND_COLLISION_RECT , 0 , TB_TRUE );
        windCollision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided) );
        windCollision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
    }
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionWind::~SideActionWind()
{
}

/*!
 * シーンに入った
 * @author teco
 */

void SideActionWind::OnEnter( void* userData )
{
    Super::OnEnter(userData);
}
    
/*!
 * シーンから出た
 * @author teco
 */
void SideActionWind::OnExit()
{
    Super::OnExit();
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 */
void SideActionWind::onCollided(ActionCollision* target)
{
    target->GetActor()->SetWind(TbVector2(m_Power,0.0f));
}
