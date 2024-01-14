/*!
 * 幽霊
 * @author teco
 */

#include "fwd.h"
#include "side_action_ghost.h"
#include "side_action/side_action_types.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -25.0f , 0.0f , 50.0f , 58.0f
};
static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(464,384,25,29)
};
static const TbRectF32Pod SPRITE_UV_RECT_ACIVE = 
{
    CMN_TEX_UV_RECT(464,288,25,29)
};

static const TbRectF32Pod BALLOON_SPRITE_RECT = 
{
    -32.0f , -64.0f , 64.0f , 64.0f
};
static const TbRectF32Pod BALLOON_SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(464,256,32,32)
};

static const TbRectF32Pod DIRECTION_SPRITE_RECT = 
{
    -15.0f , -15.0f , 30.0f , 30.0f
};
static const TbRectF32Pod DIRECTION_SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(480,96,15,15)
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+4.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-8.0f , SPRITE_RECT.height-2.0f 
};

/*!
 * コンストラクタ
 * @author teco
 */
SideActionGhost::SideActionGhost()
    : m_MoveBorder(0)
    , m_LookingPlayerCount(0)
    , m_Sec(0.0f)
    , m_Direction( DIRECTION_UP )
    , m_DirectionTimer(0.0f)
    , m_Speed(1.3f)
    , m_Alpha(1.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // 吹き出し
    AddComponent(&m_BalloonSprite);
    m_BalloonSprite.GetSprite().Initialize(BALLOON_SPRITE_RECT,BALLOON_SPRITE_UV_RECT);
    m_BalloonSprite.SetLocalPos(TbVector2(0.0f,-2.0f));
    m_BalloonSprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_1);

    // 方向
    AddComponent(&m_DirectionSprite);
    m_DirectionSprite.GetSprite().Initialize(DIRECTION_SPRITE_RECT,DIRECTION_SPRITE_UV_RECT);
    m_DirectionSprite.SetLocalPos(TbVector2(0.0f,-42.0f));
    m_DirectionSprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_2);

    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_KEY_TARGET);

    setDirection( m_Direction );
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionGhost::~SideActionGhost()
{
}

/*!
 * スイッチターゲット設定
 * @author Miyake teco
 */
void SideActionGhost::SetSwitchTarget( const char* targetName )
{
}

/*!
 * ロジック更新
 * @author Miyake teco
 */
void SideActionGhost::OnUpdateAction( TbFloat32 deltaSec )
{
    static const TbFloat32 AMPTITUDE_SPEED = 2.0f;
    static const TbFloat32 AMPTITUDE = 8.0f;
    if( m_BitArray.Test(FLAG_END) ) {
        return;
    }

    m_Sec += deltaSec;
    TbFloat32 rad = m_Sec*AMPTITUDE_SPEED;
    TbFloat32 y = TbSin(rad)*AMPTITUDE;
    if( TB_PI_RAD*2.0f <= rad ) {
        m_Sec = (rad-TB_PI_RAD*2.0f)/AMPTITUDE_SPEED;
    }
    SetOffsetDrawPos(TbVector2(0.0f,y));

    m_LookingPlayerCount = getLookingPlayerCount();

    if( m_BitArray.Test(FLAG_ACTIVE) )
    {
        if( m_MoveBorder <= m_LookingPlayerCount  ){
            setActive(TB_FALSE);
        }
    }
    else
    {
        if( m_LookingPlayerCount == 0 ){
            setActive(TB_TRUE);
        }
    }

    if( m_BitArray.Test(FLAG_ACTIVE) )
    {
        static const TbVector2 VELO_TABLE[] = 
        {
            TbVector2(0.0f,-1.0f) , 
            TbVector2(1.0f,0.0f) , 
            TbVector2(0.0f,1.0f) , 
            TbVector2(-1.0f,0.0f) , 
        };
        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(VELO_TABLE)==DIRECTION_MAX);
        // 移動
        SetVelo(VELO_TABLE[m_Direction]*m_Speed);
        m_DirectionTimer = 0.0f;
    }
    else
    {
        static const TbFloat32 DIR_SEC = 1.0f;
        // 方向変換
        ResetVelo();
        m_DirectionTimer += deltaSec;
        if( DIR_SEC <= m_DirectionTimer ){
            setDirection( static_cast<Direction>((m_Direction+1)%DIRECTION_MAX) );
            m_DirectionTimer = 0.0f;
        }
    }


    if( m_BitArray.Test(FLAG_DEAD) )
    {
        m_Alpha -= 0.1f;
        if( 0.0f <= m_Alpha ) {
            setAlpha(m_Alpha);
        }else{
            Exit();
        }
    }
    else
    {
        // 画面外に消えたら終了
        if( (CMN_WINDOW_HEIGHT < GetPosY()) ||
            (GetPosY() < -(SPRITE_RECT.y+SPRITE_RECT.height)) ||
            (CMN_WINDOW_WIDTH < GetPosX()) ||
            (GetPosX() < -(SPRITE_RECT.x+SPRITE_RECT.width)) )
        {
            SetVelo(TbVector2(0.0f,0.0f));
            m_BitArray.SetBit(FLAG_END);

            // リトライ
            GetStage()->RequestRetry();
        }
    }
}

