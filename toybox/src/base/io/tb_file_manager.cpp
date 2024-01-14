/*!
 * @file 
 * @brief ファイル管理クラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/io/tb_file_manager.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_mapper.h"
#include "base/io/tb_file_util_private.h"
#include "base/algorithm/tb_search.h"

namespace toybox
{

namespace{
    static const TbUint32 DEFAULT_APPLICATION_ROOT_PATH_MAX = 10;

    // Priortyは高いほど優先される
    struct MapperCompareFunc
    {
        TbBool operator()( TbFileMapper* left , TbSint32 right) const {
            return left->GetPriority() > right;
        }
#if 0
        TbBool operator()( TbSint32 left , TbFileMapper* right) const {
            return left > right->GetPriority();
        }
        TbBool operator()( TbFileMapper* left , TbFileMapper* right ) const {
            return left->GetPriority() > right->GetPriority();
        }
#endif
    };
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbFileManager::TbFileManager()
    : m_ApplicationRootPath( DEFAULT_APPLICATION_ROOT_PATH_MAX )
    , m_SystemRootPath()
    , m_FileCount(0)
{
    init();
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbFileManager::TbFileManager( const TbUint32 applicationRootPathMax )
    : m_ApplicationRootPath( applicationRootPathMax )
    , m_SystemRootPath()
    , m_FileCount(0)
{
    init();
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbFileManager::~TbFileManager()
{
#if TB_FILE_IS_ENABLE_ASYNC
    // スレッド終了待ち
    m_Thread.RequestFinish();
    m_Thread.Notify();
    m_Thread.Join();
#endif
}

/*! 
 * 初期化
 * @author Miyake_Shunsuke
 * @since 2014.07.18
 */
void TbFileManager::init()
{
    for( TbUint32 i = 0; i < TB_FILE_PATH_ID_PRESET_MAX; ++i ){
        m_SystemRootPath.PushBack(".");
    }
    m_FileMapperList.SetEnableSetCapacityAuto(TB_TRUE);
    m_FileMapperList.SetCapacity(10);
    AddMapper(&m_FileMapperDefault);

    // スレッド設定
#if TB_FILE_IS_ENABLE_ASYNC
    {
        TbThreadParam threadParam;
        threadParam.name = "TbFileManager";
        m_Thread.Initialize(threadParam,TbCreateMemFunc(this,&Self::onProcThread));
        m_Thread.Start();
    }
#endif

}

/*!
 * 定期処理
 * @author teco
 */
void TbFileManager::Update()
{
#if !TB_FILE_IS_ENABLE_ASYNC
    onProcThread();
#endif
}

/*!
 * マッパー追加
 * @author Miyake_Shunsuke
 * @since 2014.07.18
 */
void TbFileManager::AddMapper( TbFileMapper* mapper )
{
    TbUint32 index = TbLowerBoundIndex(m_FileMapperList.Begin(),m_FileMapperList.End(),mapper->GetPriority(),MapperCompareFunc());
    m_FileMapperList.Insert(index,mapper);
}

/*!
 * ファイル生成
 * @author Miyake Shunsuke
 * @since 2011.12.02
 */
TbFileImpl* TbFileManager::CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    TbFileImpl* result = NULL;
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        if( operateMode == TB_FILE_OPERATE_WRITE && !mapper->IsEnableWrite() ) {
            // 書き込みはできない
            continue;
        }
        if( operateMode != TB_FILE_OPERATE_WRITE && (mapper->NeedCheckExists() && !mapper->ExistsFile(path,pathId)) ){
            // 読みこみモードでファイルが存在しない
            continue;
        }
        result = mapper->CreateFileImpl( path , operateMode , syncMode , pathId );
        if( result ) {
            break;
        }
    }
    return result;
}

/*!
 * ファイル削除
 * @author Miyake Shunsuke
 * @since 2011.12.02
 */
TbResult TbFileManager::DeleteFileImpl( TbFileImpl* file )
{
    if( file ){
//        file->Close(); // デストラクタ側でCloseのタイミングを制御したい可能性もあるのでしない
        TB_DELETE file;
        --m_FileCount;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ディレクトリ作成
 * @author Miyake_Shunsuke
 * @since 2014.07.21
 */
TbResult TbFileManager::CreateDirectory( const char* path , TbFilePathId pathId )
{
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        if( !mapper->IsEnableWrite() ){
            continue;
        }
        return mapper->CreateDirectory(path,pathId);
    }
    return TB_E_FAIL;
}

/*!
 * ディレクトリ削除
 * @author Miyake_Shunsuke
 * @since 2014.07.21
 */
TbResult TbFileManager::DeleteDirectory( const char* path , TbFilePathId pathId )
{
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        if( mapper->NeedCheckExists() && !mapper->ExistsDirectory(path,pathId) ){
            continue;
        }
        return mapper->DeleteDirectory(path,pathId);
    }
    return TB_E_FAIL;
}

/*!
 * ファイルサイズ取得
 * @author Miyake Shunsuke
 * @since 2011.12.02
 */
