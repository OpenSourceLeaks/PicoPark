/*!
 * 歩行エネミー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_step_enemy.h"
#include "side_action/actor/side_action_anim_data.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -25.0f , -14.0f , 50.0f , 28.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};


static TbFloat32 SPEED = 1.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionStepEnemy::SideActionStepEnemy()
    : Super()
    , m_State(STATE_DEFAULT)
    , m_Dir(1.0f,0.0f)
{
    // スプライト設定
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SideActionAnimData::STEP_ENEMY_UV;
    m_SpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        animInitParam.param.name = "walk";
        animInitParam.param.uvOffsetAnim = &SideActionAnimData::STEP_ENEMY_WALK;
        m_SpriteAnim.Initialize(1,&animInitParam);
    }
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(&m_SpriteNode,&m_SpriteAnim);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_ALL;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_ENEMY);
    collision->SetContactAddedCallback( TbCreateMemFunc(this,&Self::onContactAdded) );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionStepEnemy::~SideActionStepEnemy()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionStepEnemy::OnUpdateAction( TbFloat32 deltatime )
{
    // 接しているオブジェクトが押してきていたら
    const ActionCollision* collision = GetCollision();
    TB_RESQUE(collision);

    SetVeloX( m_Dir.GetX() * SPEED );

    // 自由落下
    if( !collision->IsContacted(CMN_CONTACT_DOWN) ) {
        AddVelo(0.0f,SIDE_ACTION_GRAVITY);
    }else{
        SetVeloY(0.0f);
    }
}

/*! 
 * 接触開始コールバック
 * @author teco
 * @since 2013.10.14
 */
void SideActionStepEnemy::onContactAdded(const TbVector2& normal,
                                     ActionCollision::ContactTarget target,
                                     const ActionCollisionInfo& info)
{
    TbSprite& sprite = GetMainSprite();
    switch( target )
    {
    case ActionCollision::CONTACT_TARGET_MAP:
        if( !TB_FLOAT32_EQUAL( normal.GetX() , 0.0f ) ){
            m_Dir *= -1;
            sprite.SetScale(-sprite.GetScale().GetX(),1.0f);
        }
        break;
    case ActionCollision::CONTACT_TARGET_COLLISION:
        {
            TB_RESQUE(info.collision);
            ActionActor* actor =  static_cast<ActionActor*>(info.collision->GetActor());
            TB_RESQUE(actor);
            
            if( normal.GetY() < 0.0f ){
                // JUMPイベント通知
                actor->NotifyEvent( SIDE_ACTION_EVENT_JUMP , nullptr );
            }else if( !TB_FLOAT32_EQUAL( normal.GetX() , 0.0f ) ){
                if( GetVelo().GetX() * actor->GetVelo().GetX() <= 0.0f ){
                    m_Dir *= -1;
                    sprite.SetScale(-sprite.GetScale().GetX(),1.0f);
                }
                if( info.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER ){
                    actor->NotifyEvent( SIDE_ACTION_EVENT_DEAD , NULL );
                }
            }
        }
        break;
    }
}

