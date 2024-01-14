/*!
 * プレイヤーアクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_H_
#define _INCLUDED_ACTION_PLAYER_H_

#include <graphics/sprite/tb_sprite.h>
#include <input/tb_bit_input.h>
#include <crayon/actor/cr_action_actor.h>
#include <base/container/tb_static_deque.h>
#include "side_action/actor/state/side_action_player_state.h"
#include "system/sys_majority_input.h"
#include "side_action_player_input.h"
#include "common/material/cmn_sprite_player_material.h"

class SideActionPlayer;

#define SIDE_ACTION_PLAYER_ENABLE_TITLE (CMN_ENABLE_BUILD_ARCADE)

// SideActionPlayerと同じ
#define SIDE_ACTION_PLAYER_JUMP_COUNT_MAX (14)
#define SIDE_ACTION_PLAYER_JUMP_VELO_COEF (0.2f)

class SideActionPlayer : public CrActionActor
{
public:

    typedef SideActionPlayer Self;
    typedef CrActionActor Super;
    typedef toybox::TbStaticFunction32<TbBool()> FloatingFunction;

    // アニメーション
    enum AnimType
    {
        ANIM_DEFAULT    , // 静止
        ANIM_WALK       , 
        ANIM_JUMP       ,
        ANIM_PUSH_WALK  ,
        ANIM_DEAD       , 
        ANIM_SLEEP      ,
        ANIM_PLANE      , 
        ANIM_MAX ,
    };

    enum State
    {
        STATE_NONE    ,
        STATE_NULL    , 
        STATE_DEFAULT , 
        STATE_DEAD    , 
        STATE_CLEAR   ,
        STATE_PLANE   ,
        STATE_MAX 
    };

    enum InputType
    {
        INPUT_TYPE_NORMAL   ,
        INPUT_TYPE_MAJORITY ,
        INPUT_TYPE_SWITCH   , 
        INPUT_TYPE_MULTIPLAYER , 
        INPUT_TYPE_TITLE ,
        INPUT_TYPE_MAIN_MENU ,
    };

    enum EquipPart
    {
        EQUIO_PART_HAND ,
        EQUIP_PART_MAX
    };

public:

    // コンストラクタ
    SideActionPlayer( TbUint32 playerIndex );

    // デストラクタ
    virtual ~SideActionPlayer();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // プレイヤー
    TbUint32 GetPlayerIndex() const {
        return m_PlayerIndex;
    }

    // プレイヤー番号変更
    void SetPlayerIndex( TbUint32 index );

    // 横幅取得
    TbFloat32 GetWidth() const;

    // アニメーション設定
    void SetAnim( AnimType anim );

    // 現在のアニメーション設定
    AnimType GetAnim() const {
        return m_CurrentAnim;
    }

    // 状態取得
    State GetState() const {
        return m_State;
    }

    // 状態リクエスト
    void RequestState( State state ) {
        if( TB_VERIFY(0<=state&&state<STATE_MAX) ){
            m_RequestedState = state;
        }
    }

    // 表示しているか
    TbBool IsVisible() const {
        return GetMainSprite().IsVisible();
    }
    
    // クリア状態か
    TbBool IsCleared() const {
        return GetState() == SideActionPlayer::STATE_CLEAR;
    }

    // ゲームオーバー
    TbBool IsGameOver() const {
        return m_BitArray.Test(FLAG_GAME_OVER) ||
               GetState() == SideActionPlayer::STATE_NULL;
    }

    // 生きている
    TbBool IsAlive() const {
        return GetState() == SideActionPlayer::STATE_DEFAULT || 
               GetState() == SideActionPlayer::STATE_PLANE;
    }


    void SetEnableWalk( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_WALK,isEnable);
    }

    TbBool IsEnableWalk() const {
        return m_BitArray.Test(FLAG_WALK);
    }

    // 空中ジャンプ可能か
    void SetEnableAirJump( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_AIR_JUMP,isEnable);
    }

    // 空中ジャンプ可能か
    TbBool IsEnableAirJump() const {
        return m_BitArray.Test(FLAG_ENABLE_AIR_JUMP);
    }

    // ジャンプで色変更可能
    void SetEnableChangeColorWithJump( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_CHANGE_COLOR_WIDH_JUMP,isEnable);
    }

    // ジャンプで色変更可能
    TbBool IsEnableChangeColorWithJump() const {
        return m_BitArray.Test(FLAG_ENABLE_CHANGE_COLOR_WIDH_JUMP);
    }

    // PushBackVecを速度に利用するか
    void SetEnablePushBackVec( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_PUSH_BACK_VEC,isEnable);
    }

    // PushBackVecを速度に利用するか
    TbBool IsEnablePushBackVec() const {
        return m_BitArray.Test(FLAG_ENABLE_PUSH_BACK_VEC);
    }

    // ライフ追加
    void AddLife() {
        ++m_Life;
    }

    // ライフ取得
    TbUint32 GetPreLife() const {
        return m_PreLife;
    }

    // 不死身か
    TbBool IsImmoral() const {
        return m_BitArray.Test(FLAG_IMMORTAL);
    }

    // 浮いているか
    TbBool IsFloating() const;

    // 寝るリクエスト
    void Sleep() {
        if( 0 < m_WakeUpCount ) {
            --m_WakeUpCount;
        }
    }

    // 起きる
    void WakeUp() {
        if( m_WakeUpCount < m_WakeUpInitCount ){
            ++m_WakeUpCount;
        }
    }

    // 起きる
    void SetWakeUpCount( TbSint32 count ) {
        m_WakeUpInitCount = count;
        m_WakeUpCount = count;
    }

    // 寝ているか
    TbBool IsSleeping() const {
        return 0 == m_WakeUpCount;
    }
    
    // UVオフセット設定
    void SetOffserUV( const TbVector2 offset );

    // スケール変更
    TbFloat32 AddScale( TbFloat32 scale );

    // スケール取得
    TbFloat32 GetScale() const;

    // 平均移動量
    TbVector2 GetAverageMov( TbUint32 frameCount ) const;

    // 浮遊外部関数設定
    void SetFloatingFunction(const FloatingFunction& func) {
        m_FloatingFunction = func;
    }

    // 頭の位置取得
    TbVector2 GetHeadPos() const;

    // 可視設定
    virtual void SetVisible( TbBool isVisible ) TB_OVERRIDE;

    // 重力方向設定
    void SetGravityDir(TbVector2 gravity) {
        m_GravityDir = gravity;
    }

    // 重力方向リセット
    void ResetGravityDir() {
        SetGravityDir(TbVector2(0.0f,1.0f));
    }

    // 重力方向取得
    TbVector2 GetGravityDir() const {
        return m_GravityDir;
    }

    // 加算する重力計算
    TbVector2 CalcGravity( TbVector2 currentVelo );

    // マテリアル取得
    CmnSpritePlayerMaterial* GetMaterial() {
        return &m_Material;
    }

    // Z設定
    void SetZ( TbSint32 playerIndex );

public: // 装備

    // 装備
    void Equip( EquipPart part , CrActionActor* actor );

    // 装備取得
    CrActionActor* GetEquip( EquipPart part ) {
        if (0 <= part && part < TB_ARRAY_COUNT_OF(m_EquipActor)) {
            return m_EquipActor[part];
        }
        return nullptr;
    }

    // 装備取得
    const CrActionActor* GetEquip(EquipPart part) const {
        if (0 <= part && part < TB_ARRAY_COUNT_OF(m_EquipActor)) {
            return m_EquipActor[part];
        }
        return nullptr;
    }

public: // スイッチ入力モード

    // スイッチ入力モード
    void SetSwitchInput();

public: // 多数決モード

    // 一定以上の人数が入力して反応するように設定
    void SetMajorityInput( TbUint32 count=1 , TbFloat32 rate=1.0f );

    // 多数決入力取得
    const SysMajorityInput* GetMajorityInput() const {
        if( INPUT_TYPE_MAJORITY == m_InputType ){
            return &m_MajorityInput;
        }
        return NULL;
    }

public: // 10人羽織入力モード

    // 10人羽織入力モード
    void SetMultiPlayerInput( TbFloat32 jumpVelo , TbFloat32 walkVelo );

public: // 飛行機モード

    // 飛行機を持っている
    TbBool HasPlane() const {
        return m_BitArray.Test(FLAG_HAS_PLANE);
    }

public: // タイトルモード

    // タイトルモード
    void SetTitleMode() {
#if SIDE_ACTION_PLAYER_ENABLE_TITLE
        m_BitArray.SetBit(FLAG_TITLE);
        m_InputType = INPUT_TYPE_TITLE;
#endif
    }
    
    // タイトルモード
    TbBool IsTitleMode() const {
        return m_BitArray.Test(FLAG_TITLE);
    }

public: // メインメニューモード

    // メインメニューモード
    void SetMainMenuMode() {
        m_InputType = INPUT_TYPE_MAIN_MENU;
    }

    TbBool IsMainMenuMode() {
        return m_InputType == INPUT_TYPE_MAIN_MENU;
    }

public:

    // 入力し続けている
    TbBool IsOnInput( CrInput::Button button ) const;

    // 押した
    TbBool IsPressInput( CrInput::Button button ) const;

    // 離した
    TbBool IsReleaseInput( CrInput::Button button ) const;

    // ジャンプ速度取得
    TbFloat32 GetJumpVelo() const;
    
    // 歩行速度取得
    TbFloat32 GetWalkVelo() const;

    // 歩行スピードのスケール設定
    void SetWalkSpeedScale(TbFloat32 scale) {
        m_WalkSpeedScale = scale;
    }

    // 歩行スピードのスケール取得
    TbFloat32 GetWalkSpeedScale() const {
        return m_WalkSpeedScale;
    }

    // 連続ジャンプ可能数
    void SetJumpContinueCount(TbSint32 count) {
        m_JumpContinueCount = count;
    }

    // 連続ジャンプ可能数
    TbSint32 GetJumpContinueCount() const {
        return m_JumpContinueCount;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 後処理更新
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

    // 接触による移動
    virtual void OnMoveByContact(const TbVector2& mov, const toybox::TbVector2& normal, ActorSet* set , const MoveContactOption& option);

protected:

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) TB_OVERRIDE
    {
        m_PushBackVec = pushBackVec;
    }

private:

    enum Flag
    {
        FLAG_WALK,
        FLAG_ENABLE_AIR_JUMP , // 空中ジャンプ可能
        FLAG_ENABLE_CHANGE_COLOR_WIDH_JUMP ,
        FLAG_ENABLE_PUSH_BACK_VEC ,
        FLAG_GAME_OVER ,
        FLAG_TITLE , 
        FLAG_HAS_PLANE ,
        FLAG_IMMORTAL , 
        FLAG_MAX
    };

    static const TbUint32 HISTORY_MAX = 256;
    struct History
    {
        TbVector2 mov;
    };

private:

    // 接触開始コールバック
    virtual void onContactAdded(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info);

    // スケール変化
    virtual void OnChangedScale(TbFloat32 scaleX, TbFloat32 scaleY);
        
    // 状態更新
    SideActionPlayerState* createState( State state );

    // 浮いているいか
    TbBool isFloating( const CrActionCollision* collision ) const;

    // スプライト用スケールをコリジョン用スケールに変換
    TbFloat32 spriteToCollisionScale( TbFloat32 scale );

    // コリジョン用スケールをスプライト用スケールに変換
    TbFloat32 collisionToSpriteScale(TbFloat32 scale);

    // 前景描画
    void onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

    // Z値計算
    TbFloat32 calcZ() {
        return calcZ(static_cast<TbSint32>(m_PlayerIndex));
    }

    // Z値計算
    TbFloat32 calcZ(TbSint32 playerIndex);

private:

    TbBitArray32            m_BitArray;
    TbUint32                m_PlayerIndex;
    SideActionPlayerState*  m_StateImpl;
    State                   m_State;
    State                   m_RequestedState;

private:

    TbFloat32               m_WalkSpeed;
    TbFloat32               m_WalkSpeedScale;
    TbSint32                m_Life;
    TbSint32                m_PreLife;
    TbSint32                m_WakeUpInitCount;
    TbSint32                m_WakeUpCount;
    TbStaticDeque<History,HISTORY_MAX>  m_History;
    FloatingFunction        m_FloatingFunction;
    TbVector2               m_PushBackVec;
    TbVector2               m_GravityDir;
    TbSint32                m_JumpContinueCount; // 連続ジャンプ可能数

private:

    CrActionActor*          m_EquipActor[EQUIP_PART_MAX];

private: // 表示系統

    TbSpriteNodeTree            m_SpriteNode;
    TbSpriteNodeAnimTree        m_SpriteAnim[ANIM_MAX];
    CmnSpritePlayerMaterial     m_Material;
    AnimType                    m_CurrentAnim;
    CrActionActorSpriteHooker   m_SpriteHooker;

private: // 入力系統

    InputType                           m_InputType;
    SysMajorityInput                    m_MajorityInput;
    SideActionPlayerRandomInput         m_RandomInput;
    SideActionPlayerSwitchInput         m_SwitchInput;
    SideActionPlayerMultiPlayerInput    m_MultiPlayerInput;
    SideActionPlayerTitleInput          m_TitleInput;
    SideActionPlayerMainMenuInput       m_MainMenuInput;
    TbFloat32                           m_TitleInputChangeTimer;

};

#endif
