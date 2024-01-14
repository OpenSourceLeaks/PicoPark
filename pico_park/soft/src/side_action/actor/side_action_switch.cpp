/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_switch.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
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

static const TbRectF32Pod SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(160,448,16,16),
    CMN_TEX_UV_RECT(192,448,16,16)
};

static const TbRectF32Pod SPRITE_UV_RECT_ON[] = 
{
    CMN_TEX_UV_RECT(176,448,16,16),
    CMN_TEX_UV_RECT(208,448,16,16)
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
SideActionSwitch::SideActionSwitch( Type type , Color color )
    : Super()
    , m_Type(type)
    , m_Id(0)
    , m_Color(color)
    , m_ContactTarget(nullptr)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT[color]);

    // コリジョン
    switch( m_Type )
    {
    case TYPE_NORMAL:
        {
            TbUint32 pushBackTarget = 0;
            CrActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
            collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
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
            CrActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
            collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
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
 * COLOR設定
 * @author teco
 * @since 2013.09.19
 */
void SideActionSwitch::SetColor(Color color)
{
    m_Color = color;
    if (m_BitArray.Test(FLAG_ON)) {
        GetMainSprite().SetUV(SPRITE_UV_RECT_ON[m_Color]);
    }else{
        GetMainSprite().SetUV(SPRITE_UV_RECT[m_Color]);
    }
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
        GetMainSprite().SetUV(SPRITE_UV_RECT_ON[m_Color]);
        if( !m_BitArray.Test(FLAG_ON_PRE) ){
            changeSwitchState(TB_TRUE);
        }
    }else{
        GetMainSprite().SetUV(SPRITE_UV_RECT[m_Color]);
        if( m_BitArray.Test(FLAG_ON_PRE) ){
            changeSwitchState(TB_FALSE);
        }
    }
    m_BitArray.SetBit(FLAG_ON_PRE,m_BitArray.Test(FLAG_ON));
    m_ContactTarget = nullptr;

    // 繰り返し利用できるスイッチなら一度フラグをおろす
    if( m_BitArray.Test(FLAG_ENABLE_REPEAT) ){
        m_BitArray.ResetBit(FLAG_ON);
    }
    m_BitArray.ResetBit(FLAG_TRIGGER_OFF);
    if (m_BitArray.Test(FLAG_REQ_OFF)) {
        m_BitArray.ResetBit(FLAG_ON);
        m_BitArray.SetBit(FLAG_TRIGGER_OFF);
        m_BitArray.ResetBit(FLAG_REQ_OFF);
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
* 対象へイベント通知
* @author teco
*/
void SideActionSwitch::NotifyTarget( TbBool isOn )
{
    static const TbSint32 NAME_OFFSET = TB_ARRAY_COUNT_OF("Switch") - 1;
    // スイッチイベントを送信する
    TbStaticString32 key = GetName();
    TbSint32 index = TbStrFind(key.GetBuffer(), "Switch");
    if( 0 <= index)
    {
        key = key.Substr(index + NAME_OFFSET);
        TbSint32 event = isOn ? SIDE_ACTION_EVENT_SWITCH_ON : SIDE_ACTION_EVENT_SWITCH_OFF;
        GetActionStage()->NotifyEvent(key.GetBuffer(), event, &m_Id);
        if (!m_SubTarget.IsEmpty()) {
            GetActionStage()->NotifyEvent(m_SubTarget.GetBuffer(), event, &m_Id);
        }
    }
}

/*!
 * 衝突時コールバック
 * @author teco
 */
void SideActionSwitch::onCollided(CrActionCollision* collision)
{
    static TB_NDEBUG_CONST TbFloat32 OFFSET_Y = -3.0f;
    CrActionActor* actor = collision ? collision->GetActor() : nullptr;
    if (m_Type != TYPE_PHYSICS)
    {
        if (!collision) {
            return;
        }
        if (!(collision->GetPushBackTarget() & CrActionCollision::PUSH_BACK_TARGET_COLLISION)) {
            return;
        }
        if (collision->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER &&
            collision->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB &&
            collision->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT ) 
        {
            return;
        }
    }
    if( m_BitArray.Test(FLAG_REBOUND) || m_BitArray.Test(FLAG_REQ_REBOUND) ) {
        if( actor && TYPE_REVERSE != m_Type ){
            // 跳ね返り通知
            TbVector2 jumpVelo(-3.0f,-7.0f);
            actor->NotifyEvent(SIDE_ACTION_EVENT_JUMP,&jumpVelo);
        }
        m_BitArray.SetBit(FLAG_REQ_REBOUND);
    }else{
        m_ContactTarget = collision;
        if( !m_BitArray.Test(FLAG_ON) ) {
            if (m_BitArray.Test(FLAG_TRIGGER_OFF)) {
                // OFFした瞬間にONするならトリガー検知できるように
                if (m_BitArray.Test(FLAG_ON_PRE)) {
                    m_BitArray.ResetBit(FLAG_ON_PRE);
                    changeSwitchState(TB_FALSE);
                }
            }
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
    if (!m_BitArray.Test(FLAG_MANUAL_NOTIFY_TARGET))
    {
        NotifyTarget(isOn);
    }
    if(isOn)
    {
        TbPlaySound("switch");
    }
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

/////////////////////////
// SideActionJumpSwitch
/////////////////////////

/*!
* スイッチ状態変化
* @author teco
*/
void SideActionJumpSwitch::OnChangeSwitchState(TbBool isOn)
{
    CrActionCollision* collision = GetContactCollision();
    if (!collision) {
        return;
    }
    if (isOn) {
        TbStaticString32 key = GetName();
        key = key.Substr(TB_ARRAY_COUNT_OF("JumpSwitch")-1);
        CrActor* actor = GetStage()->FindActor(key.GetBuffer());
        if (!actor) {
            actor = collision->GetActor();
        }
        TB_RESQUE(actor);
        if (actor->GetLayerIndex() == SIDE_ACTION_ACTOR_LAYER_PLAYER) {
            TbVector2 velo = m_JumpVelo;
            velo.SetX(0.0f);
            actor->NotifyEvent(SIDE_ACTION_EVENT_JUMP_FROM_SWITCH, &velo);
        } else {
            actor->NotifyEvent(SIDE_ACTION_EVENT_JUMP, &m_JumpVelo);
        }
    }
}

/////////////////////////
// SideActionScaleSwitch
/////////////////////////

/*!
* スイッチ状態変化
* @author teco
*/
void SideActionScaleSwitch::OnChangeSwitchState(TbBool isOn)
{
}

/*!
 * スイッチ状態変化
 * @author teco
 */
void SideActionScaleSwitch::OnUpdateAction(TbFloat32 deltaTime)
{
    if (IsOn()) {
        CrActionCollision* collision = GetContactCollision();
        if (collision ) {
            CrActionActor* actor = static_cast<CrActionActor*>(collision->GetActor());
            TB_RESQUE(actor);
            if (collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER) {
                actor->NotifyEvent(SIDE_ACTION_EVENT_SCALE, &m_ScaleVelo);
            }
        }
    }
    Super::OnUpdateAction(deltaTime);
}

// 前景描画
void SideActionScaleSwitch::onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material)
{
    static const TbFloat32 OFFSET_Y = -40.0f;

    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    fontBuffer->SetFontSize(32);

    // 数
    fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
        static_cast<TbSint32>(drawPos.GetY() + OFFSET_Y), "%s", 0.0f < m_ScaleVelo ? "+" : "-");
    fontBuffer->PopSetting();
    fontBuffer->Flush();

}

/////////////////////////
// SideActionDelaySwitch
/////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionDelaySwitch::SideActionDelaySwitch()
    : Super(TYPE_NORMAL)
    , m_RestSec(0.0f)
    , m_DelaySec(0.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
    SetManualNotifyTarget();

    // 文字
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionDelaySwitch::Setup(const CrActorFactory::UserData& userData)
{
    m_DelaySec = userData.AsFloat32(0.0f);
}

/*!
* スイッチ状態変化
* @author teco
*/
void SideActionDelaySwitch::OnChangeSwitchState(TbBool isOn)
{
    if( isOn )
    {
        m_RestSec = m_DelaySec;
    }
}

/*! 
 * スイッチ状態変化
 * @author teco
 */
void SideActionDelaySwitch::OnUpdateAction(TbFloat32 deltaTime)
{
    if (0.0f < m_RestSec) {
        m_RestSec -= deltaTime;
        if (m_RestSec <= 0.0f)
        {
            NotifyTarget(TB_TRUE);
            RequestOff();
            m_RestSec = 0.0f;
        }
    }
    Super::OnUpdateAction(deltaTime);
}

/*! 
 * 前景描画
 * @author teco
 */
void SideActionDelaySwitch::onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material)
{
    static const TbFloat32 OFFSET_Y = -75.0f;

    if( 0.0f < m_RestSec )
    {
        const TbVector2& drawPos = GetDrawPos();
        TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
        fontBuffer->PushSetting();
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        fontBuffer->SetFontSize(32);

        // 数
        TbSint32 restSec = static_cast<TbSint32>(m_RestSec + 0.99f);
        fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
            static_cast<TbSint32>(drawPos.GetY()+ OFFSET_Y), "%d", restSec);
        fontBuffer->PopSetting();
        fontBuffer->Flush();
    }
}

/////////////////////////
// SideActionSwitchMediator
/////////////////////////

// イベント通知
TbSint32 SideActionSwitchMediator::NotifyEvent(TbUint32 event, void* param)
{
    TbStaticString32 target = m_Target;
    if (SIDE_ACTION_EVENT_SWITCH_ON == event) {
        target.SetFormatedString("%s%d", m_Target.GetBuffer(), m_Counter+1);
        GetStage()->NotifyEvent(target.GetBuffer(), event, param);
        ++m_Counter;
    }
    else if (SIDE_ACTION_EVENT_SWITCH_OFF == event) {
        TB_ASSERT(0<m_Counter);
        target.SetFormatedString("%s%d", m_Target.GetBuffer(), m_Counter);
        GetStage()->NotifyEvent(target.GetBuffer(), event, param);
        --m_Counter;
    }
    return 0;
}
