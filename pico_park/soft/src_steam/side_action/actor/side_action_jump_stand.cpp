/*!
 * ジャンプ台
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_jump_stand.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/actor/side_action_anim_data.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -16.0f , -40.0f, 32.0f , 42.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -16.0f , -34.0f, 32.0f , 34.0f
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJumpStand::SideActionJumpStand( TbFloat32 jumpVelo )
    : Super()
    , m_JumpVelo()
{
    m_JumpVelo.SetY(jumpVelo);
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SideActionAnimData::JUMP_STAND_UV; //
    m_SpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        animInitParam.param.name = "action";
        animInitParam.param.uvOffsetAnim = &SideActionAnimData::JUMP_STAND_ACTION;
        m_SpriteAnim.Initialize(1,&animInitParam);
    }
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(&m_SpriteNode,&m_SpriteAnim);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_ALL;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetContactAddedCallback( TbCreateMemFunc(this,&Self::onContactAdded) );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJumpStand::~SideActionJumpStand()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionJumpStand::OnUpdateAction( TbFloat32 deltatime )
{
}


/*! 
 * 接触開始コールバック
 * @author teco
 * @since 2013.10.14
 */
void SideActionJumpStand::onContactAdded(const TbVector2& normal,
                                         ActionCollision::ContactTarget target,
                                         const ActionCollisionInfo& info)
{
    TbSprite& sprite = GetMainSprite();
    switch( target )
    {
    case ActionCollision::CONTACT_TARGET_COLLISION:
        {
            TB_RESQUE(info.collision);
            ActionActor* actor =  static_cast<ActionActor*>(info.collision->GetActor());
            TB_RESQUE(actor);
            
            if( normal.GetY() < 0.0f ){
                // JUMPイベント通知
                actor->NotifyEvent( SIDE_ACTION_EVENT_JUMP , &m_JumpVelo );
            }
        }
        break;
    }
}

