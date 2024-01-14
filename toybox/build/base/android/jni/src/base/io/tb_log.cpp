/*!
 * ログ
 */

#include "tb_fwd.h"
#include "base/io/tb_log.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbLog::TbLog()
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbLog::~TbLog()
{
    Finish();
}

/*!
 * 開始
 * @author teco
 */
TbResult TbLog::Start( const char* fname , TbFilePathId pathId )
{
#if TB_IO_IS_ENABLE_WRITE_FILE
    TB_RESQUE_RET(!m_File.IsOpened(),TB_E_FAIL);
    return m_File.Open(fname,TB_FILE_OPERATE_WRITE,TB_FILE_SYNC,pathId);
#else
    return TB_E_FAIL;
#endif
}

/*!
 * 終了
 * @author teco
 */
void TbLog::Finish()
{
    if( m_File.IsOpened() ){
        m_File.Close();
    }
}

/*!
 * ログ出力
 * @author teco
 */
void TbLog::Print( const char* buffer )
{
    if( m_File.IsOpened() ){
        TbSint32 size = TbStrCharCount(buffer);
        m_File.Write(buffer,size);
    }
}

/*!
 * ログ出力
 * @author teco
 */
void TbPrintfLog(const TbChar8 *fmt, ...)
{
    static const TbSizeT BUF_SIZE = 1024;
    va_list ap;
    TbChar8    buf[BUF_SIZE];
    va_start( ap, fmt );
    TbVStrPrintf( buf, BUF_SIZE , fmt, ap );
    va_end( ap );

    if( TbLog::GetInstance() ){
        TbLog::GetInstance()->Print(buf);
    }
}

/*!
 * ログ衆力
 * @author teco
 */
void TbPrintLog( const TbChar8* string )
{
    if( TbLog::GetInstance() ){
        TbLog::GetInstance()->Print(string);
    }
}

}
