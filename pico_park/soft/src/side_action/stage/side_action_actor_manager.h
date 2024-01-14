/*!
 * アクションステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_ACTOR_MANAGER_H_
#define _INCLUDED_SIDE_ACTION_ACTOR_MANAGER_H_

#include "side_action/collision/side_action_collision_scene.h"
#include "side_action/actor/side_action_goal.h"
#include "side_action/actor/side_action_player_dress.h"
#include <script/lua/tb_script_lua.h>
#include <crayon/stage/cr_action_stage.h>

class SideActionPlayer;
class SideActionGoal;
class SideActionGhost;

class SideActionActorManager
{    
public:

    // コンストラクタ
    SideActionActorManager();

    // デストラクタ
    virtual ~SideActionActorManager();

public:

    // プレイヤー数制限
    void SetEnableLimitPlayer(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_LIMIT_PLAYER_COUNT, isEnable);
    }

    // プレイヤー数制限
    TbBool IsEnableLimitPlayer() const {
        return m_BitArray.Test(FLAG_LIMIT_PLAYER_COUNT);
    }

    // プレイヤーシャッフル
    void ShufflePlayer() {
        TbShuffle(m_PlayerIndexList.Begin(), m_PlayerIndexList.End());
    }

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

    // 幽霊取得
    SideActionGhost* GetGhost() const {
        return m_Ghost;
    }

    // プレイヤーのシャッフル番号からプレイヤー番号取得
    TbUint32 GetPlayerIndexFromShufferIndex(TbUint32 index) {
        TB_RESQUE_RET(index<m_PlayerIndexList.GetCount(),0);
        return m_PlayerIndexList[index];
    }

    // プレイヤーのシャッフル番号取得
    TbUint32 GetPlayerShuffleIndex(TbUint32 playerIndex) {
        for ( TbUint32 i = 0; i < m_PlayerIndexList.GetCount(); ++i ) {
            if (m_PlayerIndexList[i] == playerIndex) {
                return i;
            }
        }
        return 0;
    }

private:
    
    enum Flag
    {
        FLAG_LIMIT_PLAYER_COUNT , 
    };

public:

    // アクター生成
    CrActor* CreateActor( const CrActorFactory::CreateParam& param , CrActionStage* stage );

private:

    typedef TbStaticArray<SideActionPlayer*,CMN_PLAYER_MAX*2>   PlayerList;
    typedef TbStaticArray<TbUint32, CMN_PLAYER_MAX >            PlayerIndexList;

private:

    // プレイヤー生成
    SideActionPlayer* createPlayer( TbVector2* initPos ,
                                    CrActionStage* stage , 
                                    SideActionPlayerHeaddress::Type type = SideActionPlayerHeaddress::TYPE_NUMBER , 
                                    TbBool isEnableHeadCollision = TB_FALSE );

    // 特殊プレイヤー生成
    SideActionPlayer* createSpecialPlayer( TbVector2* initPos );

private: // 特殊所持アクター

    TbBitArray32                m_BitArray;
    PlayerList                  m_Player;
    PlayerIndexList             m_PlayerIndexList;
    SideActionGoal*             m_Goal;
    SideActionGhost*            m_Ghost;
    TbUint32                    m_SpecialPlayerCount;
    TbUint32                    m_RouletteCount;

};

#endif