TbUint32 TbFileManager::GetFileSize( const char* path , TbFilePathId pathId )
{
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        if( mapper->NeedCheckExists() && !mapper->ExistsFile(path,pathId) ){
            continue;
        }
        return mapper->GetFileSize(path,pathId);
    }
    return 0;
}

/*!
 * ファイル存在するか
 * @author Miyake Shunsuke
 * @since 2011.12.02
 */
TbBool TbFileManager::ExistsFile( const char* path , TbFilePathId pathId )
{
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        if( mapper->NeedCheckExists() && !mapper->ExistsFile(path,pathId) ){
            continue;
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ディレクトリは存在するか
 * @author Miyake_Shunsuke
 * @since 2014.07.18
 */
TbBool TbFileManager::ExistsDirectory( const char* path , TbFilePathId pathId )
{
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        if( !mapper->ExistsDirectory(path,pathId) ){
            continue;
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ファイル一覧取得
 * @author Miyake_Shunsuke
 * @since 2014.07.18
 */
void TbFileManager::GetFiles( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TbStaticString512 pathStr = path;
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        // ディレクトリが存在するマッパーを利用
        if( !mapper->ExistsDirectory(pathStr.GetBuffer(),pathId) ){
            continue;
        }
        mapper->GetFiles( result , path , searchFlag , pathId );
        return;
    }
}

/*!
 * ディレクトリ一覧取得
 * @author Miyake_Shunsuke
 * @since 2014.07.18
 */
void TbFileManager::GetDirectories( PathList& result ,const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TbStaticString512 pathStr = path;
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        // ディレクトリが存在するマッパーを利用
        if( !mapper->ExistsDirectory(pathStr.GetBuffer(),pathId) ){
            continue;
        }
        mapper->GetDirectories( result , path , searchFlag , pathId );
        return;
    }
}

/*!
 * ファイル、ディレクトリ一覧取得
 * @author Miyake_Shunsuke
 * @since 2014.07.18
 */
void TbFileManager::GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag , TbFilePathId pathId )
{
    TbStaticString512 pathStr = path;
    for( MapperList::Iterator ite = m_FileMapperList.Begin() , endIte = m_FileMapperList.End();
         ite != endIte; 
         ++ite )
    {
        TbFileMapper* mapper = *ite;
        if( !mapper->IsEnable(pathId) ){
            continue;
        }
        // ディレクトリが存在するマッパーを利用
        if( !mapper->ExistsDirectory(pathStr.GetBuffer(),pathId) ){
            continue;
        }
        mapper->GetFileSystemEntries( result , path , searchFlag , pathId );
        return;
    }
}

/*!
 * アプリケーション用ルートパス最大数設定
 * @author Miyake Shunsuke
 * @since 2011.05.01
 */
TbResult TbFileManager::SetApplicationRootPathMax( const TbUint32 applicationRootPathMax )
{
    m_ApplicationRootPath.SetCapacity( applicationRootPathMax );
    return TB_S_OK;
}

/*!
 * ルートパス追加
 * @author Miyake Shunsuke
 * @since 2011.04.24
 */
TbFilePathId TbFileManager::AddRootPath( const char* path )
{
    TbFilePathId pathId = m_ApplicationRootPath.Add( path );
    return pathId;
}

/*!
 * ルートパス除去
 * @author Miyake Shunsuke
 * @since 2011.04.24
 */
TbResult TbFileManager::RemoveRootPath( const TbFilePathId pathId )
{
    if( TB_FILE_PATH_ID_IS_APPLICATION(pathId) ){
        m_ApplicationRootPath.Remove( pathId );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ルートパス設定
 * @author Miyake Shunsuke
 * @since 2011.04.24
 */
TbResult TbFileManager::SetRootPath( const TbFilePathId pathId , const char* path )
{
    if ( TB_FILE_PATH_ID_NONE == pathId ){
        return TB_E_FAIL;
    }
    if( TB_FILE_PATH_ID_IS_APPLICATION(pathId) ){
        *m_ApplicationRootPath.GetAt( pathId ) = path;
        // todo かんすとちぇっく
        return TB_S_OK;
    }else{
        TbUint32 index = static_cast<TbUint32>( pathId - TB_FILE_PATH_ID_PRESET_BEGIN );
        if( index < TB_FILE_PATH_ID_PRESET_MAX ){
            m_SystemRootPath[index] = path;
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * ルートパス取得
 * @author Miyake Shunsuke
 * @since 2011.04.24
 */
const TbStaticString512& TbFileManager::GetRootPath( const TbFilePathId pathId ) const
{
    if ( TB_FILE_PATH_ID_NONE == pathId ){
        return TbStaticString512::EMPTY();
    }
    if( TB_FILE_PATH_ID_IS_APPLICATION(pathId) ){
        return *m_ApplicationRootPath.GetAt(pathId);
    }else{
        TbUint32 index = static_cast<TbUint32>( pathId - TB_FILE_PATH_ID_PRESET_BEGIN );
        if( index < TB_FILE_PATH_ID_PRESET_MAX ){
            return m_SystemRootPath[index];
        }
    }
	return TbStaticString512::EMPTY();
}

/*!
 * ルートパスが有効か
 * @author Miyake Shunsuke
 * @since 2011.07.25
 */
TbBool TbFileManager::IsValidRootPath( const TbFilePathId pathId ) const
{
    if ( TB_FILE_PATH_ID_NONE == pathId ){
        return TB_TRUE;
    }
    if( TB_FILE_PATH_ID_IS_APPLICATION(pathId) ){
        if( m_ApplicationRootPath.GetAt(pathId) ){
            return TB_TRUE;
        }
    }else{
        TbUint64 index = static_cast<TbUint32>( pathId - TB_FILE_PATH_ID_PRESET_BEGIN );
        if( index < TB_FILE_PATH_ID_PRESET_MAX ){
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 非同期処理
 * @author teco
 */
TbResult TbFileManager::RequestUserFuncAsync(TbFileImpl* file, const TbFileCallback& finishedCallback)
{
    AsyncUserInfo* target = nullptr;
    {
#if TB_FILE_IS_ENABLE_ASYNC
        TbScopedLock lock(m_Mutex);
#endif
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_AsyncReadFile); ++i) {
            if (!m_AsyncUserFile[i].file) {
                target = &m_AsyncUserFile[i];
                break;
            }
        }
    }
    if (target) {
        target->file = file;
        target->userFunc = finishedCallback;
#if TB_FILE_IS_ENABLE_ASYNC
        m_Thread.Notify();
#endif
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 非同期処理
 * @author Miyake_Shunsuke
 * @since 2015.01.06
 */
TbResult TbFileManager::RequestReadAsync( TbFileImpl* file , 
                                          TbUint32 size,
                                          TbFileAllocMode allocMode,
                                          void* userBuffer ,
                                          TbUint32 userBufferSize,
                                          TbSint32 seekPos , 
                                          TbUint32* resultReadSize , 
                                          const TbFileCallback& finishedCallback )
{
    AsyncReadInfo* target = nullptr;
    {
#if TB_FILE_IS_ENABLE_ASYNC
        TbScopedLock lock(m_Mutex);
#endif
        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_AsyncReadFile); ++i ) {
            if( !m_AsyncReadFile[i].file ) {
                target = &m_AsyncReadFile[i];
                break;
            }
        }
    }
    if( target ) {
        target->allocMode = allocMode;
        target->userBuffer = userBuffer;
        target->userBufferSize = userBufferSize;
        target->size = size;
        target->seekPos = seekPos;
        target->resultReadSize = resultReadSize;
        target->finishedCallback = finishedCallback;
        target->file = file;
#if TB_FILE_IS_ENABLE_ASYNC
        m_Thread.Notify();
#endif
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 非同期処理
 * @author Miyake_Shunsuke
 * @since 2015.01.06
 */
TbResult TbFileManager::RequestWriteAsync( TbFileImpl* file , const void* readBuffer , TbUint32 size , const TbFileCallback& finishedCallback )
{
    AsyncWriteInfo* target = nullptr;
    {
#if TB_FILE_IS_ENABLE_ASYNC
        TbScopedLock lock(m_Mutex);
#endif
        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_AsyncWriteFile); ++i ) {
            if( !m_AsyncWriteFile[i].file ) {
                target = &m_AsyncWriteFile[i];
                break;
            }
        }
    }
    if( target ) {
        target->buffer = readBuffer;
        target->size = size;
        target->finishedCallback = finishedCallback;
        target->file = file;
#if TB_FILE_IS_ENABLE_ASYNC
        m_Thread.Notify();
#endif
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * スレッド処理
 * @author Miyake_Shunsuke
 * @since 2015.01.04
 */
void TbFileManager::onProcThread()
{
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_AsyncUserFile); ++i) {
        if (m_AsyncUserFile[i].file) {
            m_AsyncUserFile[i].file->procUserFunc(m_AsyncUserFile[i].userFunc);
            m_AsyncUserFile[i].file = nullptr;
        }
    }
    // m_AsyncReadFile[i].fileをnullptrに書き換えるのはこのスレッドだけなのでこっちではロックしない
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_AsyncReadFile); ++i ) {
        if( m_AsyncReadFile[i].file ){
            m_AsyncReadFile[i].file->readSync( m_AsyncReadFile[i].size ,
                                               m_AsyncReadFile[i].allocMode ,
                                               m_AsyncReadFile[i].userBuffer ,
                                               m_AsyncReadFile[i].userBufferSize ,
                                               m_AsyncReadFile[i].seekPos ,
                                               m_AsyncReadFile[i].resultReadSize , 
                                               m_AsyncReadFile[i].finishedCallback );
            m_AsyncReadFile[i].file = nullptr;
        }
    }
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_AsyncWriteFile); ++i ) {
        if( m_AsyncWriteFile[i].file ){
            m_AsyncWriteFile[i].file->writeSync( m_AsyncWriteFile[i].buffer ,
                                                 m_AsyncWriteFile[i].size ,
                                                 m_AsyncWriteFile[i].finishedCallback );
            m_AsyncWriteFile[i].file = nullptr;
        }
    }
}

}
