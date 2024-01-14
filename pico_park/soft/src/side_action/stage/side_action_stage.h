/*!
 * アクションステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_STAGE_H_
#define _INCLUDED_SIDE_ACTION_STAGE_H_

#include "side_action/collision/side_action_collision_scene.h"
#include "side_action/actor/side_action_goal.h"
#include "side_action/stage/side_action_actor_manager.h"
#include "common/cmn_simple_sprite.h"
#include <script/lua/tb_script_lua.h>
#include <crayon/map/cr_action_map.h>
#include <crayon/stage/cr_action_stage.h>

class SideActionPlayer;
class SideActionGoal;

class SideActionStage : public CrActionStage
{
public:

    typedef SideActionStage Self;
    typedef CrActionStage Super;

    enum ScrollType
    {
        SCROLL_NONE     , // スクロールしない
        SCROLL_NORMAL   , // 通常スクロール
        SCROLL_AUTO       // 自動スクロール
    };
    
public:

    // コンストラクタ
    SideActionStage( TbScriptLuaNode* node );

    // デストラクタ
    virtual ~SideActionStage();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public:
    
    // プレイヤー取得
    SideActionPlayer* GetPlayer( TbUint32 index ) {
        return m_ActorManager.GetPlayer(index);
    }

    // プレイヤー取得
    const SideActionPlayer* GetPlayer( TbUint32 index ) const {
        return m_ActorManager.GetPlayer(index);
    }

    // プレイヤー数取得
    TbUint32 GetPlayerCount() const {
        return m_ActorManager.GetPlayerCount();
    }

    // クリアしたプレイヤー数取得
    TbUint32 GetClearedPlayerCount() const {
        return m_ClearedPlayerCount;
    }

    // ゴールを強制開く
    void RequestOpanGoal() {
        m_ActorManager.RequestOpanGoal();
    }

    // ゴール取得
    SideActionGoal* GetGoal() const {
        return m_ActorManager.GetGoal();
    }

    // ゲームオーバー不可
    void SetDisableGameOver() {
        m_BitArray.SetBit(FLAG_DISABLE_GAMEOVER);
    }

    // スクロール可能か
    TbBool IsScrollable() const {
        return m_ScrollParam.type != SCROLL_NONE;
    }

    // 自動スクロール中
    TbBool IsAutoScroll() const {
        return m_ScrollParam.type == SCROLL_AUTO;
    }

    // スクロールタイプ
    void SetScrollType(ScrollType type)
    {
        m_ScrollParam.type = type;
    }

    // 自動スクロール量計算
    TbFloat32 CalcAutoScrollSpeed() const;

    // 自動スクロール速度取得
    TbFloat32 GetAutoScrollSpeed() const
    {
        return m_ScrollParam.autoSpeed;
    }

    // 自動スクロール量設定
    void SetAutoScrollSpeed( TbFloat32 speed )
    {
        m_ScrollParam.autoSpeed = speed;
    }

    // スクロール制限
    void SetScrollLimit(TbFloat32 limit) {
        if( 0.0f <= limit ){
            TB_RESQUE(GetScrollX() <= limit);
        }
        m_ScrollParam.limit = limit;
    }

    // スクロール制限解除
    void ResetScrollLimit() {
        SetScrollLimit(-1.0f);
    }

    // スクロールリクエスト
    void RequestScroll(TbFloat32 scroll) {
        m_ScrollParam.request = scroll;
    }

    // 範囲取得
    TbRangeF32 GetPlayerRangeX() const {
        return m_PlayerRangeX;
    }

    // 画面端を移動可能か
    TbFloat32 CalcScreenEndWalkVelo( SideActionPlayer* player , TbFloat32 velo );

    // 画面の横幅取得
    TbFloat32 GetWindowWidth() const {
        return CMN_WINDOW_WIDTH / GetScale();
    }

    // 画面の横幅取得
    TbFloat32 GetWindowHeight() const {
        return CMN_WINDOW_HEIGHT / GetScale();
    }

    // 失敗してるY
    TbFloat32 GetFailFallY() const {
        return m_FailFallY;
    }
    
    // 失敗する高さY
    TbFloat32 GetFailUpY() const {
        return m_FailUpY;
    }

    // クリア時フォントカラー取得
    TbColor GetClearFontColor() const {
        return m_ClearFontColor;
    }

    // 暗闇ステージか
    TbBool IsDarkness() const {
        return m_BitArray.Test(FLAG_DARKNESS);
    }

    // 暗闇解除
    void ResetDarkness() {
        if (IsDarkness())
        {
            m_BitArray.ResetBit(FLAG_DARKNESS);
            TbBool isVIsible = TB_TRUE;
            GetMap().SetVisible(isVIsible);
            NotifyEvent(nullptr, SIDE_ACTION_EVENT_SET_VISIBLE, &isVIsible);
        }
    }

    // プレイヤーのシャッフル番号からプレイヤー番号取得
    TbUint32 GetPlayerIndexFromShufferIndex(TbUint32 index) {
        return m_ActorManager.GetPlayerIndexFromShufferIndex(index);
    }

    // プレイヤーのシャッフル番号取得
    TbUint32 GetPlayerShuffleIndex(TbUint32 playerIndex) {
        return m_ActorManager.GetPlayerShuffleIndex(playerIndex);
    }

public: // エディット用

    // スクロール一時停止
    void SetPauseScroll(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_PAUSE_SCROLL,isEnable);
    }
    TbBool IsPauseScroll() const {
        return m_BitArray.Test(FLAG_PAUSE_SCROLL);
    }

    // アクター生成禁止
    void SetEnableCreateActor( TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_CREATE_ACTOR, isEnable);
    }
    TbBool IsEnableCreateActor() const {
        return m_BitArray.Test(FLAG_ENABLE_CREATE_ACTOR);
    }

    // ポーズ利用可能か
    void SetEnablePause(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_PAUSE,isEnable);
    }
    TbBool IsEnablePause() const {
        return m_BitArray.Test(FLAG_ENABLE_PAUSE);
    }

    // 自動スクロールの終端オフセット
    TbFloat32 GetAutoScrollEndOffset() {
        return m_ScrollParam.autoEndOffset;
    }

    // 通常のスクロール速度の限界値を設定
    void SetEnableScrollNormalSpeedMax( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_DISABLE_NORMAL_SCROLL_SPEED_MAX,!isEnable);
    }

    // 通常のスクロール速度の限界値を設定
    TbBool IsEnableScrollNormalSpeedMax() const {
        return !m_BitArray.Test(FLAG_DISABLE_NORMAL_SCROLL_SPEED_MAX);
    }

    void SetEnablePlayerSortY() {
        m_BitArray.SetBit(FLAG_PLAYER_SORT_Y);
    }

    TbBool IsEnablePlayerSortY() {
        return m_BitArray.Test(FLAG_PLAYER_SORT_Y);
    }

private:
    
    enum Flag
    {
        FLAG_READY , 
        FLAG_ACTIVE , 
        FLAG_INIT_SCROLL ,
        FLAG_UNLIMIT_SCROLL ,
        FLAG_UNRETURN_SCROLL,
        FLAG_SHUFFLE_PLAYER ,
        FLAG_DISABLE_GAMEOVER ,
        FLAG_PAUSE_SCROLL ,
        FLAG_ENABLE_CREATE_ACTOR ,
        FLAG_DARKNESS ,
        FLAG_ENABLE_PAUSE , 
        FLAG_PLAYER_SORT_Y ,
        FLAG_DISABLE_NORMAL_SCROLL_SPEED_MAX
    };

protected:

    // 有効状態が変化した
    virtual void OnChangedActive(TbBool isActive) TB_OVERRIDE;

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // 描画
    virtual void OnDraw( TbFloat32 deltatime ) TB_OVERRIDE;

    // 手前描画
    virtual void OnDrawFront( TbFloat32 deltatime ) TB_OVERRIDE;

    // アクター生成
    virtual CrActor* OnCreatedActor( const CrActorFactory::CreateParam& param ) TB_OVERRIDE;

private:

    typedef TbStaticArray<SideActionPlayer*,CMN_PLAYER_MAX*2> PlayerList;
    typedef TbStaticArray<TbUint32, CMN_PLAYER_MAX > PlayerIndexList;

private:

    // スクロール更新
    void updateScroll();

    // クリア状態更新
    void updateClear();

    // クリアと判断可能なワープ銃にいるプレイヤー
    void calcClearedWarpPlayer( const SideActionPlayer* clearedPlayer , const SideActionPlayer** list , TbUint32* count );

    // ステージクリアしたか
    TbBool isCleared() const {
        return GetPlayerCount() != 0 && m_ClearedPlayerCount == GetPlayerCount();
    }

    // ゲームオーバー
    TbBool isGameOver() const;

    // プレイヤー範囲取得
    TbRangeF32 calcPlayerRange(TbFloat32* leftVelo, TbFloat32* rightVelo , TbBool isDrawPos ) const;

private:

    static const TbUint32 NORMAL_SCROLL_SPEED_MAX = 3;

    struct ScrollParam
    {
        ScrollParam()
            : type( SCROLL_NONE )
            , limit(0.0f)
            , request(0.0f)
            , autoSpeed(0.0f)
            , autoEndOffset(0.0f)
            , normalSpeedMax(static_cast<TbFloat32>(NORMAL_SCROLL_SPEED_MAX))
            , normalSpeedMaxRatio(1.0f)
        {
        }
        ScrollType type;
        TbFloat32  limit;
        TbFloat32  request;
        TbFloat32  autoSpeed;
        TbFloat32  autoEndOffset;
        TbFloat32  normalSpeedMax;
        TbFloat32  normalSpeedMaxRatio;
    };

private:

    TbBitArray32                m_BitArray;
    ScrollParam                 m_ScrollParam;
    TbRangeF32                  m_PlayerRangeX;
    TbFloat32                   m_FailFallY;
    TbFloat32                   m_FailUpY;
    TbColor                     m_ClearFontColor;
    CmnWhiteSprite              m_DarkSprite;

private: // アクター管理

    SideActionActorManager      m_ActorManager;
    TbUint32                    m_ClearedPlayerCount;

};

#endif
