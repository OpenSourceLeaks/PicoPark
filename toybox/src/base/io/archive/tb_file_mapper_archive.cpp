/*!
 * ファイル系ユーティリティ関数郡
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */

#include "tb_fwd.h"
#include "base/io/archive/tb_file_mapper_archive.h"
#include "base/io/archive/tb_file_archive.h"
#include "base/io/archive/tb_archive.h"
#include <base/io/tb_file_util_private.h>

namespace toybox
{

/*!
 * ファイルサイズ取得
 * @author Miyake_Shunsuke
 * @since 2014.07.17
 */
TbUint32 TbFileMapperArchive::GetFileSize( const char* path , TbFilePathId pathId )
{
    if( m_MappingPath.GetRootPathId() != pathId ) {
        return TB_FALSE;
    }
    const char* localPath = getLocalPath(path);
    const TbArchiveFileInfo* info = m_Archive->GetFileInfoFromPath(localPath);
    TbUint32 result = 0;
    if( info ){
        result = info->size;
    }
    return result;
}

/*!
 * ファイル存在するか
 * @author Miyake_Shunsuke
 * @since 2014.07.17
 */
TbBool TbFileMapperArchive::ExistsFile( const char* path , TbFilePathId pathId )
{
    if( m_MappingPath.GetRootPathId() != pathId ) {
        return TB_FALSE;
    }
    const char* localPath = getLocalPath(path);
    const TbArchiveFileInfo* info = m_Archive->GetFileInfoFromPath(localPath);
    return info != NULL;
}

/*!
 * ディレクトリは存在するか
 * @author Miyake_Shunsuke
 * @since 2014.07.17
 */
TbBool TbFileMapperArchive::ExistsDirectory( const char* path , TbFilePathId pathId )
{
    if( m_MappingPath.GetRootPathId() != pathId ) {
        return TB_FALSE;
    }
    const char* localPath = getLocalPath(path);
    const TbArchiveDirectoryInfo* info = m_Archive->GetDirectoryInfoFromPath(localPath);
    return info != NULL;
}

/*!
 * ファイル実体生成
 * @author Miyake_Shunsuke
 * @since 2014.07.17
 */
TbFileImpl* TbFileMapperArchive::CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    const char* localPath = getLocalPath(path);
    const TbArchiveFileInfo* info = m_Archive->GetFileInfoFromPath(localPath);
    TbFileImpl* result = TB_NEW TbFileArchive(localPath,operateMode,syncMode,m_Archive);
    return result;
}

