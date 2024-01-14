/*!
 * プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_player.h"
#include "root.h"

#include <crayon/stage/cr_stage.h>
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_anim_data.h"
#include "side_action/actor/state/side_action_player_state_default.h"
#include "side_action/actor/state/side_action_player_state_dead.h"
#include "side_action/actor/state/side_action_player_state_clear.h"
#include "side_action/actor/state/side_action_player_state_null.h"
#include "side_action/actor/state/side_action_player_state_plane.h"
#include <crayon/input/cr_player_input.h>
#include "common/cmn_util.h"

#include <base/math/tb_random.h>
#include <base/math/tb_math_util.h>

static const TbRectF32Pod SPRITE_RECT =
{
    -32.0f , -62.0f , 64.0f , 62.0f
};

static const TbRectF32Pod SPRITE_RECT_FOR_COL =
{
    -22.0f , -48.0f , 44.0f , 48.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT_FOR_COL.x+6.0f , SPRITE_RECT_FOR_COL.y+1 , SPRITE_RECT_FOR_COL.width-12.0f , SPRITE_RECT_FOR_COL.height-2.0f
};

struct AnimData 
{
    const char* name;
    const TbAnimationKeyFrameDataRectF32* data;
};

static AnimData ANIM_TABLE[] =
{
    {"default",&SideActionAnimData::PLAYER_DEFAULT},
    {"walk",&SideActionAnimData::PLAYER_WALK },
    {"jump",&SideActionAnimData::PLAYER_JUMP},
    {"push_walk",&SideActionAnimData::PLAYER_PUSH_WALK},
    {"dead",&SideActionAnimData::PLAYER_DEAD},
    {"sleep",&SideActionAnimData::PLAYER_SLEEP},
    {"plane",&SideActionAnimData::PLAYER_PLANE },
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ANIM_TABLE)==SideActionPlayer::ANIM_MAX);

static const TbFloat32 SCALE_MAX = 3.5f;
static const TbFloat32 SCALE_MIN = 0.4f;
static TB_NDEBUG_CONST TbFloat32 SPEED_X = 3.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayer::SideActionPlayer( TbUint32 playerIndex )
    : Super()
    , m_PlayerIndex(playerIndex)
    , m_StateImpl(NULL)
    , m_State(STATE_DEFAULT)
    , m_RequestedState(STATE_NONE)
    , m_WalkSpeed(SPEED_X)
    , m_WalkSpeedScale(1.0f)
    , m_Life(1)
    , m_WakeUpInitCount(1)
    , m_WakeUpCount(1)
    , m_JumpContinueCount(-1)
    , m_History()
    , m_CurrentAnim(ANIM_DEFAULT)
    , m_InputType(INPUT_TYPE_NORMAL)
    , m_TitleInput(playerIndex)
    , m_TitleInputChangeTimer(0.0f)
{
    AddComponent(&m_SpriteHooker);
    m_GravityDir.Set(0.0f,1.0f);
    m_History.SetEnablePopAuto(TB_TRUE);
    m_StateImpl = createState(m_State);
    SetEnableNotifyJumpRecursive();
    SetEnablePushBackVec(TB_TRUE);
    TbFillNull(m_EquipActor);

    // アクター設定
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PLAYER);

    // スプライト設定
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SideActionAnimData::PLAYER_UV;
    m_SpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        for( size_t i = 0; i < TB_ARRAY_COUNT_OF(m_SpriteAnim); ++i ){
            animInitParam.param.name = ANIM_TABLE[i].name;
            animInitParam.param.uvOffsetAnim = ANIM_TABLE[i].data;
            m_SpriteAnim[i].Initialize(1,&animInitParam);
        }
    }
    TbSprite& sprite = GetMainSprite();
    sprite.SetColor(CmnUtil::GetPlayerColor(m_PlayerIndex));
    sprite.Initialize(&m_SpriteNode,&m_SpriteAnim[m_CurrentAnim]);
    sprite.SetMaterial(&m_Material);
    sprite.SetEnableLoop(TB_TRUE);
    TbFloat32 z = calcZ();
    sprite.SetZ(z);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_3);

    // コリジョン設定
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
                              CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER);
    collision->SetPriority(-1);
    collision->SetEnableForceSearchUpContact(TB_TRUE);
    collision->SetContactAddedCallback( TbCreateMemFunc(this,&Self::onContactAdded) );
    collision->SetScalePivot( TbVector2(0.0f,-1.0f) );
    SetWeight( WEIGHT_MAX );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayer::~SideActionPlayer()
{
    TB_SAFE_DELETE(m_StateImpl);
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionPlayer::Setup( const CrActorFactory::UserData& userData )
{
    if (userData.AsSint32(0) == 1) {
        LookLeft();
    }
}

/*!
 * プレイヤー番号変更
 * @author teco
 */
