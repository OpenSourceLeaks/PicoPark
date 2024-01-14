/*!
 * バウンドボールピッチャー
 * @author teco
 * @since 2015.05.25
 */

#include "fwd.h"
#include "side_action_ball_pitcher.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
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
SideActionBallPitcher::SideActionBallPitcher( Type type )
    : Super()
    , m_Type(type)
    , m_Ball(nullptr)
    , m_CreateTimer(CREATE_TIME)
    , m_Speed(SPEED)
    , m_DefaultSpeed(SPEED)
    , m_Scale(1.0f)
    , m_Dir(INIT_DIR)
{
    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
                              CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionBallPitcher::Setup(const CrActorFactory::UserData& userData)
{
    TbFloat32 rotDeg = static_cast<TbFloat32>(userData.AsSint32(0));

    // スプライト
    {
        TbSprite& sprite = GetMainSprite();
        sprite.SetZ(CMN_SPRITE_Z_FRONT_3);
        sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
        sprite.SetRotation(TbDegToAng32(rotDeg));
    }

    // 土台
    if (rotDeg < 170.0f || 190.0f < rotDeg) {
        AddComponent(&m_BaseSprite);
        TbSprite& sprite = m_BaseSprite.GetSprite();
        sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
        sprite.Initialize(BASE_SPRITE_RECT, BASE_SPRITE_UV_RECT);
    }

    m_Dir.Rotate(TbDegToAng32(rotDeg));

    switch(m_Type)
    {
    case TYPE_BOUND:
    case TYPE_PHYSICS:
        {
            TbUint32 speedIndex = Root::GetInstance().GetPlayerCount() - 1;
            if (userData.AsSint32(1) > 0 && speedIndex > 0) {
                TbFloat32 speed = userData.AsFloat32(speedIndex)*0.1f;
                m_DefaultSpeed = speed;
                SetSpeed(speed);
            }
        }
        break;
    case TYPE_LASER:
    case TYPE_ENDLESS_LASER:
    case TYPE_DEAD_LASER:
        {
            TbFloat32 speed = userData.AsFloat32(1);
            m_DefaultSpeed = speed;
            SetSpeed(speed);
            if( 2 < userData.GetCount() ){
                m_LaserBallEventTarget = userData.AsString(2);
            }
        }
        break;
    default:
        TB_ASSERT(0);
        break;
    }
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
            TbFloat32 speed = m_Speed;
            m_Ball = createBall(m_Type, speed , TbVector2::ZERO );
            m_CreateTimer = CREATE_TIME;
        }
    }
    else
    {
        if( m_Ball && !m_Ball->IsEntered() ){
            TB_SAFE_RELEASE(m_Ball);
            if (!m_ManagerName.IsEmpty()) {
                GetStage()->NotifyEvent(m_ManagerName.GetBuffer(),SIDE_ACTION_EVENT_ACTIVATE,nullptr);
                Suspend();
            }
        }
    }
}


/*!
 * ボール生成
 * @author teco
 */
CrActionActor* SideActionBallPitcher::createBall( Type type, TbFloat32 speed , TbVector2 offst )
{
    TbVector2 initPos = GetInitPos();
    initPos += m_Dir * START_OFFSET;
    initPos += offst;
    CrActionStage* stage = GetActionStage();
    TbActorScene* scene = &stage->GetActorScene();
    CrActionActor* result = nullptr;
    switch (m_Type)
    {
    default:
        TB_ASSERT(0);
    case TYPE_BOUND:
        {
            result = new SideActionBoundBall();
        }
        break;
    case TYPE_LASER:
        {
            SideActionLaserBall* ball = new SideActionLaserBall();
            ball->SetEventTarget(m_LaserBallEventTarget.GetBuffer());
            result = ball;
        }
        break;
    case TYPE_ENDLESS_LASER:
        {
            SideActionLaserBall* ball = new SideActionLaserBall();
            ball->SetEventTarget(m_LaserBallEventTarget.GetBuffer());
            ball->SetType(SideActionLaserBall::TYPE_SCORE);
            ball->SetScale(m_Scale);
            result = ball;
        }
        break;
    case TYPE_DEAD_LASER:
        {
            SideActionLaserBall* ball = new SideActionLaserBall();
            ball->SetType(SideActionLaserBall::TYPE_DEAD);
            ball->SetScale(m_Scale);
            result = ball;
        }
        break;
    case TYPE_PHYSICS:
        {
            result = new SideActionPhysicsBall();
        }
        break;
    }
    result->SetVelo(m_Dir * speed);
    result->SetStage(stage);
    result->SetInitPos(initPos);
    result->Enter(scene, stage);
    return result;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBallPitcher::NotifyEvent( TbUint32 event , void* param )
{
    TbSint32 ret = 0;
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
    case SIDE_ACTION_EVENT_LEVEL_DOWN:
        m_Speed = m_DefaultSpeed;
        break;
    case SIDE_ACTION_EVENT_SPEED_SET:
        {
            TbFloat32 speed = *static_cast<TbFloat32*>(param);
            m_Speed = speed;
        }
        break;
    case SIDE_ACTION_EVENT_SUSPEND:
        Suspend();
        ret = 1;
        break;
    case SIDE_ACTION_EVENT_ACTIVATE:
        {
            char* str = reinterpret_cast<char*>(param);
            m_ManagerName = str;
            Resume();
            ret = 1;
        }
        break;
    case SIDE_ACTION_EVENT_SPEACIAL:
        {
            // 2個生成
            m_Scale = 1.6f;
        }
        break;
    case SIDE_ACTION_EVENT_SCALE:
        {
            TbFloat32* scale = reinterpret_cast<TbFloat32*>(param);
            m_Scale = *scale;
        }
        break;
    }
    return ret;
}