/*!
 * ファイル一覧取得
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
void TbFileMapperArchive::GetFiles( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TbStaticString512 basePath = path;
    basePath.Replace('\\','/');
    result.Clear();
    if( m_MappingPath.GetRootPathId() != pathId ) {
        return;
    }
    const char* localPath = getLocalPath(basePath.GetBuffer());
    const TbArchiveDirectoryInfo* dirInfo = m_Archive->GetDirectoryInfoFromPath(localPath);
    if( dirInfo ){
        if( searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ){
            result.SetCapacity(dirInfo->fileAllCount);
        }else{
            result.SetCapacity(dirInfo->fileCount);
        }
        TbStaticString512 basePath = path;
        basePath.Replace('\\','/');
        addFile(result,dirInfo,searchFlag,basePath);
    }
}

/*! 
 * ディレクトリ一覧取得
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
void TbFileMapperArchive::GetDirectories( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TbStaticString512 basePath = path;
    basePath.Replace('\\','/');
    result.Clear();
    if( m_MappingPath.GetRootPathId() != pathId ) {
        return;
    }
    const char* localPath = getLocalPath(basePath.GetBuffer());
    const TbArchiveDirectoryInfo* dirInfo = m_Archive->GetDirectoryInfoFromPath(localPath);
    if( dirInfo ){
        if( searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ){
            result.SetCapacity(dirInfo->subdirAllCount);
        }else{
            result.SetCapacity(dirInfo->subdirCount);
        }
        TbStaticString512 basePath = path;
        basePath.Replace('\\','/');
        addSubdir( result , dirInfo , searchFlag , basePath );
    }
}

/*!
 * ファイル、ディレクトリ一覧取得
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
void TbFileMapperArchive::GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TbStaticString512 basePath = path;
    basePath.Replace('\\','/');
    result.Clear();
    if( m_MappingPath.GetRootPathId() != pathId ) {
        return;
    }
    const char* localPath = getLocalPath(basePath.GetBuffer());
    const TbArchiveDirectoryInfo* dirInfo = m_Archive->GetDirectoryInfoFromPath(localPath);
    if( dirInfo ){
        if( searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ){
            result.SetCapacity(dirInfo->subdirAllCount+dirInfo->fileAllCount);
        }else{
            result.SetCapacity(dirInfo->subdirCount+dirInfo->fileAllCount);
        }
        addSubdir( result , dirInfo , searchFlag , basePath );
        addFile( result , dirInfo , searchFlag , basePath );
    }
}

/*!
 * Archiveに渡すパスを取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const char* TbFileMapperArchive::getLocalPath( const char* path )
{
    TbUint32 index = 0;
    if( !m_MappingPath.GetLocalPath().IsEmpty() ){
        if( 0!=TbStrCmp(m_MappingPath.GetBuffer(),
                        path,
                        m_MappingPath.GetLocalPath().GetCharCount()) )
        {
            return TB_FALSE;
        }
        index = m_MappingPath.GetLocalPath().GetCharCount();
    }
    const char* result = &path[index];
    if( result[0] == '\0' ) {
        result = ".";
    }
    return result;
}

/*!
 * サブディレクトリを追加
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
void TbFileMapperArchive::addSubdir( PathList& result , const TbArchiveDirectoryInfo* info , TbUint32 searchFlag , const TbStaticString512& basePath )
{
    for( TbUint32 i = 0; i < info->subdirCount; ++i )
    {
        const TbArchiveDirectoryInfo* dir = m_Archive->GetSubDirectoryInfoFromDir( info , i );
        TB_ASSERT(dir);
        if( searchFlag & TB_FILE_SEARCH_FLAG_NAME ){
            result.PushBack( TbStaticPath512(&dir->path[dir->nameIndex],m_MappingPath.GetRootPathId()) );
        }
        if( searchFlag & TB_FILE_SEARCH_FLAG_NAME ){
            result.PushBack( TbStaticPath512(&dir->path[dir->nameIndex],TB_FILE_PATH_ID_NONE) );
        }else{
            TbStaticString512 path = m_MappingPath.GetBuffer();
            if( !path.IsEmpty() ){
                path.Append("/");
            }
            path.Append(dir->path);
            if( searchFlag & TB_FILE_SEARCH_FLAG_RELATIVE ){
                result.PushBack( TbStaticPath512(&path[basePath.GetCharCount()+1],TB_FILE_PATH_ID_NONE) );
            }else{
                result.PushBack( TbStaticPath512(path,m_MappingPath.GetRootPathId()) );
            }
        }
        if ( searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ){
            addSubdir( result , dir , searchFlag , basePath );
        }
    }
}

/*!
 * ファイルを追加
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
void TbFileMapperArchive::addFile( PathList& result , const TbArchiveDirectoryInfo* info , TbUint32 searchFlag , const TbStaticString512& basePath )
{
    for( TbUint32 i = 0; i < info->fileCount; ++i )
    {
        const TbArchiveFileInfo* fileInfo = m_Archive->GetFileInfoFromDir( info , i );
        TB_ASSERT(fileInfo);
        if( searchFlag & TB_FILE_SEARCH_FLAG_NAME ){
            result.PushBack( TbStaticPath512(&fileInfo->path[fileInfo->nameIndex],TB_FILE_PATH_ID_NONE) );
        }else{
            TbStaticString512 path = m_MappingPath.GetBuffer();
            if( !path.IsEmpty() ){
                path.Append("/");
            }
            path.Append(fileInfo->path);
            if( searchFlag & TB_FILE_SEARCH_FLAG_RELATIVE ){
                result.PushBack( TbStaticPath512(&path[basePath.GetCharCount()+1],TB_FILE_PATH_ID_NONE) );
            }else{
                result.PushBack( TbStaticPath512(path,m_MappingPath.GetRootPathId()) );
            }
        }
    }
    if ( searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ){
        for( TbUint32 i = 0; i < info->subdirCount; ++i )
        {
            const TbArchiveDirectoryInfo* dir = m_Archive->GetSubDirectoryInfoFromDir( info , i );
            TB_ASSERT(dir);
            addFile( result , dir , searchFlag , basePath );
        }
    }
}

}
