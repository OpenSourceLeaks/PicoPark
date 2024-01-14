/*!
 * プレイヤーアクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_H_
#define _INCLUDED_ACTION_PLAYER_H_

#include <graphics/sprite/tb_sprite.h>
#include <input/tb_bit_input.h>
#include "common/actor/action_actor.h"
#include "side_action/actor/state/side_action_player_state.h"
#include "system/sys_majority_input.h"
#include "side_action_player_input.h"
#include "common/material/cmn_sprite_player_material.h"

class SideActionPlayer;

#define SIDE_ACTION_PLAYER_ENABLE_TITLE (CMN_ENABLE_BUILD_ARCADE)

class SideActionPlayer : public ActionActor
{
public:

    typedef SideActionPlayer Self;
    typedef ActionActor Super;

    // アニメーション
    enum AnimType
    {
        ANIM_DEFAULT , // 静止
        ANIM_WALK , 
        ANIM_JUMP ,
        ANIM_PUSH_WALK ,
        ANIM_DEAD , 
        ANIM_SLEEP ,
        ANIM_MAX ,
    };

    enum State
    {
        STATE_NONE    ,
        STATE_NULL    , 
        STATE_DEFAULT , 
        STATE_DEAD    , 
        STATE_CLEAR   ,
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

public:

    // コンストラクタ
    SideActionPlayer( TbUint32 playerIndex );

    // デストラクタ
    virtual ~SideActionPlayer();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public:

    // プレイヤー
    TbUint32 GetPlayerIndex() const {
        return m_PlayerIndex;
    }

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

    TbBool IsGameOver() const {
        return m_BitArray.Test(FLAG_GAME_OVER) ||
               GetState() == SideActionPlayer::STATE_NULL;
    }

    void SetEnableAirWalk( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_AIR_WALK,isEnable);
    }

    TbBool IsEnableAirWalk() const {
        return m_BitArray.Test(FLAG_AIR_WALK);
    }

    // 空中ジャンプ可能か
    void SetEnableAirJump( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_AIR_JUMP,isEnable);
    }

    // 空中ジャンプ可能か
    TbBool IsEnableAirJump() const {
        return m_BitArray.Test(FLAG_ENABLE_AIR_JUMP);
    }

    // ライフ追加
    void AddLife() {
        ++m_Life;
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
    TbBool IsOnInput( SysInput::Button button ) const;

    // 押した
    TbBool IsPressInput( SysInput::Button button ) const;

    // 離した
    TbBool IsReleaseInput( SysInput::Button button ) const;

    // ジャンプ速度取得
    TbFloat32 GetJumpVelo() const;
    
    // 歩行速度取得
    TbFloat32 GetWalkVelo() const;

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 後処理更新
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

protected:

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) TB_OVERRIDE
    {
        AddVelo(pushBackVec);
    }

private:

    enum Flag
    {
        FLAG_AIR_WALK ,
        FLAG_ENABLE_AIR_JUMP , // 空中ジャンプ可能
        FLAG_GAME_OVER ,
        FLAG_TITLE , 
        FLAG_MAX
    };

private:

    // 接触開始コールバック
    virtual void onContactAdded(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& info);

    // 状態更新
    SideActionPlayerState* createState( State state );

    // 浮いているいか
    TbBool isFloating( const ActionCollision* collision ) const;

private:

    TbBitArray32            m_BitArray;
    TbUint32                m_PlayerIndex;
    SideActionPlayerState*  m_StateImpl;
    State                   m_State;
    State                   m_RequestedState;
    TbSint32                m_Life;
    TbSint32                m_WakeUpInitCount;
    TbSint32                m_WakeUpCount;

private:

    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim[ANIM_MAX];
    CmnSpritePlayerMaterial m_Material;
    AnimType                m_CurrentAnim;

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