void SideActionPlayer::SetPlayerIndex(TbUint32 index)
{
    m_PlayerIndex = index;
    TbSprite& sprite = GetMainSprite();
    sprite.SetColor(CmnUtil::GetPlayerColor(m_PlayerIndex));
    m_MultiPlayerInput.SetMainPlayerIndex(index);
}

/*!
 * 横幅取得
 * @author teco
 */
TbFloat32 SideActionPlayer::GetWidth() const
{
    return SPRITE_RECT.width;
}

/*!
 * アニメーション設定
 * @author teco
 * @since 2013.10.20
 */
void SideActionPlayer::SetAnim( AnimType anim )
{
    if( m_CurrentAnim != anim ) {
        // 可能な状態遷移か
        switch( anim )
        {
        case ANIM_WALK:
            if( !(m_CurrentAnim == ANIM_DEFAULT || m_CurrentAnim == ANIM_PUSH_WALK) ){
                return;
            }
            break;
        case ANIM_PUSH_WALK:
            if( !(m_CurrentAnim == ANIM_DEFAULT || m_CurrentAnim == ANIM_WALK) ){
                return;
            }
            break;
        case ANIM_JUMP:
        case ANIM_DEFAULT:
        case ANIM_DEAD:
            break;
        }
        GetMainSprite().SetNodeAnimation(&m_SpriteAnim[anim]);
        m_CurrentAnim = anim;
    }
}

/*! 
 * UVオフセット設定
 * @author teco
 */
void SideActionPlayer::SetOffserUV(const TbVector2 offset)
{
    TbRectF32 gunUV = SideActionAnimData::PLAYER_UV;
    TbSprite& sprite = GetMainSprite();
    gunUV.y += offset.GetY();
    gunUV.x += offset.GetX();
    sprite.SetUV(gunUV);
}

/*! 
 * スケール変更
 * @author teco
 */
