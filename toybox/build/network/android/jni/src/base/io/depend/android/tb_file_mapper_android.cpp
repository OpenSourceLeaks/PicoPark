/*!
 * AndroidのAssetファイルマッピング
 * @author teco
 */

#include "tb_fwd.h"
#include "base/io/depend/android/tb_file_mapper_android.h"
#include "base/io/depend/android/tb_file_android.h"
#include <base/io/tb_file_util_private.h>

namespace toybox
{

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileMapperAndroid::GetFileSize( const char* path , TbFilePathId pathId )
{
    return TbFileAndroid::GetFileSize( path , pathId );
}

/*!
 * ファイル存在するか
 * @author teco
 */
TbBool TbFileMapperAndroid::ExistsFile( const char* path , TbFilePathId pathId )
{
    return TbFileAndroid::ExistsFile( path , pathId );
}

/*!
 * ディレクトリは存在するか
 * @author teco
 */
TbBool TbFileMapperAndroid::ExistsDirectory( const char* path , TbFilePathId pathId )
{
    return true;
}

/*!
 * ファイル実体生成
 * @author teco
 */
TbFileImpl* TbFileMapperAndroid::CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    // 標準関数郡でテストしたい場合
    TbFileImpl* result = TB_NEW TbFileAndroid(path,operateMode,syncMode,pathId);
    return result;
}

/*!
 * ディレクトリ作成
 * @author teco
 */
TbResult TbFileMapperAndroid::CreateDirectory( const char* path , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * ディレクトリ削除
 * @author teco
 */
TbResult TbFileMapperAndroid::DeleteDirectory( const char* path , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * ファイル一覧取得
 * @author teco
 */
void TbFileMapperAndroid::GetFiles( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
}

/*! 
 * ディレクトリ一覧取得
 * @author teco
 */
void TbFileMapperAndroid::GetDirectories( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
}

/*!
 * ファイル、ディレクトリ一覧取得
 * @author teco
 */
void TbFileMapperAndroid::GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TB_ASSERT(!"unsupported");
}

}