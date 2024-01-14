/*!
 * @file
 * @brief アサート関連実装ファイル
 * @author teco
 */

#include "tb_fwd.h"
#include "base/io/tb_print.h"
#include "base/system/tb_debug.h"
#include "base/system/tb_assert.h"
#include "base/string/tb_string_util.h"

namespace toybox
{

/*!
 * アサート(可変出力文字列あり)
 * @return 0を返す(3項演算子用)
 */
TbUint32 TbPrintAssertMessage( const char* file , TbSint32 line , const char* func , const char* fmt , ... )
{
    va_list ap;
    TbChar8    buf[512];
    va_start( ap, fmt );
    TbVStrPrintf( buf , 512 , fmt , ap );
    va_end( ap );
    TbPrintf("[ASSERT]--\nfile:%s\nline:%d\nfunc:%s\nmessage:%s\n",file,line,func,buf);
    TbBreak(); 
    return 0;
}

/*!
 * 警告(可変出力文字列あり)
 * @return 0を返す(3項演算子用)
 */
TbUint32 TbPrintWarnMessage( const char* file , TbSint32 line , const char* func , const char* fmt , ... )
{
    va_list ap;
    TbChar8    buf[512];
    va_start( ap, fmt );
    TbVStrPrintf( buf , 512 , fmt , ap );
    va_end( ap );
    TbPrintf("[WARN]--\nfile:%s\nline:%d\nfunc:%s\nmessage:%s\n",file,line,func,buf);
    return 0;
}

}