TbFloat32 SideActionPlayer::AddScale(TbFloat32 scale)
{
    CrActionCollision* collision = GetCollision();
    TbSprite& sprite = GetMainSprite();
    TbVector2 spriteScale = sprite.GetScale();
    TbFloat32 dst = spriteScale.GetY();
    TbFloat32 src = dst;
    dst += scale;
    dst = TbClamp(dst, SCALE_MIN, SCALE_MAX);
    spriteScale.Set( TbSign(spriteScale.GetX())*dst, dst );
    sprite.SetScale(spriteScale);
    TbFloat32 collisionScale = spriteToCollisionScale(dst);
    collision->SetScale(collisionScale);
    return dst- src;

#if 0
    // ワープ
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    CrActionCollisionScene& collisionScene = stage->GetCollisionScene();
    CrActionActor* target = m_Gun->GetTarget();
    CrActionCollision* targetCollision = target->GetCollision();
    TbRectF32 targetRect = targetCollision->GetRect();
    TbVector2 targetPos = GetPosXY();
    if (0.0f < normal.GetX())
    {
        targetPos.Add(-targetRect.width - targetRect.x - 1.0f, 0.0f);
    }
    else
    {
        targetPos.Add(-targetRect.x + 1.0f, 0.0f);
    }
    targetPos.Add(0.0f, -targetRect.height - targetRect.y - 1.0f);
    TbVector2 backup = targetCollision->GetPos();
    targetCollision->SetPos(targetPos);
    if (collisionScene.IsCollidedByPushBackTarget(*targetCollision, CrActionCollision::PUSH_BACK_TARGET_MAP | CrActionCollision::PUSH_BACK_TARGET_COLLISION)) {
        targetPos.Add(0.0f, targetRect.height + 2.0f);
        targetCollision->SetPos(targetPos);
    }
    if (!collisionScene.IsCollidedByPushBackTarget(*targetCollision, CrActionCollision::PUSH_BACK_TARGET_MAP | CrActionCollision::PUSH_BACK_TARGET_COLLISION)) {
        // 何も衝突してなかったらワープ
        targetCollision->SetPos(backup);
        TbBool visible = TB_TRUE;
        target->NotifyEvent(SIDE_ACTION_EVENT_WARP, &targetPos);
        target->NotifyEvent(SIDE_ACTION_EVENT_SET_VISIBLE, &visible);
        m_Gun->ResetTarget();
    }
    else {
        targetCollision->SetPos(backup);
    }
#endif
}

static const TbFloat32 COLLISION_MODIFY_SCALE = 0.88f;

/*! 
 * スプライト用スケールをコリジョン用スケールに変換
 * @author teco
 */
TbFloat32 SideActionPlayer::spriteToCollisionScale(TbFloat32 scale)
{
    if (scale <= 1.0f) {
        return scale;
    }
    scale = 1.0f + ( scale - 1.0f ) * COLLISION_MODIFY_SCALE;
    return scale;
}

/*! 
 * コリジョン用スケールをスプライト用スケールに変換
 * @author teco
 */
TbFloat32 SideActionPlayer::collisionToSpriteScale(TbFloat32 scale)
{
    if (scale <= 1.0f) {
        return scale;
    }
    scale = 1.0f + (scale - 1.0f) / COLLISION_MODIFY_SCALE;
    return scale;
}

/*! 
 * スケール取得
 * @author teco
 */
TbFloat32 SideActionPlayer::GetScale() const
{
    const TbSprite& sprite = GetMainSprite();
    return sprite.GetScale().GetY();
}

/*! 
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionPlayer::OnUpdateAction( TbFloat32 deltatime )
{
    m_PreLife = m_Life;
    // 状態遷移リクエストがあったら変更
    if( m_RequestedState != STATE_NONE ) {
        if(m_StateImpl){
            m_StateImpl->Finalize(*this);
        }
        TB_SAFE_DELETE(m_StateImpl);
        m_StateImpl = createState(m_RequestedState);
        if(m_StateImpl){
            m_StateImpl->Initialize(*this);
        }
        m_State = m_RequestedState;
        m_RequestedState = STATE_NONE;
    }

    // 入力更新
    m_RandomInput.Update(deltatime);
    m_MajorityInput.Update(deltatime);
    m_MultiPlayerInput.Update(deltatime);
    m_MainMenuInput.Update(deltatime);

    // タイトル自動入力に変更
#if SIDE_ACTION_PLAYER_ENABLE_TITLE
    m_TitleInput.Update(deltatime);
    if( IsTitleMode() )
    {
        if( CrInput::IsOnAny() )
        {
            m_InputType = INPUT_TYPE_NORMAL;
            m_TitleInputChangeTimer = 0.0f;
        }
        else
        {
            m_TitleInputChangeTimer += deltatime;
            if( 30.0f < m_TitleInputChangeTimer ){
                m_InputType == INPUT_TYPE_TITLE;
            }
        }
    }
#endif

    // 状態更新
    if( m_StateImpl ){
        m_StateImpl->Update(*this,deltatime);
    }
    m_SwitchInput.Update(deltatime);

    m_PushBackVec.Clear();
}

/*!
 * 平均移動量
 * @author teco
 */
