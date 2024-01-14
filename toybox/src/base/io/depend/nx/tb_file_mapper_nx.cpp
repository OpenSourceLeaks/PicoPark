/*!
 * ファイル系ユーティリティ関数郡
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */

#include "tb_fwd.h"
#include "base/io/depend/nx/tb_file_mapper_nx.h"
#include "base/io/depend/nx/tb_file_nx.h"
#include "base/io/tb_file_util.h"

#include <nn/fs/fs_RomForDebug.h>
#include <nn/fs/fs_Debug.h>
#include <nn/account.h>
#include <nn/util/util_ScopeExit.h>

namespace toybox
{

namespace
{

enum FileSystemMask
{
    FILE_SYSTEM_MASK_FILE = TB_BIT(0) , 
    FILE_SYSTEM_MASK_DIR  = TB_BIT(1) , 
};

void getFileSystems( TbFileMapper::PathList& result , const TbStaticString512& dirPath , TbFilePathId pathId , TbUint32 searchFlag , TbUint32 mask , const TbStaticString512& baseDirPath )
{
    result.SetEnableSetCapacityAuto(TB_TRUE);
    const TbStaticString512& fullPath = inner::TbFileUtilPrivate::GetFullPath( dirPath.GetBuffer() , pathId ) + TB_FILE_SEPARATOR + "*";
    if (0 == mask) {
        return;
    }

    int mode = nn::fs::OpenDirectoryMode_Directory;
    if (mask & FILE_SYSTEM_MASK_FILE){
        if (searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR) {
            mode = nn::fs::OpenDirectoryMode_All;
        }else {
            mode = nn::fs::OpenDirectoryMode_File;
        }
    }
    nn::fs::DirectoryHandle dirHandle;
    nn::Result ret = nn::fs::OpenDirectory(&dirHandle, fullPath.GetBuffer(), mode);
    if (!ret.IsSuccess()) {
        return;
    }

    int64_t count = 0;
    ret = nn::fs::GetDirectoryEntryCount(&count, dirHandle);
    TB_ASSERT(ret.IsSuccess());

    if (0 < count)
    {
        nn::fs::DirectoryEntry* dirEntry = new nn::fs::DirectoryEntry[count];
        ret = nn::fs::ReadDirectory(&count, dirEntry, dirHandle, count);
        TB_ASSERT(ret.IsSuccess());

        for (int i = 0; i < count; i++)
        {
            TbStaticString512 dirPathNew;
            if (dirEntry[i].directoryEntryType == nn::fs::DirectoryEntryType_Directory)
            {
                dirPathNew = dirPath;
                dirPathNew += TB_FILE_SEPARATOR;
                dirPathNew += dirEntry[i].name;
            }
            if (searchFlag & TB_FILE_SEARCH_FLAG_NAME) {
                result.PushBack(TbStaticString512(dirEntry[i].name));
            }
            else if (searchFlag & TB_FILE_SEARCH_FLAG_RELATIVE) {
                TbStaticPath512 pathDst;
                if (dirEntry[i].directoryEntryType == nn::fs::DirectoryEntryType_Directory) {
                    pathDst.Set(&dirPathNew[baseDirPath.GetCharCount()], "", TB_FILE_PATH_ID_NONE);
                }
                else {
                    pathDst.Set(&dirPath[baseDirPath.GetCharCount() + 1], dirEntry[i].name, TB_FILE_PATH_ID_NONE);
                }
                result.PushBack(pathDst);
            }
            else {
                TbStaticPath512 pathDst;
                if (dirEntry[i].directoryEntryType == nn::fs::DirectoryEntryType_Directory) {
                    pathDst.Set(dirPathNew, "", pathId);
                }
                else {
                    pathDst.Set(dirPath, dirEntry[i].name, pathId);
                }
                result.PushBack(pathDst);
            }
            if ((dirEntry[i].directoryEntryType == nn::fs::DirectoryEntryType_Directory) && searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR) {
                // サブディレクトリ検索
                getFileSystems(result, dirPathNew, pathId, searchFlag, mask, baseDirPath);
            }
        }
        TB_SAFE_DELETE_ARRAY(dirEntry);
    }
    nn::fs::CloseDirectory(dirHandle);
}

}

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
#if TB_PLATFORM_IS_NXWIN
    result = nn::fs::MountSaveDataForDebug("save");
#else
    // アカウントライブラリを初期化します。
    nn::account::Initialize();

    // アプリケーション起動時に選択されたユーザーをオープンします。
    // ユーザーが操作していると言える期間はオープンしたままにします。
    NN_ABORT_UNLESS_RESULT_SUCCESS(nn::account::OpenPreselectedUser(&m_UserHandle));

    nn::account::Uid uid;
    NN_ABORT_UNLESS_RESULT_SUCCESS(nn::account::GetUserId(&uid, m_UserHandle));
    result = nn::fs::MountSaveData("save",uid);
#endif
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
    nn::account::CloseUser(m_UserHandle);
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