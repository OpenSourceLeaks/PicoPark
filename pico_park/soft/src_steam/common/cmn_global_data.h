/*!
 * グローバルデータ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CMN_GLOBAL_DATA_H_
#define _INCLUDED_CMN_GLOBAL_DATA_H_

// チェックポイント
struct CheckPoint
{
    CheckPoint() : isActive(TB_FALSE) , sec(0.0f) {}
    TbBool      isActive;
    TbVector2   pos;
    TbFloat32   sec;
};

// ステージデータ
struct StageData
{
    static const TbUint32 TIME_INVALID = 0xFFFFFFFF;

    StageData()
        : playCount(0)
        , clearCount(0)
    {
        TbFill(bestTimeMs,TIME_INVALID);
    }
    TbUint32    playCount;
    TbUint32    clearCount;
    TbUint32    bestTimeMs[CMN_PLAYER_MAX];
    CheckPoint  checkPoint[CMN_PLAYER_MAX];
};

struct WorldData
{
    static const TbUint32 STAGE_MAX = 10;
    WorldData()
    {
        TbFillZero(clearCount);
        TbFillZero(clearFlag);
    }
    TbUint32 clearCount[STAGE_MAX];
    TbBool   clearFlag[STAGE_MAX];
};

// グローバルデータ
class CmnGlobalData
{
public:
    
    static const TbUint32 STAGE_MAX = 100;
    static const TbUint32 WORLD_MAX = 100;

public:
    
    // コンストラクタ
    CmnGlobalData() {}
    
    // デストラクタ
    ~CmnGlobalData() {}
    
public: // チェックポイント
    
    // チェックポイント設定
    void SetCheckPoint( TbUint32 stage , TbUint32 player , const TbVector2& pos , TbFloat32 sec ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && player < TB_ARRAY_COUNT_OF(m_StageData[stage].checkPoint) ){
            m_StageData[stage].checkPoint[player].pos = pos;
            m_StageData[stage].checkPoint[player].sec = sec;
            m_StageData[stage].checkPoint[player].isActive = TB_TRUE;
        }
    }
  
    // チェックポイントリセット
    void ResetCheckPoint( TbUint32 stage , TbUint32 player ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && player < TB_ARRAY_COUNT_OF(m_StageData[stage].checkPoint) ){
            m_StageData[stage].checkPoint[player].isActive = TB_FALSE;
        }
    }

    // チェックポイントリセット
    void ResetCheckPoint() 
    {
        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_StageData); ++i ) {
            TbFill( m_StageData[i].checkPoint , CheckPoint() );
        }
    }
    
    // チェックポイント取得
    TbBool GetCheckPoint( TbVector2& dst , TbFloat32& dstSec , TbUint32 stage , TbUint32 player ) const {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && player < TB_ARRAY_COUNT_OF(m_StageData[stage].checkPoint) ){
            if( m_StageData[stage].checkPoint[player].isActive ) {
                dst = m_StageData[stage].checkPoint[player].pos;
                dstSec = m_StageData[stage].checkPoint[player].sec;
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

public:

    // プレイ回数設定
    void SetPlayCount( TbUint32 stage , TbUint32 playCount ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) ){
            m_StageData[stage].playCount = playCount;
        }
    }
  
    // プレイ回数カウントアップ
    void IncrementPlayCount( TbUint32 stage ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) ){
            ++m_StageData[stage].playCount;
        }
    }

    // プレイ回数取得
    TbUint32 GetPlayCount( TbUint32 stage ) const {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) ){
            return m_StageData[stage].playCount;
        }
        return 0;
    }

    // クリア回数設定
    void SetClearCount( TbUint32 stage , TbUint32 clearCount ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) ){
            m_StageData[stage].clearCount = clearCount;
        }
    }
  
    // クリア回数カウントアップ
    void IncrementClearCount( TbUint32 stage ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) ){
            ++m_StageData[stage].clearCount;
        }
    }

    // クリア回数取得
    TbUint32 GetClearCount( TbUint32 stage ) const {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) ){
            return m_StageData[stage].clearCount;
        }
        return 0;
    }

    // ベストタイム設定
    TbBool SetBestMillSec( TbUint32 stage , TbUint32 playerCount , TbUint32 ms ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && 
            0 < playerCount && playerCount <= CMN_PLAYER_MAX &&
            ms < m_StageData[stage].bestTimeMs[playerCount-1] )
        {
            // 更新
            m_StageData[stage].bestTimeMs[playerCount-1] = ms;
            return TB_TRUE;
        }
        return TB_FALSE;
    }
  
    // ベストタイム取得
    TbUint32 GetBestMillSec( TbUint32 stage , TbUint32 playerCount ) const {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && 
            0 < playerCount && playerCount <= CMN_PLAYER_MAX )
        {
            return m_StageData[stage].bestTimeMs[playerCount-1];
        }
        return StageData::TIME_INVALID;
    }

public: // ワールド

    // クリア回数設定
    void SetWorldStageClearCount(TbUint32 world, TbUint32 stage , TbUint32 count) {
        m_WorldData[world].clearCount[stage] = count;
    }

    // ワールドのステージクリアフラグ設定
    void SetWorldStageClearFlag(TbUint32 world, TbUint32 stage) {
        m_WorldData[world].clearFlag[stage] = TB_TRUE;
    }

    // ワールドのステージクリアフラグをクリア
    void ClearWorldStageClearFlag(TbUint32 world) {
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_WorldData[world].clearFlag); ++i)
        {
            if (m_WorldData[world].clearFlag[i]) {
                ++m_WorldData[world].clearCount[i];
                m_WorldData[world].clearFlag[i] = TB_FALSE;
            }
        }
    }

    // ワールドの一番進んでいるステージ取得
    TbUint32 GetLatestStage(TbUint32 world) {
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_WorldData[world].clearFlag); ++i)
        {
            if ( 0 == m_WorldData[world].clearCount[i]) {
                return i;
            }
        }
        return STAGE_MAX;
    }
    
    // ワールドの新しいステージをクリアしたか
    TbBool IsWorldNewStageCleared(TbUint32 world) {
        TbUint32 latestStage = GetLatestStage(world);
        if (latestStage == STAGE_MAX) {
            return TB_FALSE;
        }
        return m_WorldData[world].clearFlag[latestStage];
    }

    // ワールド情報リセット
    void ResetWorldData() {
        WorldData data;
        TbFill(m_WorldData, data);
    }

private:
    StageData m_StageData[STAGE_MAX];
    WorldData m_WorldData[WORLD_MAX];
};

#endif
