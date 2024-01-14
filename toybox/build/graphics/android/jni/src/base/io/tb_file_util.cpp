/*!
 * ファイルユーティリティ関数郡
 * @author Miyake_Shunsuke
 * @since 2013.11.12
 */

#include "tb_fwd.h"
#include "base/io/tb_file_util.h"
#include "base/io/tb_file_util_private.h"
#include "base/io/tb_file_manager.h"

namespace toybox
{

namespace inner
{

// パス取得
const TbStaticString512 TbFileUtilPrivate::GetFullPath( const char* path , TbFilePathId pathId ) {
    TbStaticString512 result = "";
    const TbStaticString512& rootPath = TbFileManager::GetInstance().GetRootPath( pathId );
    if( !rootPath.IsEmpty() ){
        result += rootPath.GetBuffer();
        result += TB_FILE_SEPARATOR;
    }
    result += path;
    return result;
}

}

}