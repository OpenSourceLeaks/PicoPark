/*!
 * @file
 * @brief コンソールの実装ファイル(printfバージョン)
 * @author teco
 */

#include "tb_fwd.h"
#include "base/system/tb_console.h"

namespace toybox
{

/*!
 * コンソール生成
 * @author teco
 */
TbResult TbConsole::Create()
{
    return TB_S_OK;
}

/*!
 * コンソール生成されている
 * @author teco
 */
TbBool TbConsole::IsCreated()
{
    return TB_TRUE;
}

/*!
 * コンソール削除
 * @author teco
 */
TbResult TbConsole::Delete()
{
    return TB_S_OK;
}

/*!
 * 出力
 * @author teco
 */
void TbConsole::Printf( const TbChar8 *fmt , ... )
{
    va_list ap;
    TbChar8    buf[1024];
    va_start( ap, fmt );
    TbVStrPrintf( buf, 1024 , fmt, ap );
    va_end( ap );
    TbPrintf("%s",buf);
}

}