/*!
 * 自分を見ているプレイヤー数取得
 * @author teco
 */
TbUint32 SideActionGhost::getLookingPlayerCount() const
{
    TbUint32 result = 0;
    TbVector4 pos = GetPos();
    const SideActionStage* stage = TbDynamicCast<const SideActionStage*>(GetStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    for( TbUint32 i = 0; i < playerCount; ++i ) {
        const SideActionPlayer* player = stage->GetPlayer(i);
        TbVector4 delta =  player->GetPos() - pos;
        if( !TB_FLOAT32_EQUAL_ZERO(delta.GetX()) ){
            if( 0.0f <= delta.GetX() ) {
                if( !player->IsLookingRight() ) {
                    ++result;
                }
            }else{
                if( player->IsLookingRight() ) {
                    ++result;
                }
            }
        }
    }
    return result;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionGhost::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_SWITCH_ON:
        {
            m_BitArray.SetBit(FLAG_SWITCH);
        }
        break;
    case SIDE_ACTION_EVENT_KEY_END:
        {
            m_BitArray.SetBit(FLAG_DEAD);
        }
        break;
    }
    return 0;
}

/*!
 * 方向設定
 * @author teco
 */
void SideActionGhost::setDirection( Direction dir )
{
    static const TbAngle32 ROT_TABLE[] = 
    {
        TbDegToAng32(90.0f) , 
        TbDegToAng32(180.0f) , 
        TbDegToAng32(-90.0f) , 
        TbDegToAng32(0.0f) , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ROT_TABLE)==DIRECTION_MAX);
    m_Direction = dir;
    m_DirectionSprite.GetSprite().SetRotation(ROT_TABLE[dir]);
}

/*!
 * 有効設定
 * @author teco
 */
void SideActionGhost::setActive( TbBool isActive )
{
    if( TB_BOOL_NOT_EQUAL(m_BitArray.Test(FLAG_ACTIVE),isActive) ) {
        m_BitArray.SetBit(FLAG_ACTIVE,isActive);
        if( isActive ) {
            GetMainSprite().SetUV(SPRITE_UV_RECT_ACIVE);
            if( DIRECTION_RIGHT == m_Direction ){
                LookRight();
            }else if( DIRECTION_LEFT == m_Direction ){
                LookLeft();
            }
        }else{
            GetMainSprite().SetUV(SPRITE_UV_RECT);
        }
    }
}

/*!
 * 透明度設定
 * @author teco
 */
void SideActionGhost::setAlpha( TbFloat32 alpha )
{
    GetMainSprite().SetAlpha(alpha);
    m_BalloonSprite.GetSprite().SetAlpha(alpha);
    m_DirectionSprite.GetSprite().SetAlpha(alpha);
}
