/*!
 * 天秤
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_balance.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -98.0f , -8.0f , 196.0f , 16.0f
};
static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(352,288,98,8)
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};

static TB_NDEBUG_CONST TbFloat32 INTERVAL_SEC = 5.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 0.06f;

////////////////////////////
// SideActionBalanceUnit
////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBalanceUnit::SideActionBalanceUnit()
    : m_TargetOffsetY(0.0f)
    , m_OffsetY(0.0f)
    , m_SpeedY(1.0f)
    , m_ContactCount(0)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
//    collision->SetFixedPos(TB_TRUE);
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBalanceUnit::OnUpdateAction( TbFloat32 deltatime )
{
    const ActionCollision* collision = GetCollision();
    TB_RESQUE(collision);

    m_ContactCount = collision->GetContactCount(CMN_CONTACT_UP,
                                                TB_TRUE,
                                                nullptr,
                                                TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER));
    
    TbFloat32 deltaY = m_TargetOffsetY - m_OffsetY;
    if( TB_FLOAT32_EQUAL_ZERO(deltaY) ){
        return;
    }

    deltaY = TbSign(deltaY) * TbMin( m_SpeedY , TbAbs(deltaY) );
    TbVector2 mov(0.0f,deltaY);
    if( IsEnableMoveContactActor(mov,CMN_CONTACT_UP) ){
        m_OffsetY = m_OffsetY + mov.GetY();
        SetPos( GetInitPos().GetX() , 
                GetInitPos().GetY() + m_OffsetY );
        // 上に乗っかっているアクターは連動して動かす
        MoveContactActor( mov , CMN_CONTACT_UP );
    }else{
        m_BitArray.SetBit(FLAG_PUSH_BACK);
    }
}


////////////////////////////
// SideActionBalance
////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBalance::SideActionBalance( TbFloat32 interval )
    : Super()
    , m_Interval(interval)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Unit); ++i ) {
        m_Unit[i] = new SideActionBalanceUnit();
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBalance::~SideActionBalance()
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Unit); ++i ) {
        TB_SAFE_RELEASE( m_Unit[i] );
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBalance::OnUpdateAction( TbFloat32 deltatime )
{
    static const TbFloat32 SPEED = 1.0f;
    static const TbFloat32 SPEED_MIN = 0.2f;

    // それぞれの上に乗っている
    TbSint32 leftCount = m_Unit[UNIT_LEFT]->GetContactCount();
    TbSint32 rightCount = m_Unit[UNIT_RIGHT]->GetContactCount();
    TbFloat32 baseCount = static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount())/2.0f;
    TbFloat32 delta = static_cast<TbFloat32>(rightCount - leftCount)/ baseCount;
    delta = TbClamp( delta , -1.0f , 1.0f );

    TbFloat32 heightMax = (m_Interval/2.0f) * TbTan(TbDegToRad(7.0f));
 
    TbFloat32 height = 0.0f;
    if( !TB_FLOAT32_EQUAL_ZERO(delta) ) {
        height = heightMax * -delta;
    }
    m_Unit[UNIT_LEFT]->SetTargetY(height);
    m_Unit[UNIT_RIGHT]->SetTargetY(-height);

#if 1
    
    TbFloat32 speed = TbMax((SPEED*TbAbs(delta)),SPEED_MIN); // SPEED*1.0f/baseCount);
    if( delta > 0.0f ) {
        // 右側の方が乗っているキャラが多い
        TbFloat32 movY =  m_Unit[UNIT_RIGHT]->GetTargetOffsetY() - m_Unit[UNIT_RIGHT]->GetOffsetY();
        if( movY < 0.0f ) {
            speed = SPEED_MIN;
        }
        movY =  m_Unit[UNIT_LEFT]->GetTargetOffsetY() - m_Unit[UNIT_LEFT]->GetOffsetY();
        if( movY > 0.0f ) {
            speed = SPEED_MIN;
        }
    }else if( delta < 0.0f ) {
        // 左側の方が乗っているキャラが多い
        TbFloat32 movY =  m_Unit[UNIT_LEFT]->GetTargetOffsetY() - m_Unit[UNIT_LEFT]->GetOffsetY();
        if( movY < 0.0f ) {
            speed = SPEED_MIN;
        }
        movY =  m_Unit[UNIT_RIGHT]->GetTargetOffsetY() - m_Unit[UNIT_RIGHT]->GetOffsetY();
        if( movY > 0.0f ) {
            speed = SPEED_MIN;
        }
    }
#else
    TbFloat32 speed = TbMax((SPEED*TbAbs(delta)),0.1f); // SPEED*1.0f/baseCount);
#endif
    m_Unit[UNIT_LEFT]->SetSpeedY(speed);
    m_Unit[UNIT_RIGHT]->SetSpeedY(speed);

    TbFloat32 leftOffsetY = m_Unit[UNIT_LEFT]->GetOffsetY();
    TbFloat32 rightOffsetY = m_Unit[UNIT_RIGHT]->GetOffsetY();
    TbFloat32 maxOffsetY = TbMax(TbAbs(leftOffsetY),TbAbs(rightOffsetY)) * TbSign(m_Unit[UNIT_RIGHT]->GetOffsetY());

    TbRadian angle = TbArcTan2( maxOffsetY , m_Interval/2.0f );
    TbStaticString32 key = GetName();
    key = key.Substr(TB_ARRAY_COUNT_OF("Balance")-1);
    GetActionStage()->NotifyEvent(key.GetBuffer(),SIDE_ACTION_EVENT_ANGLE,&angle);
}

/*!
 * 入る
 * @author teco
 */
void SideActionBalance::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    ActionStage* stage = GetActionStage();
    TbActorScene* scene = &stage->GetActorScene();

    // ユニットの位置決定
    TbVector2 pos = GetInitPos();
    pos.Add(-m_Interval/2.0f,0.0f);
    m_Unit[UNIT_LEFT]->SetInitPos(pos);
    pos = GetInitPos();
    pos.Add(m_Interval/2.0f,0.0f);
    m_Unit[UNIT_RIGHT]->SetInitPos(pos);

    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Unit); ++i ) {
        m_Unit[i]->SetStage(stage);
        m_Unit[i]->Enter(scene,userData);
    }
}
