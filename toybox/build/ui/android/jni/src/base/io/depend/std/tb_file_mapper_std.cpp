/*!
 * ファイルマッピング
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/io/depend/std/tb_file_mapper_std.h"
#include "base/io/depend/std/tb_file_std.h"
#include <base/io/tb_file_util_private.h>

namespace toybox
{

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileMapperStd::GetFileSize( const char* path , TbFilePathId pathId )
{
    return TbFileStd::GetFileSize( path , pathId );
}

/*!
 * ファイル存在するか
 * @author teco
 */
TbBool TbFileMapperStd::ExistsFile( const char* path , TbFilePathId pathId )
{
    return TbFileStd::ExistsFile( path , pathId );
}

/*!
 * ディレクトリは存在するか
 * @author teco
 */
TbBool TbFileMapperStd::ExistsDirectory( const char* path , TbFilePathId pathId )
{
    return true;
}

/*!
 * ファイル実体生成
 * @author teco
 */
TbFileImpl* TbFileMapperStd::CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    // 標準関数郡でテストしたい場合
    TbFileImpl* result = TB_NEW TbFileStd(path,operateMode,syncMode,pathId);
    return result;
}

/*!
 * ディレクトリ作成
 * @author teco
 */
TbResult TbFileMapperStd::CreateDirectory( const char* path , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * ディレクトリ削除
 * @author teco
 */
TbResult TbFileMapperStd::DeleteDirectory( const char* path , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * ファイル一覧取得
 * @author teco
 */
void TbFileMapperStd::GetFiles( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
}

/*! 
 * ディレクトリ一覧取得
 * @author teco
 */
void TbFileMapperStd::GetDirectories( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
}

/*!
 * ファイル、ディレクトリ一覧取得
 * @author teco
 */
void TbFileMapperStd::GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
}

}