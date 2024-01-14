/*!
 * @file
 * @brief ファイルクラスの実装ファイル
 * @author teco
 */

#include "tb_fwd.h"
#include "base/io/tb_file_impl.h"
#include "base/io/tb_file_manager.h"
#include "base/io/tb_file_util.h"
#include "base/util/tb_bind.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @path ファイルパス
 * @param operateMode 処理モード
 * @param syncMode 同期モード
 * @parma pathId ルートパスID
 * @author teco
 */
TbFileImpl::TbFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
     : m_BitArray()
     , m_State(TB_IO_STATE_CLOSED)
     , m_FilePath("")
     , m_FilePathId(pathId)
     , m_OperateMode(TB_FILE_OPERATE_READ)
     , m_AllocMode(TB_FILE_ALLOC_FORWARD)
     , m_SyncMode()
     , m_ReadBuffer(NULL)
     , m_ReadBufferSize(0)
     , m_CompletedCallback()
     , m_AsyncCount(0)
{
    setup(path,operateMode,syncMode,pathId);
}

/*!
 * デストラクタ
 * @author teco
 */
TbFileImpl::~TbFileImpl()
{
    TB_SAFE_FREE_DEFAULT_HEAP(m_ReadBuffer);
}

/*!
 * 読みこみバッファを確保
 * @author teco
 */
void* TbFileImpl::AllocReadBuffer(TbUint32 size) {
    if (TB_VERIFY(isEnableRead())) {
        TB_SAFE_FREE_DEFAULT_HEAP(m_ReadBuffer);
        m_ReadBufferSize = size;
        m_ReadBuffer = reinterpret_cast<TbUint8*>(TbMallocDefaultHeap(m_ReadBufferSize,16));
        return m_ReadBuffer;
    }
    return NULL;
}


/*!
 * セットアップ
 * @author teco
 * @since 2011.05.01
 */
void TbFileImpl::setup( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    m_OperateMode = operateMode;
    m_SyncMode = syncMode;
    if( NULL == path ){
        TB_ASSERT( path != NULL );
        m_State = TB_IO_STATE_ERROR;
        return;
    }
    m_FilePath = TbFileUtil::GetFullPath( path , pathId );
}

/*!
 * 非同期ユーザー処理リクエスト
 * @author teco
 */
TbResult TbFileImpl::RequestUserFuncAsync(const TbFileUserFunc& userFunc)
{
    incAsyncCount();
    // デフォルト処理はスレッドに同期処理を逃がす
    TbResult res = TbFileManager::GetInstance().RequestUserFuncAsync(this,
                                                                     userFunc);
    if (TB_SUCCEEDED(res)) {
        return TB_E_PENDING;
    }
    decAsyncCount();
    return TB_E_FAIL;
}

/*!
 * ユーザー処理
 * @author teco
 */
TbBool   TbFileImpl::procUserFunc(const TbFileUserFunc& userFunc)
{
    userFunc();
    decAsyncCount();
    return TB_TRUE; 
}


/*!
 * 非同期読みこみ
 * @author Miyake_Shunsuke
 * @since 2015.01.04
 */
TbResult TbFileImpl::ReadAsyncImpl( TbUint32 size,
                                    TbFileAllocMode allocMode, 
                                    void* userBuffer,
                                    TbUint32 userBufferSize ,
                                    TbSint32 seekPos , 
                                    TbUint32* resultReadSize  ,
                                    const TbFileCallback& completedCallback )
{
    // デフォルト処理はスレッドに同期処理を逃がす
    TbResult res = TbFileManager::GetInstance().RequestReadAsync(this,
                                                                 size,
                                                                 allocMode,
                                                                 userBuffer, 
                                                                 userBufferSize,
                                                                 seekPos,
                                                                 resultReadSize ,
                                                                 completedCallback);
    if( TB_SUCCEEDED(res) ){
        return TB_E_PENDING;
    }
    return TB_E_FAIL;
}

/*!
 * 非同期書き込み
 * @author Miyake_Shunsuke
 * @since 2015.01.08
 */
TbResult TbFileImpl::WriteAsyncImpl( const void* buffer , 
                                    TbUint32 size , 
                                    const TbFileCallback& completedCallback )
{
    // デフォルト処理はスレッドに同期処理を逃がす
    TbResult res = TbFileManager::GetInstance().RequestWriteAsync(this,buffer,size,completedCallback);
    if( TB_SUCCEEDED(res) ){
        return TB_E_PENDING;
    }
    return TB_E_FAIL;
}

