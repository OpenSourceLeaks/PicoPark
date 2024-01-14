/*!
 * ワールド管理クラス
 * @author teco
 */

#include "fwd.h"
#include "world_manager.h"
#include "common/cmn_global_data.h"
#include "root.h"
#include "common/cmn_util.h"

/*!
 * コンストラクタ
 * @author teco
 */
WorldManager::WorldManager()
    : m_IsOpenedAllWorld(TB_FALSE)
{
}

/*!
 * 初期化
 * @author teco
 */
void WorldManager::Initialize(TbScriptLuaNode* node)
{
    TB_RESQUE(node);
    {
        // ワールド情報
        TbSint32 number = 1;
        if (TbScriptLuaNode::RESULT_OK == node->GoToChild("world")) {
            TbUint32 worldCount;
            node->TryToTableLength(worldCount);
            m_WorldInfo.PushBackDefault(worldCount);
            for (TbUint32 i = 0; i < worldCount; ++i) {
                const char* name = NULL;
                node->GoToChild(i);
                if (TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name, static_cast<TbSint32>(0))) {
                    m_WorldInfo[i].name = name;
                }
                TbSint32 enable = 0;
                if (TbScriptLuaNode::RESULT_OK == node->TryToSint32ByIndex(enable, static_cast<TbSint32>(1))) {
                    m_WorldInfo[i].isEnable = enable;
                }
                {
                    TbSint32 uv = 0;
                    node->TryToSint32ByIndex(uv, static_cast<TbSint32>(2));
                    m_WorldInfo[i].uvOffset.SetX(CMN_TEX_POS(80.0f)*uv);
                    uv = 0;
                    node->TryToSint32ByIndex(uv, static_cast<TbSint32>(3));
                    uv += 1;
                    m_WorldInfo[i].uvOffset.SetY(CMN_TEX_POS(80.0f)*uv);
                }
                {
                    node->GoToChild(4);
                    TbUint32 stageCount = 0;
                    node->TryToTableLength(stageCount);
                    m_WorldInfo[i].stageCount = stageCount;
                    for (TbUint32 j = 0; j < stageCount; ++j) {
                        node->TryToSint32ByIndex(m_WorldInfo[i].stage[j].id,j);
                        m_WorldInfo[i].stage[j].number = number;
                        ++number;
                    }
                    node->GoToParent(1);
                }
                node->GoToParent(1);
            }
            node->GoToParent(1);
        }

        // バトルステージ情報
        if (TbScriptLuaNode::RESULT_OK == node->GoToChild("battleStage")) {
            enum
            {
                PARAM_NAME  ,
                PARAM_UV_X  ,
                PARAM_UV_Y  ,
                PARAM_STAGE ,
            };
            TbUint32 stageCount;
            node->TryToTableLength(stageCount);
            m_BattleStageInfo.PushBackDefault(stageCount);
            for (TbUint32 i = 0; i < stageCount; ++i) {
                const char* name = NULL;
                node->GoToChild(i);
                if (TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name, static_cast<TbSint32>(PARAM_NAME))) {
                    m_BattleStageInfo[i].name = name;
                }
                {
                    TbSint32 uv = 0;
                    node->TryToSint32ByIndex(uv, static_cast<TbSint32>(PARAM_UV_X));
                    m_BattleStageInfo[i].uvOffset.SetX(CMN_TEX_POS(80.0f)*uv);
                    uv = 0;
                    node->TryToSint32ByIndex(uv, static_cast<TbSint32>(PARAM_UV_Y));
                    uv += 1;
                    m_BattleStageInfo[i].uvOffset.SetY(CMN_TEX_POS(80.0f)*uv);
                }
                {
                    node->TryToSint32ByIndex(m_BattleStageInfo[i].stage, PARAM_STAGE);
                }
                node->GoToParent(1);
            }
            node->GoToParent(1);
        }

        // エンドレス
        if (TbScriptLuaNode::RESULT_OK == node->GoToChild("endlessStage")) {
            enum
            {
                PARAM_NAME,
                PARAM_UV_X,
                PARAM_UV_Y,
                PARAM_STAGE,
            };
            TbUint32 stageCount;
            node->TryToTableLength(stageCount);
            m_EndlessStageInfo.PushBackDefault(stageCount);
            for (TbUint32 i = 0; i < stageCount; ++i) {
                const char* name = NULL;
                node->GoToChild(i);
                if (TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name, static_cast<TbSint32>(PARAM_NAME))) {
                    m_EndlessStageInfo[i].name = name;
                }
                {
                    TbSint32 uv = 0;
                    node->TryToSint32ByIndex(uv, static_cast<TbSint32>(PARAM_UV_X));
                    m_EndlessStageInfo[i].uvOffset.SetX(CMN_TEX_POS(80.0f)*uv);
                    uv = 0;
                    node->TryToSint32ByIndex(uv, static_cast<TbSint32>(PARAM_UV_Y));
                    uv += 1;
                    m_EndlessStageInfo[i].uvOffset.SetY(CMN_TEX_POS(80.0f)*uv);
                }
                {
                    node->TryToSint32ByIndex(m_EndlessStageInfo[i].stage, PARAM_STAGE);
                }
                node->GoToParent(1);
            }
            node->GoToParent(1);
        }

    }
}

