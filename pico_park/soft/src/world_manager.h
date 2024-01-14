/*!
 * ワールド管理クラス
 * @author teco
 */

#ifndef _INCLUDED_WORLD_MANAGER_H_
#define _INCLUDED_WORLD_MANAGER_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include <script/lua/tb_script_lua_node.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"

struct WorldInfo
{
    struct WorldStageInfo
    {
        WorldStageInfo()
            : id(0)
            , number(0)
        {
        }
        TbSint32 id;
        TbSint32 number;
    };
    static const TbSint32 STAGE_MAX = 5;
    WorldInfo() : isEnable(TB_FALSE) 
                , stageCount(0) 
                , latestStage(0) 
                , isWorldClearAny(TB_FALSE)
    {
        TbFill(isWorldClear,TB_FALSE);
    }

    TbStaticString32 name;
    TbVector2        uvOffset;
    TbSint32         isEnable;
    TbSint32         stageCount;
    TbSint32         latestStage;
    TbBool           isWorldClear[CMN_PLAYER_MAX];
    TbBool           isWorldClearAny;
    WorldStageInfo   stage[STAGE_MAX];
};

struct BattleStageInfo
{
    BattleStageInfo() : stage(0) {}

    TbStaticString32 name;
    TbVector2        uvOffset;
    TbSint32         stage;
};

struct EndlessStageInfo
{
    EndlessStageInfo() : stage(0) , hiscore(0) {}

    TbStaticString32 name;
    TbVector2        uvOffset;
    TbSint32         stage;
    TbSint32         hiscore;
};

class WorldManager : public TbSingletonRef<WorldManager>
{
public:

    struct UpdateInfoResult
    {
        // コンストラクタ
        UpdateInfoResult()
            : isUpdatedLatestStage(TB_FALSE)
            , isUpdatedWorldClear(TB_FALSE)
        {
        }
        TbBool isUpdatedLatestStage;
        TbBool isUpdatedWorldClear;
    };

public:

    // コンストラクタ
    WorldManager();

    // デストラクタ
    virtual ~WorldManager() {}

private:

    TB_DISALLOW_COPY_AND_ASSIGN(WorldManager);

public:

    // 初期化
    void Initialize( TbScriptLuaNode* node);

public:

    // ステージIDからワールド取得
    TbSint32 GetWorldFromStageId(TbSint32 stageId,TbSint32* stageIndex) const;

    // ワールド数取得
    TbSint32 GetWorldCount() const {
        return m_WorldInfo.GetCount();
    }

    // ワールド情報取得
    const WorldInfo* GetWorldInfo(TbSint32 index) const {
        if (index < GetWorldCount()) {
            return &m_WorldInfo[index];
        }
        return nullptr;
    }

    // 全ワールド内のラストステージか
    TbBool IsLastStageInAllWorld(TbSint32 stageId) const;

    // ワールド内のラストステージか
    TbBool IsLastStageInWorld(TbSint32 stageId) const {
        TbSint32 index = 0;
        TbSint32 world = GetWorldFromStageId(stageId,&index);
        if ( 0 <= world )
        {
            const WorldInfo* info = GetWorldInfo(world);
            if (index == info->stageCount - 1) {
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

    // 次のステージ取得
    TbSint32 GetNextStage(TbSint32 stageId);


public:

    // バトルステージ数取得
    TbSint32 GetBattleStageCount() const {
        return m_BattleStageInfo.GetCount();
    }

    // バトルステージ情報取得
    const BattleStageInfo* GetBattleStageInfo(TbSint32 index) const {
        if (index < GetBattleStageCount()) {
            return &m_BattleStageInfo[index];
        }
        return nullptr;
    }

    // バトルステージか
    TbBool IsBattleStage(TbSint32 stageId, TbSint32* index = nullptr) const {
        for (TbSint32 i = 0; i < static_cast<TbSint32>(m_BattleStageInfo.GetCount()); ++i) {
            if (m_BattleStageInfo[i].stage == stageId)
            {
                if (index) {
                    *index = i;
                }
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

    // ステージか
    TbBool IsWorldStage(TbSint32 stageId ) const {
        TbSint32 index = 0;
        return 0 <= GetWorldFromStageId(stageId,&index);
    }

    // 全ワールドオープンの条件がそろっているか
    TbBool           IsOpenedAllWorld() const {
        return m_IsOpenedAllWorld;
    }

public:

    // エンドレスステージ数取得
    TbSint32 GetEndlessStageCount() const {
        return m_EndlessStageInfo.GetCount();
    }

    // エンドレスステージ情報取得
    const EndlessStageInfo* GetEndlessStageInfo(TbSint32 index) const {
        if (index < GetEndlessStageCount()) {
            return &m_EndlessStageInfo[index];
        }
        return nullptr;
    }

    // エンドレスステージか
    TbBool IsEndlessStage(TbSint32 stageId, TbSint32* index = nullptr ) const {
        for (TbSint32 i = 0; i < static_cast<TbSint32>(m_EndlessStageInfo.GetCount()); ++i) {
            if (m_EndlessStageInfo[i].stage == stageId)
            {
                if(index){
                    *index = i;
                }
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

public:

    // 最新のステージ
    UpdateInfoResult UpdateInfo( TbSint32 worldIndex );

    // 全ワールドオープン
    TbBool           UpdateOpenAllWorld();

private:

    TbBool                              m_IsOpenedAllWorld;
    TbStaticArray<WorldInfo,32>         m_WorldInfo;
    TbStaticArray<BattleStageInfo,32>   m_BattleStageInfo;
    TbStaticArray<EndlessStageInfo,32>  m_EndlessStageInfo;

};

#endif