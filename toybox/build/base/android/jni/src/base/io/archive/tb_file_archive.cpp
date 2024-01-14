/*!
 * @file
 * @brief アーカイブを利用したファイルクラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_math_util.h"
#include "base/io/archive/tb_file_archive.h"
#include "base/io/archive/tb_archive.h"
#include "base/io/tb_file_util.h"
#include "base/task/tb_task.h"
#include "base/util/tb_bind.h"

namespace toybox
{

//////////////

/*!
 * コンストラクタ
 * @path ファイルパス
 * @param operateMode 処理モード
 * @param syncMode 同期モード
 * @parma pathId ルートパスID
 * @author Miyake Shunsuke
 */
TbFileArchive::TbFileArchive( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbArchive* archive )
     : Super(path,operateMode,syncMode,TB_FILE_PATH_ID_NONE)
     , m_Archive(archive)
     , m_FileIndex(TbArchive::INVALID_INDEX)
     , m_Seek(0)
{
    TB_ASSERT(m_Archive);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbFileArchive::~TbFileArchive()
{
}

/*!
 * 開く
 * @author Miyake Shunsuke
 * @since 2011.11.29
 */
TbResult TbFileArchive::Open()
{
    TbResult result = TB_S_OK;
    m_FileIndex = m_Archive->GetFileIndexFromPath(GetPath());
    if( !m_Archive->GetFileInfoFromIndex(m_FileIndex) ){
        // ファイルが存在しない
        TbDebugPrintf("[FILE][ARCHIVE] Open Failed : %s\n" , GetPath());
        result = TB_E_FAIL;
    }else{
        TbDebugPrintf("[FILE][ARCHIVE] Open Success : %s\n" , GetPath());
        SetState( TB_IO_STATE_READY );
    }
    return result;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 * @since 2012.03.27
 */
TbResult TbFileArchive::Seek( TbSint32 size , TbIOSeekType type )
{
    TbResult result = TB_S_OK;
    TbUint32 seek = GetSeek();
    switch( type )
    {
    case TB_IO_SEEK_TYPE_BEGIN:
        seek = size;
        break;
    case TB_IO_SEEK_TYPE_CURRENT:
        seek += size;
        break;
    case TB_IO_SEEK_TYPE_END:
        seek = GetFileSize()+size;
        break;
    default:
            TB_ASSERT(0);
        break;
    }
    seek = TbMin( seek , GetFileSize() );
    m_Seek = seek;
    return result;
}

/*!
 * ファイルサイズ取得
 * @author Miyake Shunsuke
 * @since 2011.11.30
 */
TbUint32 TbFileArchive::GetFileSize() const
{
    const TbArchiveFileInfo* info = m_Archive->GetFileInfoFromIndex(m_FileIndex);
    TbUint32 result = 0;
    if( info ){
        result = info->size;
    }
    return result;
}

#if 0
/*!
 * 非同期読みこみ
 * @author Miyake_Shunsuke
 * @since 2015.01.10
 */
TbResult TbFileArchive::ReadAsyncImpl( void* readBuffer ,
                        TbUint32 readSize ,
                        TbSint32 seekPos , 
                        const TbFileCallback& completedCallback )
{
    if( seekPos >= 0 ) {
        Seek( seekPos , TB_IO_SEEK_TYPE_BEGIN );
    }
    TbUint32 seek = seekPos >= 0 ? seekPos : GetSeek();
    TbUint32 nextSeek = GetSeek() + readSize;
    TbResult res = m_Archive->ReadFileAsync(m_FileIndex,
                                           readBuffer,
                                           readSize,
                                           seek,
                                           completedCallback);
    Seek(nextSeek,TB_IO_SEEK_TYPE_BEGIN);
    return res;
}
#endif

/*!
 * 同期読みこみ
 * @author Miyake_Shunsuke
 * @since 2015.01.09
 */
TbResult TbFileArchive::ReadSyncImpl( void* readBuffer ,
                                      TbUint32 readSize ,
                                      TbSint32 seekPos ,
                                      TbUint32* resultReadSize)
{
    if( seekPos >= 0 ) {
        Seek( seekPos , TB_IO_SEEK_TYPE_BEGIN );
    }
    TbUint32 seek = seekPos >= 0 ? seekPos : GetSeek();
    TbUint32 nextSeek = GetSeek() + readSize;
    TbResult res = m_Archive->ReadFileSync(m_FileIndex,
                                           readBuffer,
                                           readSize,
                                           seek,
                                           resultReadSize);
    Seek(nextSeek,TB_IO_SEEK_TYPE_BEGIN);
    return res;
}

/*!
 * 同期書き込み
 * @author Miyake_Shunsuke
 * @since 2015.01.09
 */
TbResult TbFileArchive::WriteSyncImpl( const void* buffer , TbUint32 size )
{
    TB_ASSERT(!"not supported");
    return TB_S_OK;
}

/*!
 * クローズ処理
 * @author Miyake Shunsuke
 * @since 2011.11.29
 */
TbResult TbFileArchive::Close()
{
    m_Seek = 0;
    SetState( TB_IO_STATE_CLOSED );
    return TB_S_OK;
}

}