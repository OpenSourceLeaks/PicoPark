/*!
 * @file
 * @brief NXを利用したファイルクラスの実装ファイル
 * @author teco
 */

#include "tb_fwd.h"
#include "base/math/tb_math_util.h"
#include "base/io/depend/nx/tb_file_nx.h"
#include "base/io/tb_file_util.h"
#include "base/task/tb_task.h"

namespace toybox
{

//////////////

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileNX::GetFileSize( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullpath( TbFileUtil::GetFullPath( path , pathId ) );
    nn::fs::FileHandle handle;
    nn::Result ret = nn::fs::OpenFile(&handle, fullpath.GetBuffer(), nn::fs::OpenMode_Read);
    if (!ret.IsSuccess()) {
        return 0;
    }
    int64_t result = 0;
    ret = nn::fs::GetFileSize(&result, handle);
    nn::fs::CloseFile(handle);
    if (!ret.IsSuccess()) {
        TB_ASSERT(0);
        return 0;
    }
    return result;
}

/*!
 * ファイル存在しているか
 * @author teco
 */
TbBool TbFileNX::ExistsFile( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullpath( TbFileUtil::GetFullPath( path , pathId ) );

    nn::fs::FileHandle handle;
    nn::Result result = nn::fs::OpenFile(&handle, fullpath.GetBuffer(), nn::fs::OpenMode_Read);
    if (result.IsSuccess()) {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * コンストラクタ
 * @path ファイルパス
 * @param operateMode 処理モード
 * @param syncMode 同期モード
 * @parma pathId ルートパスID
 * @author teco
 */
TbFileNX::TbFileNX( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
     : Super(path,operateMode,syncMode,pathId)
     , m_IsOpened(TB_FALSE)
     , m_FilePos(0)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbFileNX::~TbFileNX()
{
    Close();
}

/*!
 * 開く
 * @author teco
 */
TbResult TbFileNX::Open()
{
    if(m_IsOpened){
        // 開いている場合は、一度Closeを呼んでください
        return TB_E_DENY;
    }

    int mode = 0;
    switch( GetOperateMode() )
    {
    case TB_FILE_OPERATE_READ:
        mode = nn::fs::OpenMode_Read;
        break;
    case TB_FILE_OPERATE_WRITE:
        mode = nn::fs::OpenMode_Write | nn::fs::OpenMode_AllowAppend;
        break;
    default:
        TB_ASSERT(0);
        return TB_E_FAIL; 
        break;
    }

    nn::Result result = nn::fs::OpenFile( &m_FileHandle , GetPath() , mode );

    // 失敗処理
    if(result.IsFailure()){
        TbDebugPrintf("[FILE] Open Failed : %s\n" , GetPath());
        SetState( TB_IO_STATE_ERROR );
        return TB_E_FAIL;
    }

    TbDebugPrintf("[FILE] Open Success : %s\n" , GetPath());
    SetState( TB_IO_STATE_READY );
    m_IsOpened = TB_TRUE;
    return TB_S_OK;
}

/*!
 * シーク
 * @author teco
 */
TbResult TbFileNX::Seek( TbSint32 size , TbIOSeekType type )
{
    switch (type)
    {
    case TB_IO_SEEK_TYPE_BEGIN:
        m_FilePos = size;
        break;
    case TB_IO_SEEK_TYPE_CURRENT:
        m_FilePos += size;
        break;
    case TB_IO_SEEK_TYPE_END:
        {
            m_FilePos = GetFileSize();
        }
        break;
    default:
        break;
    }
    return TB_S_OK;
}

/*!
 * シーク
 * @author teco
 */
TbUint32 TbFileNX::GetSeek() const
{
    return m_FilePos;
}

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileNX::GetFileSize() const
{
    int64_t result = 0U;
    if( m_IsOpened ){
        nn::fs::GetFileSize( &result , m_FileHandle );
    }
    return static_cast<TbUint32>(result);
}

/*!
 * 読み込み開始
 * @param allocMode メモリ確保モード
 * @param size 読み込みサイズ
 * @param userBuffer allocModeがALLOC_MODE_USERの場合に利用するバッファ
 */
TbResult TbFileNX::ReadSyncImpl( void* readBuffer ,
                                   TbUint32 readSize ,
                                   TbSint32 seekPos ,
                                   TbUint32* resultReadSize )
{
    if (!m_IsOpened) {
        return TB_E_FAIL;
    }
    if( seekPos >= 0 ){
        Seek( seekPos , TB_IO_SEEK_TYPE_BEGIN );
    }
    size_t resultSize = 0;
    nn::fs::ReadOption      readOption = nn::fs::ReadOption::MakeValue(0);
    nn::Result result = nn::fs::ReadFile(&resultSize, m_FileHandle , m_FilePos , readBuffer, readSize , readOption);
    if( resultReadSize ) {
        *resultReadSize = resultSize;
    }
    if(result.IsSuccess()){
        m_FilePos += resultSize;
//        TbPrintf("[FILE] Read Finish : %s\n" , GetPath());
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 * @since 2011.11.30
 */
TbResult TbFileNX::WriteSyncImpl( const void* buffer , TbUint32 size )
{
    if (!m_IsOpened) {
        return TB_E_FAIL;
    }

    nn::fs::WriteOption      option = nn::fs::WriteOption::MakeValue(nn::fs::WriteOptionFlag_Flush);
    nn::Result result = nn::fs::WriteFile(m_FileHandle,m_FilePos,buffer,size,option);
    m_FilePos += size;

    if(TB_VERIFY(result.IsSuccess())){
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * クローズ処理
 * @author Miyake Shunsuke
 * @since 2011.11.29
 */
TbResult TbFileNX::Close()
{
    SetState(TB_IO_STATE_CLOSED);
    if( m_IsOpened ){
        nn::fs::CloseFile(m_FileHandle);
        m_IsOpened = TB_FALSE;
        if (GetFilePathId() == TB_FILE_PATH_ID_PRESET_SAVE) {
            const char* name[] = { "save" };
            nn::fs::Commit(name,1);
        }
    }
    return TB_S_OK;
}

}