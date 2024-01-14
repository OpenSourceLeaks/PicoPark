/*!
 * @brief ログ
 */
 
#ifndef _INCLUDED_TB_LOG_H_
#define _INCLUDED_TB_LOG_H_

#include <base/util/tb_singleton.h>
#include <base/io/tb_file.h>
 
namespace toybox
{

class TbLog : public TbSingletonPointer<TbLog>
{
public:
    
    // コンストラクタ
    TbLog();

    // デストラクタ
    ~TbLog();

public:

    // 開始
    TbResult Start( const char* fname , TbFilePathId pathId );

    // 終了
    void Finish();

public:

    // 出力
    void Print( const char* buffer );

private:

    TbFile  m_File;
    
};

// ログ衆力
void TbPrintfLog( const TbChar8* fmt, ... );

// ログ衆力
void TbPrintLog( const TbChar8* string );

}

#endif
