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
    StageData()
    {
    }
    CheckPoint  checkPoint[CMN_PLAYER_MAX];
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
    void SetCheckPoint( TbUint32 stage , const TbVector2& pos , TbFloat32 sec ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && 0 < TB_ARRAY_COUNT_OF(m_StageData[stage].checkPoint) ){
            m_StageData[stage].checkPoint[0].pos = pos;
            m_StageData[stage].checkPoint[0].sec = sec;
            m_StageData[stage].checkPoint[0].isActive = TB_TRUE;
        }
    }
  
    // チェックポイントリセット
    void ResetCheckPoint( TbUint32 stage ) {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && 0 < TB_ARRAY_COUNT_OF(m_StageData[stage].checkPoint) ){
            m_StageData[stage].checkPoint[0].isActive = TB_FALSE;
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
    TbBool GetCheckPoint( TbVector2& dst , TbFloat32& dstSec , TbUint32 stage ) const {
        if( stage < TB_ARRAY_COUNT_OF(m_StageData) && 0 < TB_ARRAY_COUNT_OF(m_StageData[stage].checkPoint) ){
            if( m_StageData[stage].checkPoint[0].isActive ) {
                dst = m_StageData[stage].checkPoint[0].pos;
                dstSec = m_StageData[stage].checkPoint[0].sec;
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

private:
    StageData m_StageData[STAGE_MAX];
};

#endif
