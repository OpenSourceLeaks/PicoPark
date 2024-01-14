/*!
 * ファイル系ユーティリティ関数郡
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */

#include "tb_fwd.h"
#include "base/io/depend/wiiu/tb_file_mapper_wiiu.h"
#include "base/io/depend/wiiu/tb_file_wiiu.h"
#include <base/io/tb_file_util_private.h>

namespace toybox
{

enum FileSystemMask
{
    FILE_SYSTEM_MASK_FILE = TB_BIT(0) , 
    FILE_SYSTEM_MASK_DIR  = TB_BIT(1) , 
};

void TbFileMapperWiiU::getFileSystems( TbFileMapper::PathList& result , 
                                       const TbStaticString512& dirPath , 
                                       TbFilePathId pathId , 
                                       TbUint32 searchFlag , 
                                       TbUint32 mask , 
                                       const TbStaticString512& baseDirPath )
{
    result.SetEnableSetCapacityAuto(TB_TRUE);
    const TbStaticString512& fullPath = inner::TbFileUtilPrivate::GetFullPath( dirPath.GetBuffer() , pathId ) + TB_FILE_SEPARATOR;
    
    FSDirHandle dh;
    FSDirEntry  de;
    FSOpenDir(&m_Client, &m_CmdBlock , fullPath.GetBuffer(), &dh, FS_RET_NO_ERROR);
    
    while (FSReadDir(&m_Client, &m_CmdBlock , dh, &de, FS_RET_NO_ERROR) == FS_STATUS_OK)
    {
        TbBool isActive = TB_TRUE;
        if( de.stat.flag & FS_STAT_FLAG_IS_DIRECTORY ) {
            isActive = mask & FILE_SYSTEM_MASK_DIR;
        }else{
            isActive = mask & FILE_SYSTEM_MASK_FILE;
        }
        TbStaticString512 dirPathNew;
        if( ( de.stat.flag & FS_STAT_FLAG_IS_DIRECTORY ) && 
            ( isActive || searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ) )
        {
            dirPathNew = dirPath;
            dirPathNew += TB_FILE_SEPARATOR;
            dirPathNew += de.name;
        }
        if( isActive ) {
            if( searchFlag & TB_FILE_SEARCH_FLAG_NAME ) {
                result.PushBack(TbStaticString512(de.name));
            }else if( searchFlag & TB_FILE_SEARCH_FLAG_RELATIVE ) {
                TbStaticPath512 pathDst;
                if( de.stat.flag & FS_STAT_FLAG_IS_DIRECTORY ) {
                    pathDst.Set( &dirPathNew[baseDirPath.GetCharCount()] , "" , TB_FILE_PATH_ID_NONE );
                }else{
                    pathDst.Set( &dirPath[baseDirPath.GetCharCount()+1] , de.name , TB_FILE_PATH_ID_NONE);
                }
                result.PushBack(pathDst);
            }else{
                TbStaticPath512 pathDst;
                if( de.stat.flag & FS_STAT_FLAG_IS_DIRECTORY ) {
                    pathDst.Set( dirPathNew , "" , pathId );
                }else{
                    pathDst.Set( dirPath , de.name , pathId);
                }
                result.PushBack(pathDst);
            }
        }
        if( (de.stat.flag & FS_STAT_FLAG_IS_DIRECTORY) && searchFlag & TB_FILE_SEARCH_FLAG_SUBDIR ){
            // サブディレクトリ検索
            getFileSystems( result , dirPathNew , pathId , searchFlag , mask , baseDirPath );
        }
    }

    FSCloseDir(&m_Client, &m_CmdBlock , dh , FS_RET_NO_ERROR);
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 */
TbFileMapperWiiU::TbFileMapperWiiU()
    : TbFileMapper(TB_TRUE,TB_FALSE)
{
    FSInit();
    FSAddClient(&m_Client, FS_RET_NO_ERROR);
    FSInitCmdBlock(&m_CmdBlock);
    
    // Set state change notification callback.
    FSStateChangeParams stateChangeParams = {
        .userCallback = TbFileMapperWiiU::stateChangeCallbackStatic,
        .userContext  = this,
        .ioMsgQueue   = NULL
    };
    FSSetStateChangeNotification(&m_Client, &stateChangeParams);
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 */
TbFileMapperWiiU::~TbFileMapperWiiU()
{
    FSDelClient(&m_Client, FS_RET_NO_ERROR);
    FSShutdown();
}

/*!
 * 状態変化コールバック
 * @author Miyake_Shunsuke
 */
void TbFileMapperWiiU::stateChangeCallbackStatic(FSClient *client, FSVolumeState state, void* context)
{
    TbFileMapperWiiU* self = reinterpret_cast<TbFileMapperWiiU*>(context);
    self->stateChangeCallback(client,state);
}
    
/*!
 * 状態変化コールバック
 * @author Miyake_Shunsuke
 */
void TbFileMapperWiiU::stateChangeCallback(FSClient *client, FSVolumeState state )
{
    if (state != FS_VOLSTATE_READY)
    {
        // call error viewer with last error
        s32 lastErrorCode = FSGetLastErrorCodeForViewer(client);
        if (lastErrorCode > 0)
        {
//            ErrorViewerSetErrorCode(lastErrorCode);
//            ErrorViewerAppear();
        }
        else if (lastErrorCode < 0)
        {
            TbPrintf("Failed to get last error code for error viewer\n");
        }
    }

    // Report volume state and last error.
    FSError lastError = FSGetLastError(client);
    TbPrintf("Volume state of client 0x%08x changed to %d\n", client, state);
    TbPrintf("Last error: %d\n", lastError);
}

/*!
 * ファイルサイズ取得
 * @author Miyake_Shunsuke
 */
TbUint32 TbFileMapperWiiU::GetFileSize( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullPath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    
    FSStat stat;
    if( FS_STATUS_OK == FSGetStat(&m_Client,&m_CmdBlock,fullPath.GetBuffer(),&stat,FS_RET_NO_ERROR) )
    {
        if( !(stat.flag & FS_STAT_FLAG_IS_DIRECTORY) ){
            return stat.size;
        }
    }
    return 0;
}

/*!
 * ファイル存在するか
 * @author Miyake_Shunsuke
 */
TbBool TbFileMapperWiiU::ExistsFile( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullPath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    
    FSStat stat;
    if( FSGetStat(&m_Client,&m_CmdBlock,fullPath.GetBuffer(),&stat,FS_RET_NO_ERROR) == FS_STATUS_OK )
    {
        return (stat.flag & FS_STAT_FLAG_IS_DIRECTORY) ? TB_TRUE : TB_FALSE;
    }
    return TB_FALSE;
}

/*!
 * ディレクトリは存在するか
 * @author Miyake_Shunsuke
 */
TbBool TbFileMapperWiiU::ExistsDirectory( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullPath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    
    FSStat stat;
    if( FSGetStat(&m_Client,&m_CmdBlock,fullPath.GetBuffer(),&stat,FS_RET_NO_ERROR) == FS_STATUS_OK )
    {
        return (stat.flag & FS_STAT_FLAG_IS_DIRECTORY) ? TB_TRUE : TB_FALSE;
    }
    return TB_FALSE;
}

/*!
 * ファイル実体生成
 * @author Miyake_Shunsuke
 */
TbFileImpl* TbFileMapperWiiU::CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    TbFileImpl* result = TB_NEW TbFileWiiU(path,operateMode,syncMode,pathId);
    return result;
}

/*!
 * ディレクトリ作成
 * @author Miyake_Shunsuke
 */
TbResult TbFileMapperWiiU::CreateDirectory( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullPath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    FSStatus result = FSMakeDir(&m_Client,&m_CmdBlock,fullPath.GetBuffer(),FS_RET_NO_ERROR);
    return result == FS_STATUS_OK;
}

/*!
 * ディレクトリ削除
 * @author Miyake_Shunsuke
 * @since 2014.07.21
 */
TbResult TbFileMapperWiiU::DeleteDirectory( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullPath( inner::TbFileUtilPrivate::GetFullPath( path , pathId ) );
    FSStatus result = FSRemove(&m_Client,&m_CmdBlock,fullPath.GetBuffer(),FS_RET_NO_ERROR);
    return result == FS_STATUS_OK;
}

/*!
 * ファイル一覧取得
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
void TbFileMapperWiiU::GetFiles( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    getFileSystems(result,path,pathId,searchFlag,FILE_SYSTEM_MASK_FILE,path);
}

/*! 
 * ディレクトリ一覧取得
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
void TbFileMapperWiiU::GetDirectories( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    getFileSystems(result,path,pathId,searchFlag,FILE_SYSTEM_MASK_DIR,path);
}

/*!
 * ファイル、ディレクトリ一覧取得
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
void TbFileMapperWiiU::GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    getFileSystems(result,path,pathId,searchFlag,FILE_SYSTEM_MASK_DIR|FILE_SYSTEM_MASK_FILE,path);
}

}