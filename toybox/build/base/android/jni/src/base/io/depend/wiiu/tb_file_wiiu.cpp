/*!
 * @file
 * @brief Win32APIを利用したファイルクラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_math_util.h"
#include "base/io/depend/wiiu/tb_file_wiiu.h"
#include "base/io/tb_file_util.h"
#include "base/task/tb_task.h"

namespace toybox
{

//////////////

/*!
 * ファイルサイズ取得
 * @author Miyake Shunsuke
 */
TbUint32 TbFileWiiU::GetFileSize( const char* path , TbFilePathId pathId )
{
    return TbFileMapperWiiU::GetInstance().GetFileSize(path,pathId);
}

/*!
 * ファイル存在しているか
 * @author Miyake Shunsuke
 */
TbBool TbFileWiiU::ExistsFile( const char* path , TbFilePathId pathId )
{
    return TbFileMapperWiiU::GetInstance().ExistsFile(path,pathId);
}

/*!
 * コンストラクタ
 * @path ファイルパス
 * @param operateMode 処理モード
 * @param syncMode 同期モード
 * @parma pathId ルートパスID
 * @author Miyake Shunsuke
 */
TbFileWiiU::TbFileWiiU( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
     : Super(path,operateMode,syncMode,pathId)
     , m_Handle(FS_INVALID_HANDLE_VALUE )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbFileWiiU::~TbFileWiiU()
{
    Close();
}

/*!
 * 開く
 * @author Miyake Shunsuke
 */
TbResult TbFileWiiU::Open()
{
    TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
    
    if( FS_INVALID_HANDLE_VALUE  != m_Handle ){
        // 開いている場合は、一度Closeを呼んでください
        return TB_E_DENY;
    }

    const char* mode = nullptr;

    switch( GetOperateMode() )
    {
    case TB_FILE_OPERATE_READ:
        mode = "rb";
        break;
    case TB_FILE_OPERATE_WRITE:
        mode = "wb";
        break;
    default:
        TB_ASSERT(0);
        return TB_E_FAIL; 
    }
    
    m_Handle = FSOpenFile( mapper.GetClient() ,
                           mapper.GetCmdBlock() ,
                           GetPath() , 
                           "r" ,
                           &m_Handle ,
                           FS_RET_NO_ERROR
                         );
    
    // 失敗処理
    if( FS_INVALID_HANDLE_VALUE == m_Handle ){
        TbDebugPrintf("[FILE] Open Failed : %s\n" , GetPath());
        SetState( TB_IO_STATE_ERROR );
        return TB_E_FAIL;
    }

    TbDebugPrintf("[FILE] Open Success : %s\n" , GetPath());
    SetState( TB_IO_STATE_READY );
    return TB_S_OK;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 */
TbResult TbFileWiiU::Seek( TbSint32 size , TbIOSeekType type )
{
    TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
    FSFilePosition pos = 0;
    switch( type )
    {
    case TB_IO_SEEK_TYPE_BEGIN:
        pos = size;
        break;
    case TB_IO_SEEK_TYPE_CURRENT:
        pos = GetSeek() + size;
        break;
    case TB_IO_SEEK_TYPE_END:
        TB_ASSERT(0==size);
        pos = GetFileSize();
        break;
    }
    
    if( FS_STATUS_OK == FSSetPosFile( mapper.GetClient() ,
                                      mapper.GetCmdBlock(),
                                      m_Handle,
                                      pos,
                                      FS_RET_NO_ERROR) )
    {
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 */
TbUint32 TbFileWiiU::GetSeek() const
{
    TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
    FSFilePosition pos = 0;
    if( FS_STATUS_OK == FSGetPosFile(mapper.GetClient() ,
                                     mapper.GetCmdBlock(),
                                     m_Handle,
                                     &pos,
                                     FS_RET_NO_ERROR) )
    {
        return pos;
    }
    return 0;
}

/*!
 * ファイルサイズ取得
 * @author Miyake Shunsuke
 */
TbUint32 TbFileWiiU::GetFileSize() const
{
    TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
    TbUint32 result = 0U;
    FSStat stat;
    if( FS_STATUS_OK == FSGetStatFile(mapper.GetClient() ,
                                      mapper.GetCmdBlock(),
                                      m_Handle,
                                      &stat,
                                      FS_RET_NO_ERROR) )
    {
        result = stat.size;
    }
    return result;
}

/*!
 * 読み込み開始
 * @param allocMode メモリ確保モード
 * @param size 読み込みサイズ
 * @param userBuffer allocModeがALLOC_MODE_USERの場合に利用するバッファ
 */
TbResult TbFileWiiU::ReadSyncImpl( void* readBuffer ,
                                   TbUint32 readSize ,
                                   TbSint32 seekPos ,
                                   TbUint32* resultReadSize )
{
    TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
    if( seekPos >= 0 ){
        Seek( seekPos , TB_IO_SEEK_TYPE_BEGIN );
    }
    TbUint32 beginPos = 0;
    if( resultReadSize ) {
        beginPos = GetSeek();
    }
    FSStatus status = FSReadFile( mapper.GetClient() ,
                                  mapper.GetCmdBlock(),
                                  readBuffer,
                                  1 ,
                                  readSize,
                                  m_Handle, 
                                  0, 
                                  FS_RET_NO_ERROR);
    if( resultReadSize ) {
        TbUint32 endPos = GetSeek();
        *resultReadSize = endPos = beginPos;
    }
    if(TB_VERIFY(FS_STATUS_OK==status)){
//        TbPrintf("[FILE] Read Finish : %s\n" , GetPath());
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 */
TbResult TbFileWiiU::WriteSyncImpl( const void* buffer , TbUint32 size )
{
    TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
    FSStatus status = FSWriteFile( mapper.GetClient() ,
                                  mapper.GetCmdBlock(),
                                  buffer,
                                  1 ,
                                  size ,
                                  m_Handle , 
                                  0, 
                                  FS_RET_NO_ERROR);
    if(TB_VERIFY(FS_STATUS_OK==status)){
//        TbPrintf("[FILE] Write Finish : %s\n" , GetPath());
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * クローズ処理
 * @author Miyake Shunsuke
 */
TbResult TbFileWiiU::Close()
{
    SetState(TB_IO_STATE_CLOSED);
    if( FS_INVALID_HANDLE_VALUE  != m_Handle ){
        TbFileMapperWiiU& mapper = TbFileMapperWiiU::GetInstance();
        FSCloseFile(mapper.GetClient(),mapper.GetCmdBlock(),m_Handle,FS_RET_NO_ERROR);
        m_Handle = FS_INVALID_HANDLE_VALUE; 
    }
    return TB_S_OK;
}

}