TbVector2 SideActionPlayer::GetAverageMov(TbUint32 frameCount) const
{
    if (0 == frameCount) {
        return TbVector2();
    }
    TbUint32 count = TbMin(frameCount,m_History.GetCount());
    TbVector2 velo;
    for (TbUint32 i = 0; i < count; ++i) {
        velo += m_History[i].mov;
    }
    velo /= static_cast<TbFloat32>(frameCount);
    return velo;
}

/*!
 * 頭の位置取得
 * @author teco
 */
TbVector2 SideActionPlayer::GetHeadPos() const
{
    TbVector2 pos = GetPosXY();
    TbRectF32 rect = GetMainSprite().GetNodeTree()->GetNodeFromIndex(0)->GetBody().rect;
    pos = pos + rect.GetLeftTop();
    return pos;
}

/*!
 * 可視設定
 * @author teco
 */
void SideActionPlayer::SetVisible(TbBool isVisible)
{
    Super::SetVisible(isVisible);
    GetCollision()->SetActive(isVisible);
    if (isVisible) {
        Resume();
    }
    else {
        Suspend();
    }
}

/*!
 * 後処理更新
 * @author teco
 * @since 2014.08.02
 */
void SideActionPlayer::OnPostAction( TbFloat32 deltatime )
{
    if(IsEnablePushBackVec()){
        AddVelo(m_PushBackVec);
    }
    if( m_StateImpl ){
        m_StateImpl->Post(*this,deltatime);
    }
    toybox::TbVector4 mov = GetPos() - GetPrePos();
    History history;
    history.mov = TbVector2(mov.GetX(), mov.GetY());
    m_History.PushFront(history);
}

/*!
 * イベント通知
 * @author teco
 * @since 2013.10.15
 */
TbSint32 SideActionPlayer::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_GET_PLAYERINDEX == event )
    {
        return m_PlayerIndex+1;
    }
    else if( SIDE_ACTION_EVENT_DEAD == event ) {
        if( 0 < m_Life && !m_BitArray.Test(FLAG_IMMORTAL) && !GetActionStage()->IsPlayerUndead())
        {
            --m_Life;
            if( m_Life <= 0 )
            {
                if( !param || *reinterpret_cast<TbSint32*>(param) ) {
                    // 死亡ステートに切り替える
                    RequestState(STATE_DEAD);
                }else{
                    // 直接ゲームオーバー状態にする
                    m_BitArray.SetBit(FLAG_GAME_OVER);
                }
            }
            return 1;
        }
    }else if( SIDE_ACTION_EVENT_GAMEOVER == event ) {
        // ゲームオーバー状態にはしておく
        m_BitArray.SetBit(FLAG_GAME_OVER);
    }else if( SIDE_ACTION_EVENT_SWITCH_ON == event || 
              SIDE_ACTION_EVENT_SWITCH_OFF == event ) 
    {
        if( INPUT_TYPE_SWITCH == m_InputType ){
            // スイッチコントロールモードだと受信
            m_SwitchInput.OnEvent(event,param);
        }
        if (SIDE_ACTION_EVENT_SWITCH_ON == event)
        {
            if (INPUT_TYPE_MULTIPLAYER == m_InputType) {
                m_JumpContinueCount *= 2;
            }
        }
    }else if( SIDE_ACTION_EVENT_RESET_JUMP == event ) {
        if( INPUT_TYPE_MULTIPLAYER == m_InputType ){
            m_MultiPlayerInput.OnEvent(event,param);
        }
    }else if( SIDE_ACTION_EVENT_SLEEP == event ) {
        Sleep();
    }else if( SIDE_ACTION_EVENT_WAKEUP == event ) {
        WakeUp();
    }else if( SIDE_ACTION_EVENT_RETURN == event ) {
        m_WakeUpCount = m_WakeUpInitCount;
    }else if( SIDE_ACTION_EVENT_SUSPEND == event ) {
        Suspend();
    }
    else if (SIDE_ACTION_EVENT_RESUME == event)
    {
        Resume();
    }
    else if (SIDE_ACTION_EVENT_IMMORTAL == event)
    {
        if (param) {
            TbBool isOn = *reinterpret_cast<TbBool*>(param);
            m_BitArray.SetBit(FLAG_IMMORTAL,isOn);
        } else {
            m_BitArray.SetBit(FLAG_IMMORTAL);
        }
    }
    if( m_StateImpl ){
        return m_StateImpl->NotifyEvent(*this,event,param);
    }
    return 0;
}

