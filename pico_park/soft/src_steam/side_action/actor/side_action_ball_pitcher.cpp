/*!
 * バウンドボールピッチャー
 * @author teco
 * @since 2015.05.25
 */

#include "fwd.h"
#include "side_action_ball_pitcher.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action_bound_ball.h"
#include "side_action_laser_ball.h"
#include "side_action_physics_ball.h"

#include "side_action/side_action_types.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -23.0f , -38.0f , 46.0f , 58.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(240,400,23,29),
};

static const TbRectF32Pod BASE_SPRITE_RECT = 
{
    -25.0f , -4.0f , 52.0f , 40.0f
};

static const TbRectF32Pod BASE_SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(240,432,26,20),
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -40.0f , -18.0f , 54.0f , 40.0f
};

static const TbFloat32 CREATE_TIME = 0.5f;
static const TbFloat32 START_OFFSET = 20.0f;
static TB_NDEBUG_CONST TbFloat32 SPEED = 5.0f;
static const TbVector2 INIT_DIR(0.0f,-1.0f);

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBallPitcher::SideActionBallPitcher( Type type , TbFloat32 rotDeg )
    : Super()
    , m_Type(type)
    , m_Ball(nullptr)
    , m_CreateTimer(CREATE_TIME)
    , m_Speed(SPEED)
    , m_Dir(INIT_DIR)
{
    // スプライト
    {
        TbSprite& sprite = GetMainSprite();
        sprite.SetZ(CMN_SPRITE_Z_FRONT_3);
        sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
        sprite.SetRotation(TbDegToAng32(rotDeg));
    }

    // 土台
    if( rotDeg < 170.0f || 190.0f < rotDeg  ) {
        AddComponent(&m_BaseSprite);
        TbSprite& sprite = m_BaseSprite.GetSprite();
        sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
        sprite.Initialize(BASE_SPRITE_RECT,BASE_SPRITE_UV_RECT);
    }
    
    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);

    m_Dir.Rotate(TbDegToAng32(rotDeg));
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionBallPitcher::~SideActionBallPitcher()
{
    TB_SAFE_RELEASE(m_Ball);
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBallPitcher::OnUpdateAction( TbFloat32 deltatime )
{
#if 0
    TbAngle32 angle = GetSprite().GetRotation();
    angle += TbDegToAng32(1.0f);
    if( angle >= TB_PI_ANG32*2 ){
        angle = 0;
    }
    GetSprite().SetRotation(angle);
    m_Dir = INIT_DIR;
    m_Dir.Rotate(angle);
#endif

    if( !m_Ball )
    {
        m_CreateTimer -= deltatime;
        if( m_CreateTimer <= 0.0f ){
            TbVector2 initPos = GetInitPos();
            initPos += m_Dir * START_OFFSET;
            ActionStage* stage = GetActionStage();
            TbActorScene* scene = &stage->GetActorScene();

            switch( m_Type )
            {
            default:
                TB_ASSERT(0);
            case TYPE_BOUND:
                {
                    m_Ball = new SideActionBoundBall();
                }
                break;
            case TYPE_LASER:
                {
                    m_Ball = new SideActionLaserBall();
                }
                break;
            case TYPE_PHYSICS:
                {
                    m_Ball = new SideActionPhysicsBall();
                }
                break;
            }
            m_Ball->SetVelo( m_Dir * m_Speed );
            m_Ball->SetStage(stage);
            m_Ball->SetInitPos( initPos );
            m_Ball->Enter( scene , stage );
            m_CreateTimer = CREATE_TIME;
        }
    }
    else
    {
        if( !m_Ball->IsEntered() ){
            TB_SAFE_RELEASE(m_Ball);
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBallPitcher::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_LEVEL_UP:
        if( param ){
            TbFloat32 scale = *static_cast<TbFloat32*>(param);
            m_Speed *= scale;
        }else{
            m_Speed *= 2.0f;
        }
        break;
    }
    return 0;
}
