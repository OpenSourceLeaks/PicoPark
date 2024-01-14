/*!
 * 踏むと倒れるエネミー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_updown_enemy.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_anim_data.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -30.0f , -26.0f , 60.0f , 52.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+2 , SPRITE_RECT.y+6 , SPRITE_RECT.width-4 , SPRITE_RECT.height-6
};

static TB_NDEBUG_CONST TbFloat32 UPDOWN_TIME = 2.5f;
static TB_NDEBUG_CONST TbFloat32 WAIT_DOWN_TIME = 2.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 2.0f;
static TB_NDEBUG_CONST TbFloat32 HEIGHT = 50.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionUpDownEnemy::SideActionUpDownEnemy()
    : Super()
    , m_State(STATE_BEGIN)
    , m_OffsetY(0.0f)
    , m_Timer(0.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SideActionAnimData::UPDOWN_ENEMY_UV; //
    m_SpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        animInitParam.param.name = "look";
        animInitParam.param.uvOffsetAnim = &SideActionAnimData::UPDOWN_ENEMY_LOOK;
        m_SpriteAnim.Initialize(1,&animInitParam);
    }
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(&m_SpriteNode,&m_SpriteAnim);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    TbUint32 pushBackTarget = 0;
    ActionCollision* collision = CreateCollision( SPRITE_RECT , pushBackTarget , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_ENEMY);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionUpDownEnemy::~SideActionUpDownEnemy()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionUpDownEnemy::OnUpdateAction( TbFloat32 deltatime )
{
    switch( m_State )
    {
    case STATE_WAIT_DOWN:
        {
            if( m_Timer < WAIT_DOWN_TIME ) {
                m_Timer += deltatime;
            }else{
                setState(STATE_UP);
            }
        }
        break;
    case STATE_UP:
        if( m_Timer < UPDOWN_TIME ) {
            m_Timer += deltatime;
            TbFloat32 rate = m_Timer / UPDOWN_TIME;
            m_OffsetY = rate * -HEIGHT;
        }else{
            m_OffsetY = -HEIGHT;
            setState(STATE_WAIT_UP);
        }
        break;
    case STATE_WAIT_UP:
        if( m_Timer < WAIT_TIME ) {
            m_Timer += deltatime;
        }else{
            setState(STATE_DOWN);
        }
        break;
    case STATE_DOWN:
        if( m_Timer < UPDOWN_TIME ) {
            m_Timer += deltatime;
            TbFloat32 rate = m_Timer / UPDOWN_TIME;
            m_OffsetY = (1-rate) * -HEIGHT;
        }else{
            m_OffsetY = 0.0f;
            setState(STATE_WAIT_DOWN);
        }
        break;
    }
    TbVector2 pos = GetInitPos();
    pos.Add(0.0f,m_OffsetY);
    SetPos( pos.GetX() , pos.GetY() );
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionUpDownEnemy::onCollided(ActionCollision* target)
{
    if( target->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER ){
        ActionActor* actor = static_cast<ActionActor*>(target->GetActor());
        if( actor ) {
            actor->NotifyEvent( SIDE_ACTION_EVENT_DEAD , NULL );
        }
    }
}