/*!
 * ステージIDからワール09ド取得
 * @author teco
 */
TbSint32 WorldManager::GetWorldFromStageId(TbSint32 stageId, TbSint32* stageIndex) const
{
    TbSint32 ret = -1;
    if (stageIndex)
    {
        *stageIndex = 0;
    }
    for (TbUint32 i = 0; i < m_WorldInfo.GetCount(); ++i)
    {
        for (TbUint32 j = 0; j < m_WorldInfo[i].stageCount; ++j)
        {
            if (stageId == m_WorldInfo[i].stage[j].id) {
                ret = i;
                if (stageIndex)
                {
                    *stageIndex = j;
                }
                return ret;
            }
        }
    }
    return ret;
}

/*!
 * 全ワールド内のラストステージか
 * @author teco
 */
TbBool WorldManager::IsLastStageInAllWorld(TbSint32 stageId) const
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    TbBool result = TB_FALSE;
    TbUint32 clearCount = SaveFile::GetInstance().GetClearCount(stageId, playerCount);
    if (IsWorldStage(stageId) && clearCount == 0)
    {
        result = TB_TRUE;
        TbUint32 unclearedStage = 0;
        TbSint32 worldCount = GetWorldCount();
        for (TbSint32 i = 0; i < worldCount; ++i)
        {
            const WorldInfo* info = &m_WorldInfo[i];
            for (TbUint32 j = 0; j < info->stageCount; ++j)
            {
                TbSint32 id = info->stage[j].id;
                if (id != stageId)
                {
                    // 他で未クリアのステージがあればまだラストではな
                    if (SaveFile::GetInstance().GetClearCount(id, playerCount) == 0)
                    {
                        result = TB_FALSE;
                        break;
                    }
                }
            }
            if (!result) {
                break;
            }
        }
    }
    return result;
}

/*!
 * 次のステージ取得
 * @author teco
 */
TbSint32 WorldManager::GetNextStage(TbSint32 stageId)
{
    TbSint32 index = -1;
    if (IsBattleStage(stageId, &index))
    {
        const BattleStageInfo* info = GetBattleStageInfo(index+1);
        if (info) {
            return info->stage;
        }
    }
    else if(IsEndlessStage(stageId, &index))
    {
        const EndlessStageInfo* info = GetEndlessStageInfo(index + 1);
        if (info) {
            return info->stage;
        }
    }
    else
    {
        TbSint32 world = GetWorldFromStageId(stageId,&index);
        const WorldInfo* info = GetWorldInfo(world);
        if (index < info->stageCount - 1) {
            return info->stage[index+1].id;
        }else if( world < GetWorldCount()-1 ) {
            const WorldInfo* nextInfo = GetWorldInfo(world+1);
            return nextInfo->stage[0].id;
        }
    }
    return -1;
}

/*!
 * 最新のステージ
 * @author teco
 */
WorldManager::UpdateInfoResult WorldManager::UpdateInfo(TbSint32 worldIndex)
{
    UpdateInfoResult result;
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    WorldInfo* info = nullptr;
    if ( 0 <= worldIndex && worldIndex < GetWorldCount()) {
        info = &m_WorldInfo[worldIndex];
    }
    if (!TB_VERIFY(info)) {
        return result;
    }
    TbSint32 latest = 0;
    SaveFile& data = SaveFile::GetInstance();
    if (!info->isWorldClearAny)
    {
        info->isWorldClearAny = TB_TRUE;
        for (TbSint32 i = 0; i < info->stageCount; ++i) {
            if (0 == data.GetClearCount(info->stage[i].id)) {
                info->isWorldClearAny = TB_FALSE;
                break;
            }
            ++latest;
        }
        latest = TbMin<TbSint32>(latest, info->stageCount - 1);
        result.isUpdatedLatestStage = latest != info->latestStage;
        info->latestStage = latest;
    }

    for (TbSint32 p = CMN_PLAYER_MIN; p < CMN_PLAYER_MAX; ++p)
    {
        TbSint32 index = p-1;
        if (!info->isWorldClear[index])
        {
            if (p == playerCount) {
                result.isUpdatedWorldClear = TB_TRUE;
            }
            info->isWorldClear[index] = TB_TRUE;
            for (TbSint32 i = 0; i < info->stageCount; ++i) {
                if (0 == data.GetClearCount(info->stage[i].id, p)) {
                    if (p == playerCount) {
                        result.isUpdatedWorldClear = TB_FALSE;
                    }
                    info->isWorldClear[index] = TB_FALSE;
                    break;
                }
            }
        }
    }

    return result;
}

/*!
 * 全ワールドオープン
 * @author teco
 */
TbBool  WorldManager::UpdateOpenAllWorld()
{
    TbBool ret = TB_FALSE;
    WorldInfo* firstWorldInfo = &m_WorldInfo[0];
    // 最初のワールドがクリアだったら
    TbSint32 worldCount = GetWorldCount();
    for (TbSint32 i = 1; i < worldCount; ++i)
    {
        WorldInfo* info = &m_WorldInfo[i];
        if (firstWorldInfo->isWorldClearAny)
        {
            if (!info->isEnable)
            {
                info->isEnable = TB_TRUE;
                m_IsOpenedAllWorld = TB_TRUE;
                ret = TB_TRUE;
            }
        }
    }
    return TB_TRUE;
}