/*!  
 * 接触による移動
 * @author teco
 */
void SideActionPlayer::OnMoveByContact(const TbVector2& mov, const TbVector2& normal, ActorSet* set , const MoveContactOption& option)
{
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_EquipActor); ++i )
    {
        if(m_EquipActor[i]){
            m_EquipActor[i]->MoveByContactActor(mov,normal,set,option);
        }
    }
}

/*!  
 * 接触開始コールバック
 * @author teco
 * @since 2013.10.15
 */
void SideActionPlayer::onContactAdded(const TbVector2& normal,
                                      CrActionCollision::ContactTarget target,
                                      const CrActionCollisionInfo& info)
{
    if( m_StateImpl ){
        m_StateImpl->OnContactAdded(*this,normal,target,info);
    }
}

// 
/*!  
 * スケール変化
 * @author teco
 */
void SideActionPlayer::OnChangedScale(TbFloat32 scaleX, TbFloat32 scaleY)
{
    scaleX = collisionToSpriteScale(scaleX);
    scaleY = collisionToSpriteScale(scaleY);
    Super::OnChangedScale(scaleX, scaleY);
}

/*!
 * 状態更新
 * @author teco
 * @since 2013.10.18
 */
SideActionPlayerState* SideActionPlayer::createState( State state )
{
    SideActionPlayerState* result = NULL;
    switch( state )
    {
    case STATE_DEFAULT:
        result = new SideActionPlayerStateDefault();
        break;
    case STATE_DEAD:
        result = new SideActionPlayerStateDead();
        break;
    case STATE_CLEAR:
        result = new SideActionPlayerStateClear();
        break;
    case STATE_NULL:
        result = new SideActionPlayerStateNull();
        break;
    case STATE_PLANE:
        m_BitArray.SetBit(FLAG_HAS_PLANE);
        result = new SideActionPlayerStatePlane();
        break;
    };
    return result;
}

/*!
 * 加算する重力計算
 * @author teco
 */
TbVector2 SideActionPlayer::CalcGravity(TbVector2 currentVelo)
{
    // 自由落下
    TbVector2 gravityDir = GetGravityDir();
    TbFloat32 dot = currentVelo.Dot(gravityDir);
    TbFloat32 gravityPower = TbMin(SIDE_ACTION_GRAVITY_MAX, dot + SIDE_ACTION_GRAVITY);
    TbVector2 gravityVelo = gravityDir * (gravityPower - dot);
    return gravityVelo;
}

/*!
 * Z設定
 * @author teco
 */
void SideActionPlayer::SetZ(TbSint32 playerIndex)
{
    TbFloat32 z = calcZ(playerIndex);
    GetMainSprite().SetZ(z);
}

/*!
 * 装備
 * @author teco
 */
void SideActionPlayer::Equip(EquipPart part, CrActionActor* actor)
{
    if (0 <= part && part < TB_ARRAY_COUNT_OF(m_EquipActor)) {
        m_EquipActor[part] = actor;
        // 参照カウンターどうしよう。一旦操作しない。
    }
}

/*!
 * スイッチ入力モード
 * @author teco
 */
