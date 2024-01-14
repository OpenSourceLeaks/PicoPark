/*!
 * プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_player.h"
#include "root.h"

#include "common/stage/stage.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_anim_data.h"
#include "side_action/actor/state/side_action_player_state_default.h"
#include "side_action/actor/state/side_action_player_state_dead.h"
#include "side_action/actor/state/side_action_player_state_clear.h"
#include "system/sys_player_input.h"

#include <base/math/tb_random.h>
#include <base/math/tb_math_util.h>

static const TbRectF32Pod SPRITE_RECT = 
{
    -22.0f , -24.0f , 44.0f , 48.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+6.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-12.0f , SPRITE_RECT.height-2.0f 
};

struct AnimData 
{
    const char* name;
    const TbAnimationKeyFrameDataRectF32* data;
};

AnimData ANIM_TABLE[] =
{
    {"default",&SideActionAnimData::PLAYER_DEFAULT},
    {"walk",&SideActionAnimData::PLAYER_WALK },
    {"jump",&SideActionAnimData::PLAYER_JUMP},
    {"push_walk",&SideActionAnimData::PLAYER_PUSH_WALK},
    {"dead",&SideActionAnimData::PLAYER_DEAD},
    {"sleep",&SideActionAnimData::PLAYER_SLEEP},
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ANIM_TABLE)==SideActionPlayer::ANIM_MAX);

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
    , m_Life(1)
    , m_WakeUpInitCount(1)
    , m_WakeUpCount(1)
    , m_CurrentAnim(ANIM_DEFAULT)
    , m_InputType(INPUT_TYPE_NORMAL)
    , m_TitleInput(playerIndex)
    , m_TitleInputChangeTimer(0.0f)
{
    m_StateImpl = createState(m_State);

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
    sprite.SetZ(CMN_SPRITE_Z_FRONT_3);

    // コリジョン設定
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER);
    collision->SetPriority(-1);
    collision->SetContactAddedCallback( TbCreateMemFunc(this,&Self::onContactAdded) );
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
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionPlayer::OnUpdateAction( TbFloat32 deltatime )
{
    // 状態遷移リクエストがあったら変更
    if( m_RequestedState != STATE_NONE ) {
        TB_SAFE_DELETE(m_StateImpl);
        m_StateImpl = createState(m_RequestedState);
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
        if( SysInput::IsOnAny() )
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
}

/*!
 * 後処理更新
 * @author teco
 * @since 2014.08.02
 */
void SideActionPlayer::OnPostAction( TbFloat32 deltatime )
{
    if( m_StateImpl ){
        m_StateImpl->Post(*this,deltatime);
    }
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
        if( 0 < m_Life )
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
    if( m_StateImpl ){
        m_StateImpl->NotifyEvent(*this,event,param);
    }
    return 0;
}

/*!  
 * 接触開始コールバック
 * @author teco
 * @since 2013.10.15
 */
void SideActionPlayer::onContactAdded(const TbVector2& normal,
                                      ActionCollision::ContactTarget target,
                                      const ActionCollisionInfo& info)
{
    if( m_StateImpl ){
        m_StateImpl->OnContactAdded(*this,normal,target,info);
    }
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
        {
            // 状態を持たない
            // いろいろ消す
            GetMainSprite().SetVisible(TB_FALSE);
            GetCollision()->SetActive(TB_FALSE);
            ResetVelo();
        }
        break;
    };
    return result;
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
TbBool SideActionPlayer::IsOnInput( SysInput::Button button ) const
{
    if( IsSleeping() ){
        return TB_FALSE;
    }
    static TbSint32 s_Debug = -1;
    SysInput::Button s_DebugButton = SysInput::BUTTON_RIGHT;
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
TbBool SideActionPlayer::IsPressInput( SysInput::Button button ) const
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
TbBool SideActionPlayer::IsReleaseInput( SysInput::Button button ) const
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
    return result;
}

/*!
 * 歩行速度取得
 * @author teco
 */
TbFloat32 SideActionPlayer::GetWalkVelo() const
{
    static TB_NDEBUG_CONST TbFloat32 SPEED_X = 3.0f;

    TbFloat32 result = SPEED_X;
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
    const ActionCollision* collision = GetCollision();
    return isFloating(collision);
}

/*!
 * 浮いているか
 * @author teco
 */
TbBool SideActionPlayer::isFloating( const ActionCollision* collision ) const
{
    TB_RESQUE_RET(collision,TB_FALSE);

    if( collision->IsContactedMap(CMN_CONTACT_DOWN) ){
        return TB_FALSE;
    }
        
    ActionCollision* dst[ActionCollision::CONTACT_GET_MAX];
    TbUint32 count = collision->GetContactCollision(dst,CMN_CONTACT_DOWN);
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

