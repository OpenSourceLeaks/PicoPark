/*!
 * アクションステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_STAGE_H_
#define _INCLUDED_SIDE_ACTION_STAGE_H_

#include "common/stage/stage.h"
#include "common/map/action_map.h"
#include "common/stage/action_stage.h"
#include "side_action/collision/side_action_collision_scene.h"
#include "side_action/actor/side_action_goal.h"
#include <script/lua/tb_script_lua.h>

class SideActionPlayer;
class SideActionGoal;
class Actor;

class SideActionStage : public ActionStage
{
    typedef SideActionStage Self;
    typedef ActionStage Super;
    
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
        if( index < GetPlayerCount() ){
            return m_Player.GetAt(index);
        }
        return NULL;
    }

    // プレイヤー取得
    const SideActionPlayer* GetPlayer( TbUint32 index ) const {
        if( index < GetPlayerCount() ){
            return m_Player.GetAt(index);
        }
        return NULL;
    }

    // プレイヤー数取得
    TbUint32 GetPlayerCount() const {
        return m_Player.GetCount();
    }

    // クリアしたプレイヤー数取得
    TbUint32 GetClearedPlayerCount() const {
        return m_ClearedPlayerCount;
    }

    // ゴールを強制開く
    void RequestOpanGoal() {
        if( m_Goal ){
            m_Goal->Open();
        }
    }

    // ゴール取得
    SideActionGoal* GetGoal() const {
        return m_Goal;
    }

    // ゲームオーバー不可
    void SetDisableGameOver() {
        m_BitArray.SetBit(FLAG_DISABLE_GAMEOVER);
    }

    // スクロール可能か
    TbBool IsScrollable() const {
        return m_BitArray.Test(FLAG_SCROLLABLE);
    }

    // 自動スクロール中
    TbBool IsAutoScroll() const {
        return m_BitArray.Test(FLAG_AUTO_SCROLL);
    }

    TbFloat32 GetAutoScrollSpeed() const {
        return m_AutoScrollSpeed;
    }

    // スクロール制限
    void SetScrollLimit(TbFloat32 limit) {
        if( 0.0f <= limit ){
            TB_RESQUE(GetScroll() <= limit);
        }
        m_ScrollLimit = limit;
    }

    // スクロール制限解除
    void ResetScrollLimit() {
        SetScrollLimit(-1.0f);
    }

    // スクロールリクエスト
    void RequestScroll(TbFloat32 scroll) {
        m_RequestScroll = scroll;
    }

    // 範囲取得
    TbRangeF32 GetPlayerRangeX() const {
        return m_PlayerRangeX;
    }

    // 画面端を移動可能か
    TbFloat32 CalcScreenEndWalkVelo( SideActionPlayer* player , TbFloat32 velo );

private:
    
    enum Flag
    {
        FLAG_READY , 
        FLAG_ACTIVE , 
        FLAG_SCROLLABLE , 
        FLAG_AUTO_SCROLL ,
        FLAG_DISABLE_GAMEOVER ,
        FLAG_LIMIT_PLAYER_COUNT ,
    };

protected:

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime );

    // 描画
    virtual void OnDraw( TbFloat32 deltatime );

    // 手前描画
    virtual void OnDrawFront( TbFloat32 deltatime );

    // アクター生成
    virtual void OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData& userData);

private:

    typedef TbStaticArray<SideActionPlayer*,CMN_PLAYER_MAX*2> PlayerList;

private:

    // スクロール更新
    void updateScroll();

    // クリア状態更新
    void updateClear();

    // ステージクリアしたか
    TbBool isCleared() const {
        return m_ClearedPlayerCount == m_Player.GetCount();
    }

    // ゲームオーバー
    TbBool isGameOver() const;

    // プレイヤー生成
    SideActionPlayer* createPlayer( TbVector2* initPos );

    // 特殊プレイヤー生成
    SideActionPlayer* createSpecialPlayer( TbVector2* initPos );

private:

    TbBitArray32                m_BitArray;
    TbFloat32                   m_AutoScrollSpeed;
    TbFloat32                   m_ScrollLimit;
    TbFloat32                   m_RequestScroll;
    TbRangeF32                  m_PlayerRangeX;

private: // 特殊所持アクター

    PlayerList                  m_Player;
    SideActionGoal*             m_Goal;
    TbUint32                    m_ClearedPlayerCount;
    TbUint32                    m_SpecialPlayerCount;
    TbUint32                    m_RouletteCount;

};

#endif
