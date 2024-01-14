/*!
 * 踏むと倒れるエネミー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_updown_lift.h"
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"

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
static TB_NDEBUG_CONST TbFloat32 HEIGHT = -70.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 0.06f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionUpDownLift::SideActionUpDownLift()
    : Super()
    , m_OldTimer(0.0f)
    , m_Timer(0.0f)
    , m_PushBackTimer(0.0f)
    , m_Height(HEIGHT)
    , m_Speed(1.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetFixedPos(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionUpDownLift::~SideActionUpDownLift()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionUpDownLift::OnUpdateAction( TbFloat32 deltatime )
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
        m_PushBackTimer += deltatime;
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
    m_Timer += deltatime*m_Speed;
    if( m_Timer > INTERVAL_SEC ) {
        m_Timer -= INTERVAL_SEC;
    }
    TbFloat32 rate = m_Timer/INTERVAL_SEC;
    TbFloat32 y  = TbSin(rate*2*TB_PI)*m_Height;
    TbVector2 mov( 0.0f ,  y - m_OffsetPos.GetY() );

    if( IsEnableMoveContactActor(mov,CR_CONTACT_UP) ){
        m_OffsetPos.SetY(y);
        SetPos( GetInitPos().GetX() + m_OffsetPos.GetX() , 
                GetInitPos().GetY() + m_OffsetPos.GetY() );

        // 上に乗っかっているアクターは連動して動かす
        MoveContactActor( mov , CR_CONTACT_UP );
    }else{
        m_BitArray.SetBit(FLAG_PUSH_BACK);
    }
}
