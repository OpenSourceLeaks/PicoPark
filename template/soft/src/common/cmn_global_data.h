/*!
 * グローバルデータ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CMN_GLOBAL_DATA_H_
#define _INCLUDED_CMN_GLOBAL_DATA_H_

struct CmnSongResult
{
    CmnSongResult()
        : successCount(0) 
        , targetCount(0)
    {}
    TbUint32 successCount;
    TbUint32 targetCount;
};

// グローバルデータ
class CmnGlobalData
{    
public:
    
    // コンストラクタ
    CmnGlobalData() {}
    
    // デストラクタ
    ~CmnGlobalData() {}

public:

    
    // 曲リザルト取得
    const CmnSongResult& GetSongResult() const {
        return m_Result;
    }

    // 曲リザルト設定
    void SetSongResult( const CmnSongResult& result ){
        m_Result = result;
    } 

private:
    CmnSongResult   m_Result;
};

#endif