void SideActionPlayer::SetSwitchInput()
{
    m_InputType = INPUT_TYPE_SWITCH;
}

/*!
 * 一定以上の人数が入力して反応するように設定
 * @author teco
 */
void SideActionPlayer::SetMajorityInput( TbUint32 count , TbFloat32 rate )
{
    m_MajorityInput.Initialize(count,rate);
    m_InputType = INPUT_TYPE_MAJORITY;
    TbSprite& sprite = GetMainSprite();
    sprite.SetColor(CmnUtil::GetMajorityColor());
}

/*!
 * 10人羽織入力モード
 * @author teco
 */
void SideActionPlayer::SetMultiPlayerInput( TbFloat32 jumpVelo , TbFloat32 walkVelo )
{
    SetEnableAirJump(TB_TRUE);
    m_MultiPlayerInput.Initialize(jumpVelo,walkVelo);
    m_InputType = INPUT_TYPE_MULTIPLAYER;
}

/*!
 * 入力し続けている
 * @author teco
 * @since 2013.10.26
 */
TbBool SideActionPlayer::IsOnInput( CrInput::Button button ) const
{
    if( IsSleeping() ){
        return TB_FALSE;
    }
    static TbSint32 s_Debug = -1;
    CrInput::Button s_DebugButton = CrInput::BUTTON_RIGHT;
    if( s_Debug >= 0 ) {
        if( s_Debug == m_PlayerIndex && button == s_DebugButton ){
            return TB_TRUE;
        }
    }
    TbBool result = TB_FALSE;
    if( GetActionStage()->IsEnableRandomInput() ){
        result = m_RandomInput.IsOn( TB_BIT(button) );
    }else{
        switch( m_InputType )
        {
        case INPUT_TYPE_NORMAL:
            result = GetStage()->GetPlayerInput().IsOn(button,GetPlayerIndex());
            break;
        case INPUT_TYPE_MAJORITY:
            result = m_MajorityInput.IsOn(button);
            break;
        case INPUT_TYPE_SWITCH:
            result = m_SwitchInput.IsOn(TB_BIT(button));
            break;
        case INPUT_TYPE_MULTIPLAYER:
            result = m_MultiPlayerInput.IsOn(TB_BIT(button));
            break;
        case INPUT_TYPE_TITLE:
            result = m_TitleInput.IsOn(TB_BIT(button));
            break;
        case INPUT_TYPE_MAIN_MENU:
            result = m_MainMenuInput.IsOn(TB_BIT(button));
            break;
        }
    }
    return result;
}

/*!
 * 押した
 * @author teco
 */
TbBool SideActionPlayer::IsPressInput( CrInput::Button button ) const
{
    if( IsSleeping() ){
        return TB_FALSE;
    }
    TbBool result = TB_FALSE;
    if( GetActionStage()->IsEnableRandomInput() ){
        result = m_RandomInput.IsPress( TB_BIT(button) );
    }else{
        switch( m_InputType )
        {
        case INPUT_TYPE_NORMAL:
            result = GetStage()->GetPlayerInput().IsPress(button,GetPlayerIndex());
            break;
        case INPUT_TYPE_MAJORITY:
            result = m_MajorityInput.IsPress(button);
            break;
        case INPUT_TYPE_SWITCH:
            result = m_SwitchInput.IsPress(TB_BIT(button));
            break;
        case INPUT_TYPE_MULTIPLAYER:
            result = m_MultiPlayerInput.IsPress(TB_BIT(button));
            break;
        case INPUT_TYPE_TITLE:
            result = m_TitleInput.IsPress(TB_BIT(button));
            break;
        case INPUT_TYPE_MAIN_MENU:
            result = m_MainMenuInput.IsPress(TB_BIT(button));
            break;
        }
    }
    return result;
}

/*!
 * 離した
 * @author teco
 */
