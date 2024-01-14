/*!
 * リフト
 * @author teco
 */

#include "fwd.h"
#include "side_action_lift.h"
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"
#include <base/util/tb_dynamic_cast.h>

static const TbRectF32Pod SPRITE_RECT = 
{
    -60.0f , -10.0f , 120.0f , 20.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(385,49,60,10)
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};

static TB_NDEBUG_CONST TbFloat32 INTERVAL_SEC = 5.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 0.06f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionLift::SideActionLift()
    : Super()
    , m_Move()
    , m_OldTimer(0.0f)
    , m_Timer(0.0f)
    , m_PushBackTimer(0.0f)
    , m_Speed(1.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetFixedPos(TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionLift::~SideActionLift()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionLift::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        ARG_MOVE_X , 
        ARG_MOVE_Y ,
        ARG_MOVE_X_OFFSET ,
        ARG_MOVE_Y_OFFSET,
        ARG_SPEED,
        ARG_ABS ,
        ARG_SWITCH
    };
    TbSint32 x = userData.AsFloat32(ARG_MOVE_X);
    TbSint32 y = userData.AsFloat32(ARG_MOVE_Y);
    if (ARG_MOVE_X_OFFSET < userData.GetCount()) {
        x += userData.AsSint32(ARG_MOVE_X_OFFSET) * Root::GetInstance().GetPlayerCount();
    }
    if (ARG_MOVE_Y_OFFSET < userData.GetCount()) {
        y += userData.AsSint32(ARG_MOVE_Y_OFFSET) * Root::GetInstance().GetPlayerCount();
    }
    m_Move.Set(x,y);
    if (ARG_SPEED < userData.GetCount()) {
        m_Speed = userData.AsFloat32(ARG_SPEED);
    }
    if (ARG_ABS < userData.GetCount()) {
        if (userData.AsSint32(ARG_ABS)) {
            m_BitArray.SetBit(FLAG_ABS);
        }
    }
    if (ARG_SWITCH < userData.GetCount()){
        if (userData.AsSint32(ARG_SWITCH)) {
            m_BitArray.SetBit(FLAG_SWITCH);
        }
    }
    if (!m_BitArray.Test(FLAG_SWITCH)) {
        m_BitArray.SetBit(FLAG_ACTIVE);
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionLift::OnUpdateAction( TbFloat32 deltatime )
{
    const CrActionCollision* collision = GetCollision();
    TB_RESQUE(collision);
    
    // 押し戻され対応
    if( m_BitArray.Test(FLAG_PUSH_BACK) ){
        m_Timer = m_OldTimer;
        if( TB_FLOAT32_EQUAL_ZERO(m_PushBackLimitTime) ){
            TbFloat32 half = INTERVAL_SEC/2.0f;
            TbFloat32 offset = m_Timer > half ? half : 0.0f;
            TbFloat32 base = m_Timer - offset;
            m_PushBackLimitTime = (half-base)-base;
            if( m_PushBackLimitTime < 0.0f ) {
                m_PushBackLimitTime += half+INTERVAL_SEC;
            }
        }
        if (m_BitArray.Test(FLAG_ACTIVE)) {
            m_PushBackTimer += deltatime;
        }
        if( m_PushBackTimer >= m_PushBackLimitTime ) {
            m_Timer += m_PushBackLimitTime;
            m_PushBackTimer = 0.0f;
            m_PushBackLimitTime = 0.0f;
        }
    }else{
        m_PushBackTimer = 0.0f;
        m_PushBackLimitTime = 0.0f;
    }
    m_BitArray.ResetBit(FLAG_PUSH_BACK);

    // 通常移動
    m_OldTimer = m_Timer;
    if (m_BitArray.Test(FLAG_ACTIVE)) {
        m_Timer += deltatime*m_Speed;
        if (m_BitArray.Test(FLAG_SWITCH))
        {
            if (m_BitArray.Test(FLAG_ABS)) {
                if (INTERVAL_SEC/2.0f <= m_Timer) {
                    m_BitArray.ResetBit(FLAG_ACTIVE);
                    m_Timer = 0.0f;
                    m_OldTimer = 0.0f;
                }
            }
            else {
                if (INTERVAL_SEC <= m_Timer) {
                    m_BitArray.ResetBit(FLAG_ACTIVE);
                    m_Timer = 0.0f;
                    m_OldTimer = 0.0f;
                }
            }
            if (!m_BitArray.Test(FLAG_ACTIVE)) {
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
                if (stage) {
                    stage->SetEnableScrollNormalSpeedMax(TB_TRUE);
                }
            }
        }
    }
    if(INTERVAL_SEC < m_Timer ) {
        m_Timer -= INTERVAL_SEC;
    }
    TbFloat32 rate = m_Timer/INTERVAL_SEC;
    TbFloat32 rad = rate * 2 * TB_PI;
    TbFloat32 s = TbSin(rad);
    if (m_BitArray.Test(FLAG_ABS)) {
        s = TbAbs(s);
    }
    TbVector2 mov = (m_Move * s);
    TbVector2 delta = mov - m_OffsetPos;

    if( IsEnableMoveContactActor(delta,CR_CONTACT_UP) ){
        m_OffsetPos = mov;
        SetPos( GetInitPos().GetX() + m_OffsetPos.GetX() , 
                GetInitPos().GetY() + m_OffsetPos.GetY() );

        TbVector2 deltaX(delta.GetX(),0.0f);
        TbVector2 deltaY(0.0f,delta.GetY());

        // 上に乗っかっているアクターは連動して動かす
        if (!TB_FLOAT32_EQUAL_ZERO(deltaY.GetY()))
        {
            MoveContactActor(deltaY, CR_CONTACT_UP);
        }

        // 横移動は真上だけ
        MoveContactOption option;
        option.recursiveFaceIsMov = TB_TRUE;
        if (!TB_FLOAT32_EQUAL_ZERO(deltaX.GetX()))
        {
            MoveContactActor(deltaX, CR_CONTACT_UP, option);
        }
    }else{
        m_BitArray.SetBit(FLAG_PUSH_BACK);
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionLift::NotifyEvent(TbUint32 event, void* param)
{
    switch(event)
    {
    case SIDE_ACTION_EVENT_SWITCH_ON:
        {
            if( m_BitArray.Test(FLAG_SWITCH))
            {
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
                if (stage) {
                    stage->SetEnableScrollNormalSpeedMax(TB_FALSE);
                }
                m_BitArray.SetBit(FLAG_ACTIVE);
            }
        }
        break;
    }
    return 0;
}
