/*!
 * キー代理ゴール
 * @author teco
 */

#include "fwd.h"
#include "side_action_key_proxy_goal.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -32.0f , -64.0f , 64.0f , 64.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -24.0f , -64.0f , 48.0f , 64.0f
};

static const TbRectF32Pod PUSHBACK_COLLISION_RECT[] = 
{
    { -35.0f , -64.0f , 1.0f , 64.0f } ,
    {  34.0f , -64.0f , 1.0f , 64.0f } ,
    {  -25.0f , -10.0f , 50.0f , 10.0f } ,
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(48.0f,368.0f,32.0f, 32.0f) ,
};

static const TbRectF32Pod ARROW_SPRITE_RECT = 
{
    -15.0f , -15.0f , 30.0f , 30.0f
};

static const TbRectF32Pod ARROW_SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(480,96,15,15)
};


/*!
 * コンストラクタ
 * @author teco
 */
SideActionKeyProxyGoal::SideActionKeyProxyGoal()
    : Super()
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    // 方向
    AddComponent(&m_ArrowSprite);
    m_ArrowSprite.GetSprite().Initialize(ARROW_SPRITE_RECT,ARROW_SPRITE_UV_RECT);
    m_ArrowSprite.SetLocalPos(TbVector2(0.0f,-100.0f));
    m_ArrowSprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_2);
    m_ArrowSprite.GetSprite().SetRotation(TbDegToAng32(-90.0f));

    // コリジョン
    TbUint32 pushBackTarget = 0;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_GOAL);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }

    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(PUSHBACK_COLLISION_RECT); ++i )
    {
        pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
        ActionCollision* collision = CreateCollision( PUSHBACK_COLLISION_RECT[i] , pushBackTarget , TB_TRUE );
        if( TB_VERIFY(collision)) {
            collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
        }
    }
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionKeyProxyGoal::~SideActionKeyProxyGoal()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionKeyProxyGoal::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 */
void SideActionKeyProxyGoal::onCollided(ActionCollision* target)
{
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_KEY:
        {
            TbSint32 param = 0;
            GetStage()->NotifyEvent("Goal",SIDE_ACTION_EVENT_SWITCH_ON,&param);
        }
        break;
    }
}
