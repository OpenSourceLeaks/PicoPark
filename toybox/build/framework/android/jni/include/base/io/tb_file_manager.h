/*!
 * ファイル管理システムのヘッダーファイル
 * @author Miayke Shunsuke
 */

#ifndef _INCLUDED_TB_FILE_MANAGER_H_
#define _INCLUDED_TB_FILE_MANAGER_H_

#include <base/io/tb_file_types.h>
#include <base/io/tb_path.h>
#include <base/util/tb_singleton.h>
#include <base/string/tb_static_string.h>
#include <base/container/tb_pool.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_array.h>
#include "depend/tb_file_mapper_depend.h"

#include <base/thread/tb_wait_notify_thread.h>

#define TB_FILE_IS_ENABLE_ASYNC ((1 && !TB_THREADAPI_IS_NONE) || TB_THREADAPI_IS_NX)

namespace toybox
{

class TbFileImpl;
class TbFileMapper;

/*!
 * ファイル管理システム
 */
class TbFileManager : public TbSingletonRef<TbFileManager>
{
public:

    typedef TbArray<TbStaticPath512> PathList;
    typedef TbFileManager Self;

public:

    //デフォルトコンストラクタ
    TbFileManager();

    //コンストラクタ
    explicit TbFileManager( const TbUint32 applicationRootPathMax );

    //デストラクタ
    ~TbFileManager();

public:

    // マッパー追加
    void AddMapper( TbFileMapper* mapper );

public:

    // アプリケーション用ルートパス最大数設定
    TbResult SetApplicationRootPathMax( const TbUint32 applicationRootPathMax );

    // ルートパス追加
    TbFilePathId AddRootPath( const char* path );

    // ルートパス除去
    TbResult RemoveRootPath( const TbFilePathId path );

    // ルートパス設定
    TbResult SetRootPath( const TbFilePathId pathId , const char* path );

    // ルートパス取得
    const TbStaticString512& GetRootPath( const TbFilePathId pathId ) const;

    // ルートパス有効チェック
    TbBool IsValidRootPath( const TbFilePathId pathId ) const;

public:

    // ディレクトリ作成
    TbResult CreateDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリ削除
    TbResult DeleteDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイルサイズ取得
    TbUint32 GetFileSize( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル存在するか
    TbBool ExistsFile( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリは存在するか
    TbBool ExistsDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル一覧取得
    void GetFiles( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリ一覧取得
    void GetDirectories( PathList& result ,const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル、ディレクトリ一覧取得
    void GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    //! ファイル実体生成
    TbFileImpl* CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode ) {
        return CreateFileImpl( path , operateMode , syncMode , TB_FILE_PATH_ID_PRESET_ROOT );
    }

    // ファイル実体生成
    TbFileImpl* CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // ファイル実体削除
    TbResult DeleteFileImpl( TbFileImpl* file );

public: // スレッド処理

    // 非同期処理
    TbResult RequestUserFuncAsync(TbFileImpl* file, const TbFileUserFunc& userFunc);

    // 非同期処理
    TbResult RequestReadAsync( TbFileImpl* file , 
                               TbUint32 size,
                               TbFileAllocMode allocMode,
                               void* userBuffer , 
                               TbUint32 userBufferSize,
                               TbSint32 seekPos , 
                               TbUint32* resultReadSize ,
                               const TbFileCallback& finishedCallback );

    // 非同期処理
    TbResult RequestWriteAsync( TbFileImpl* file , const void* buffer , TbUint32 size , const TbFileCallback& finishedCallback );

public:

    // 定期処理
    void Update();

private:

    typedef TbArray<TbFileMapper*> MapperList;

    static const TbUint32 ASYNC_FILE_MAX = 128; // 同時に処理する非同期ファイル数

    struct AsyncUserInfo
    {
        AsyncUserInfo()
            : file(nullptr)
        {}
        TbFileImpl*     file;
        TbFileUserFunc  userFunc;
    };

    struct AsyncReadInfo
    {
        AsyncReadInfo()
            : file(nullptr)
            , allocMode(TB_FILE_ALLOC_FORWARD)
            , userBuffer(nullptr)
            , userBufferSize(0)
            , size(0)
            , seekPos(0)
            , resultReadSize(nullptr)
        {}
        TbFileImpl* file;
        TbFileAllocMode allocMode;
        void* userBuffer;
        TbUint32 userBufferSize;
        TbUint32 size;
        TbSint32 seekPos;
        TbUint32* resultReadSize;
        TbFileCallback finishedCallback;
    };

    struct AsyncWriteInfo
    {
        AsyncWriteInfo()
            : file(nullptr)
            , buffer(nullptr)
            , size(0)
        {}
        TbFileImpl* file;
        const void* buffer;
        TbUint32 size;
        TbFileCallback finishedCallback;
    };

private:

    // 初期化
    void init();

    // ルートパス取得
    TbStaticString512* getRootPath( TbFilePathId pathId );

    // スレッド処理
    void onProcThread();

private:

    TbPool<TbStaticString512>                                   m_ApplicationRootPath;    // アプリケーション用ルートパス
    TbStaticArray<TbStaticString512,TB_FILE_PATH_ID_PRESET_MAX> m_SystemRootPath;         // システム用ルートパス
    TbUint32                                                    m_FileCount;              // 生成済みファイル数
    MapperList                                                  m_FileMapperList;         // ファイルマッパーリスト
    TbFileMapperDefault                                         m_FileMapperDefault;

private: // スレッド対応

#if TB_FILE_IS_ENABLE_ASYNC
    TbWaitNotifyThread      m_Thread;
    TbMutex                 m_Mutex;
#endif
    AsyncUserInfo           m_AsyncUserFile[ASYNC_FILE_MAX];
    AsyncReadInfo           m_AsyncReadFile[ASYNC_FILE_MAX];
    AsyncWriteInfo          m_AsyncWriteFile[ASYNC_FILE_MAX];

};

}

#endif
