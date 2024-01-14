/*!
 * ファイル系ユーティリティ関数郡
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */

#include "tb_fwd.h"
#include "base/io/depend/nx/tb_file_mapper_save_nx.h"
#include "base/io/depend/nx/tb_file_save_nx.h"
#include "base/io/tb_file_util.h"

#include <nn/fs/fs_RomForDebug.h>

namespace toybox
{

/*!
* コンストラクタ
* @author teco
*/
TbFileMapperNX::TbFileMapperNX() 
    : TbFileMapper(TB_TRUE, TB_FALSE)
    , m_MountRomCache(nullptr)
{
    size_t cacheSize= 0;
    // ファイルシステムのメタデータキャッシュに必要なバッファサイズを取得します。
    (void)nn::fs::QueryMountRomCacheSize(&cacheSize);

    // キャッシュバッファを確保します。
    m_MountRomCache = new TbUint8[cacheSize];
    if (!m_MountRomCache){
        TB_ASSERT(0);
        return;
    }

    TB_ASSERT(nn::fs::CanMountRomForDebug());

    // ファイルシステムをマウントします。
    // キャッシュバッファはアンマウントするまで解放しないでください。
    nn::Result result = nn::fs::MountRom("rom", m_MountRomCache, cacheSize);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    // セーブデータのmountしておく
    result = nn::fs::MountSaveData("save");
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
}

/*!
* デストラクタ
* @author teco
*/
TbFileMapperNX::~TbFileMapperNX()
{
    nn::fs::Unmount("rom");
    nn::fs::Unmount("save");
    TB_SAFE_DELETE_ARRAY(m_MountRomCache);
}

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileMapperNX::GetFileSize( const char* path , TbFilePathId pathId )
{
    return TbFileNX::GetFileSize( path , pathId );
}

/*!
 * ファイル存在するか
 * @author teco
 */
TbBool TbFileMapperNX::ExistsFile( const char* path , TbFilePathId pathId )
{
    return TbFileNX::ExistsFile( path , pathId );
}

/*!
 * ディレクトリは存在するか
 * @author teco
 */
TbBool TbFileMapperNX::ExistsDirectory( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullpath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );

    nn::fs::DirectoryHandle dirHandle;
    nn::Result result = nn::fs::OpenDirectory(&dirHandle, fullpath.GetBuffer() , nn::fs::OpenDirectoryMode_All);
    if (result.IsSuccess()) {
        nn::fs::CloseDirectory(dirHandle);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ファイル実体生成
 * @author teco
 */
TbFileImpl* TbFileMapperNX::CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    TbFileImpl* result = TB_NEW TbFileNX(path,operateMode,syncMode,pathId);
    return result;
}

/*!
 * ディレクトリ作成
 * @author teco
 */
TbResult TbFileMapperNX::CreateDirectory( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullpath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    nn::Result result = nn::fs::CreateDirectory( fullpath.GetBuffer() );
    return result.IsSuccess() ? TB_S_OK : TB_E_FAIL;
}

/*!
 * ディレクトリ削除
 * @author teco
 */
TbResult TbFileMapperNX::DeleteDirectory( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullpath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    nn::Result result = nn::fs::DeleteDirectory(fullpath.GetBuffer());
    return result.IsSuccess() ? TB_S_OK : TB_E_FAIL;
}

/*!
 * ファイル一覧取得
 * @author teco
 */
void TbFileMapperNX::GetFiles( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    getFileSystems(result,path,pathId,searchFlag,FILE_SYSTEM_MASK_FILE,path);
}

/*! 
 * ディレクトリ一覧取得
 * @author teco
 */
void TbFileMapperNX::GetDirectories( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    getFileSystems(result,path,pathId,searchFlag,FILE_SYSTEM_MASK_DIR,path);
}

/*!
 * ファイル、ディレクトリ一覧取得
 * @author teco
 */
void TbFileMapperNX::GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    getFileSystems(result,path,pathId,searchFlag,FILE_SYSTEM_MASK_DIR|FILE_SYSTEM_MASK_FILE,path);
}

}