TbBool SideActionPlayer::IsReleaseInput( CrInput::Button button ) const
{
    if( IsSleeping() ){
        return TB_FALSE;
    }
    TbBool result = TB_FALSE;
    if( GetActionStage()->IsEnableRandomInput() ){
        result = m_RandomInput.IsRelease( TB_BIT(button) );
    }else{
        switch( m_InputType )
        {
        case INPUT_TYPE_NORMAL:
            result = GetStage()->GetPlayerInput().IsRelease(button,GetPlayerIndex());
            break;
        case INPUT_TYPE_MAJORITY:
            result = m_MajorityInput.IsRelease(button);
            break;
        case INPUT_TYPE_SWITCH:
            result = m_SwitchInput.IsRelease(TB_BIT(button));
            break;
        case INPUT_TYPE_MULTIPLAYER:
            result = m_MultiPlayerInput.IsRelease(TB_BIT(button));
            break;
        case INPUT_TYPE_TITLE:
            result = m_TitleInput.IsRelease(TB_BIT(button));
            break;
        case INPUT_TYPE_MAIN_MENU:
            result = m_MainMenuInput.IsRelease(TB_BIT(button));
            break;
        }
    }
    return result;
}

/*!
 * ジャンプ速度取得
 * @author teco
 */
TbFloat32 SideActionPlayer::GetJumpVelo() const
{
    TbFloat32 result = SIDE_ACTION_JUMP_VELO;
    if( m_InputType == INPUT_TYPE_MULTIPLAYER ) {
        result = m_MultiPlayerInput.GetJumpVelo();
    }
    const TbSprite& sprite = GetMainSprite();
    TbFloat32 spriteScale = sprite.GetScale().GetY();
    if ( spriteScale < 1.0f) {
        static TB_NDEBUG_CONST TbFloat32 SCALE_VELO_MIN = 0.8f;
        result *= TbLerp(SCALE_VELO_MIN,1.0f,spriteScale);
    }
    return result;
}

/*!
 * 歩行速度取得
 * @author teco
 */
TbFloat32 SideActionPlayer::GetWalkVelo() const
{
    TbFloat32 result = m_WalkSpeed * m_WalkSpeedScale;
    if( m_InputType == INPUT_TYPE_MULTIPLAYER ) {
        result = m_MultiPlayerInput.GetWalkVelo();
    }
    return result;
}

/*!
 * 浮いているか
 * @author teco
 */
TbBool SideActionPlayer::IsFloating() const
{
    const CrActionCollision* collision = GetCollision();
    TbBool result = isFloating(collision);
    if (result && m_FloatingFunction.IsValid() ) {
        result = m_FloatingFunction();
    }
    return result;
}

/*!
 * 浮いているか
 * @author teco
 */
TbBool SideActionPlayer::isFloating( const CrActionCollision* collision ) const
{
    TB_RESQUE_RET(collision,TB_FALSE);

    if( collision->IsContactedMap(CR_CONTACT_DOWN) ){
        return TB_FALSE;
    }
        
    CrActionCollision* dst[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 count = collision->GetContactCollision(dst,CR_CONTACT_DOWN);
    for( TbUint32 i = 0; i < count; ++i ){
        if( dst[i]->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER) {
            return TB_FALSE;
        }else{
            if( !isFloating(dst[i]) ){
                return TB_FALSE;
            }
        }
    }

    return TB_TRUE;
}

/*!
* 文字描画
* @author teco
*/
void SideActionPlayer::onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material)
{
    if (m_StateImpl) {
        m_StateImpl->OnDrawText(*this, poseMatrix,material);
    }
    if (GetMainSprite().IsVisible()) {
        const TbVector2& drawPos = GetDrawPos();
    }
}

/*!
 * Z値計算
 * @author teco
 */
TbFloat32 SideActionPlayer::calcZ(TbSint32 playerIndex)
{
    TbFloat32 z = CMN_SPRITE_Z_FRONT_3 - (static_cast<TbFloat32>(playerIndex)*0.01f);
    return z;
}