/*!
 * 同期読みこみ
 * @author Miyake_Shunsuke
 * @since 2015.01.04
 */
TbResult TbFileImpl::readSync(  TbUint32 size,
                                TbFileAllocMode allocMode, 
                                void* userBuffer ,
                                TbUint32 userBufferSize ,
                                TbSint32 seekPos , 
                                TbUint32* resultReadSize  ,
                                const TbFileCallback& completedCallback )
{
    TbUint32 readSize = 0;
    void* readBuffer = nullptr;

    // 読み込みサイズ確定
    if (0 == size) {
        readSize = GetFileSize();
    } else {
        readSize = TbMin(GetFileSize(), size);
    }
    switch (allocMode)
    {
    case TB_FILE_ALLOC_FORWARD:
    case TB_FILE_ALLOC_REVERSE:
        readBuffer = AllocReadBuffer(readSize);
        break;
    case TB_FILE_ALLOC_USER:
        // バッファが足りない
        if (userBufferSize < readSize) {
            return TB_E_FAIL;
        }
        readBuffer = reinterpret_cast<TbUint8*>(userBuffer);;
        break;
    default:
        TB_ASSERT(0);
        break;
    }
    TbResult res = ReadSyncImpl( readBuffer , readSize , seekPos , resultReadSize );
    if (TB_SUCCEEDED(res))
    {
        if (completedCallback.IsValid()) {
            completedCallback();
        }
        NotifyComplete();
    }else {
        SetState(TB_IO_STATE_ERROR);
    }
    decAsyncCount();
    return res;
}

/*!
 * 同期書き込み
 * @author Miyake_Shunsuke
 * @since 2015.01.08
 */
TbResult TbFileImpl::writeSync( const void* buffer ,
                    TbUint32 size ,
                    const TbFileCallback& completedCallback )
{
    TbResult res = WriteSyncImpl( buffer , size );
    if (TB_SUCCEEDED(res))
    {
        if (completedCallback.IsValid()) {
            completedCallback();
        }
        NotifyComplete();
    }else {
        SetState(TB_IO_STATE_ERROR);
    }
    decAsyncCount();
    return res;
}

/*!
 * 読み込み
 * @author Miyake_Shunsuke
 * @since 2015.01.04
 */
TbResult TbFileImpl::read( TbUint32 size , 
                           TbFileAllocMode allocMode , 
                           void* userBuffer , 
                           TbUint32 userBufferSize ,
                           TbSint32 seekPos , 
                           TbUint32* resultReadSize ,
                           const TbFileCallback& completedCallback )
{

    TbResult res = TB_S_OK;
    incAsyncCount();
    if(TB_FILE_ASYNC == GetSyncMode() ){
        res = ReadAsyncImpl(size, allocMode , userBuffer,userBufferSize, seekPos, resultReadSize , completedCallback);
    }else{
        res = readSync(size, allocMode , userBuffer , userBufferSize , seekPos , resultReadSize , completedCallback );
    }
    if( TB_S_OK != res && TB_E_PENDING != res ) {
        decAsyncCount();
    }
    // 同期処理
    return res;
}

/*!
 * 読み込み
 * @author Miyake_Shunsuke
 * @since 2015.01.08
 */
TbResult TbFileImpl::write( const void* buffer ,
                            TbUint32 size , 
                            const TbFileCallback& completedCallback )
{
    TbResult res = TB_S_OK;
    incAsyncCount();
    if(TB_FILE_ASYNC == GetSyncMode() ){
        res = WriteAsyncImpl(buffer,size,completedCallback);
    }else{
        res = writeSync( buffer , size , completedCallback );
    }
    if( TB_S_OK != res && TB_E_PENDING != res ) {
        decAsyncCount();
    }
    return res;
}

/*!
 * IO処理完了通知
 * @author teco
 * @since 2011.12.02
 */
void TbFileImpl::NotifyComplete()
{
    if( m_CompletedCallback.IsValid() ){
        m_CompletedCallback();
    }
    if( IsEnableCloseAuto() ){
        Close();
    }
}

}