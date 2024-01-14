/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_switch.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/side_action_types.h"

static TbFloat32 RADIUS = 12.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -16.0f , -32.0f , 32.0f , 32.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -8.0f , -32.0f , 16.0f , 32.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(272,432,16,16)
};

static const TbRectF32Pod SPRITE_UV_RECT_ON = 
{
    CMN_TEX_UV_RECT(288,432,16,16)
};

static const TbCircleF32 COLLISION_CIRCLE( 0.0f , 0.0f , RADIUS );

static TB_NDEBUG_CONST TbUint32  DEAD_COUNT = 180;
static TB_NDEBUG_CONST TbUint32  ANIM_COUNT = 10;
static TB_NDEBUG_CONST TbFloat32 SCALE = 1.05f;
static TB_NDEBUG_CONST TbFloat32 FRICTION = 0.95f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionSwitch::SideActionSwitch( Type type )
    : Super()
    , m_Type(type)
    , m_Id(0)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    // コリジョン
    switch( m_Type )
    {
    case TYPE_NORMAL:
        {
            TbUint32 pushBackTarget = 0;
            ActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
            collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
            collision->SetCollidedCallback(TbCreateMemFunc(this,&Self::onCollided));
        }
        break;
    case TYPE_REVERSE:
        {
            sprite.SetRotation(TbDegToAng32(180.0f));
            sprite.AddTranslate(0.0f,SPRITE_RECT.height);

            TbUint32 pushBackTarget = 0;
            TbCircleF32 circle = COLLISION_CIRCLE;
            circle.y += COLLISION_RECT.height;
            ActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
            collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
            collision->SetCollidedCallback(TbCreateMemFunc(this,&Self::onCollided));
        }
        break;
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionSwitch::~SideActionSwitch()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionSwitch::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_BitArray.Test(FLAG_REQ_REBOUND) ){
        m_BitArray.SetBit(FLAG_REBOUND);
        m_BitArray.ResetBit(FLAG_REQ_REBOUND);
    }else{
        m_BitArray.ResetBit(FLAG_REBOUND);
    }
    if( m_BitArray.Test(FLAG_ON) ){
        GetMainSprite().SetUV(SPRITE_UV_RECT_ON);
        if( !m_BitArray.Test(FLAG_ON_PRE) ){
            changeSwitchState(TB_TRUE);
        }
    }else{
        GetMainSprite().SetUV(SPRITE_UV_RECT);
        if( m_BitArray.Test(FLAG_ON_PRE) ){
            changeSwitchState(TB_FALSE);
        }
    }
    m_BitArray.SetBit(FLAG_ON_PRE,m_BitArray.Test(FLAG_ON));

    // 繰り返し利用できるスイッチなら一度フラグをおろす
    if( m_BitArray.Test(FLAG_ENABLE_REPEAT) ){
        m_BitArray.ResetBit(FLAG_ON);
    }

    switch( m_Type )
    {
    case TYPE_PHYSICS:
        {
            // 物理用スイッチ挙動
            TbPhysicsRay2d ray;
            TbPhysicsRayCastResult result;
            ray.p1 = GetDrawPos();
            ray.p2= ray.p1 + TbVector2(0.0f,-16.0f);
            if( GetActionStage()->GetPhysicsScene().RayCast(ray,&result) ){
                onCollided(nullptr);
            }
        }
        break;
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionSwitch::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_RETURN:
        {
            if( m_BitArray.Test(FLAG_ON) ){
                m_BitArray.SetBit(FLAG_REQ_REBOUND);
                m_BitArray.ResetBit(FLAG_ON);
            }
        }
        break;
    }
    return 0;
}

/*!
 * 衝突時コールバック
 * @author teco
 */
void SideActionSwitch::onCollided(ActionCollision* collision)
{
    static TB_NDEBUG_CONST TbFloat32 OFFSET_Y = -3.0f;
    ActionActor* actor = collision ? collision->GetActor() : nullptr;
    if( m_BitArray.Test(FLAG_REBOUND) || m_BitArray.Test(FLAG_REQ_REBOUND) ) {
        if( actor && TYPE_REVERSE != m_Type ){
            // 跳ね返り通知
            TbVector2 jumpVelo(-3.0f,-7.0f);
            actor->NotifyEvent(SIDE_ACTION_EVENT_JUMP,&jumpVelo);
        }
        m_BitArray.SetBit(FLAG_REQ_REBOUND);
    }else{
        if( !m_BitArray.Test(FLAG_ON) ) {
            m_BitArray.SetBit(FLAG_ON);
            // 睡眠通知
            if( actor && m_BitArray.Test(FLAG_ENABLE_SLEEP) ){
                actor->NotifyEvent(SIDE_ACTION_EVENT_SLEEP,nullptr);
            }
        }
    }
    if( actor ) {
        actor->SetOffsetDrawPos(TbVector2(0.0f,OFFSET_Y));
    }
}

/*!
 * イベント通知
 * @author teco
 */
void SideActionSwitch::changeSwitchState( TbBool isOn )
{
    static const TbSint32 NAME_OFFSET[] = 
    {
        TB_ARRAY_COUNT_OF("Switch")-1 , 
        TB_ARRAY_COUNT_OF("ReverseSwitch")-1 , 
        TB_ARRAY_COUNT_OF("PhysicsSwitch")-1
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(NAME_OFFSET)==TYPE_MAX);
    // スイッチイベントを送信する
    TbStaticString32 key = GetName();
    key = key.Substr(NAME_OFFSET[m_Type]);
    TbSint32 event = isOn?SIDE_ACTION_EVENT_SWITCH_ON:SIDE_ACTION_EVENT_SWITCH_OFF;
    GetActionStage()->NotifyEvent(key.GetBuffer(),event,&m_Id);
    OnChangeSwitchState(isOn);
}


/////////////////////////
// SideActionWakeSwitch
/////////////////////////

/*!
 * スイッチ状態変化
 * @author teco
 */
void SideActionWakeSwitch::OnChangeSwitchState(TbBool isOn)
{
    if (isOn) {
        // プレイヤー全員死亡
        GetActionStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_WAKEUP, nullptr);
    }
}
