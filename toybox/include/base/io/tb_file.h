/*!
 * ファイルハンドル
 * @author Miyake Shunsuke
 * @since 2013.08.16
 */ 
 
#ifndef _INCLUDED_TB_FILE_H_
#define _INCLUDED_TB_FILE_H_

#include <base/io/tb_file_impl.h>

namespace toybox
{

class TbFile
{
public:

    typedef TbStaticFunction32<void ()> CallbackType;

public:

    // コンストラクタ
    TbFile();

    // コンストラクタ
    TbFile( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // デストラクタ
    virtual ~TbFile();

private:

    TB_DISALLOW_COPY_AND_ASSIGN( TbFile );

public:

    //! 作成(Openはしない)
    TbResult Create(const char* path, TbFileOperateMode operateMode, TbFileSyncMode syncMode, TbFilePathId pathId);

    //! 開く
    TbResult Open(const char* path, TbFileOperateMode operateMode, TbFileSyncMode syncMode, TbFilePathId pathId) {
        Create(path, operateMode, syncMode, pathId);
        return Open();
    }

    //! 一度開いたモノを再度開く
    TbResult Open() {
        if( m_FileImpl ) {
            return m_FileImpl->Open();
        }
        return TB_E_FAIL;
    }

    // 閉じる
    TbResult Close() {
        if( m_FileImpl ) {
            return m_FileImpl->Close();
        }
        return TB_E_FAIL;
    }

    // 実体を解放
    void Release();

public:

    //! ファイルサイズ取得
    TbUint32 GetFileSize() const {
        if( m_FileImpl ) {
            return m_FileImpl->GetFileSize();
        }
        return TB_E_FAIL;
    }

    // シーク
    TbResult Seek( TbSint32 offset , TbIOSeekType type ) {
        if( m_FileImpl ) {
            return m_FileImpl->Seek(offset,type);
        }
        return TB_E_FAIL;
    }

    // 現在のシーク位置を取得
    TbUint32 GetSeek() const {
        if( m_FileImpl ) {
            return m_FileImpl->GetSeek();
        }
        return 0;
    }

    // 全読み込み
    TbResult Read( TbFileAllocMode allocMode = TB_FILE_ALLOC_FORWARD ) {
        if( m_FileImpl ) {
            return m_FileImpl->Read( allocMode );
        }
        return TB_E_FAIL;
    }

    // 読み込み
    TbResult Read( TbUint32 size , TbFileAllocMode allocMode  = TB_FILE_ALLOC_FORWARD ) {
        if( m_FileImpl ) {
            return m_FileImpl->Read( size , allocMode );
        }
        return TB_E_FAIL;
    }

    // ユーザーデータに読み込み
    TbResult Read( TbUint32 size , 
                   void* userBuffer , 
                   TbUint32 userBufferSize , 
                   TbUint32* resultReadReSize ,
                   const TbFile::CallbackType& finishedCalaback = TbFile::CallbackType() ) 
    {
        if( m_FileImpl ) {
            return m_FileImpl->Read( size , userBuffer , userBufferSize , resultReadReSize , finishedCalaback );
        }
        return TB_E_FAIL;
    }

    // シーク位置からユーザーデータ
    TbResult ReadFromSeek( TbUint32 seek , 
                           TbUint32 size  , 
                           void* userBuffer , 
                           TbUint32 userBufferSize ,
                           TbUint32* resultReadReSize ,
                           const TbFile::CallbackType& finishedCalaback = TbFile::CallbackType() ) 
    {
        if( m_FileImpl ) {
            return m_FileImpl->ReadFromSeek( seek , size , userBuffer , userBufferSize , resultReadReSize , finishedCalaback );
        }
        return TB_E_FAIL;
    }

    // ユーザーデータに読み込み
    template <typename T>
    TbResult Read( T* dst ) {
        if( m_FileImpl ) {
            return m_FileImpl->Read( dst );
        }
        return TB_E_FAIL;
    }

    // 書き込み
    TbResult Write( const void* buffer , TbUint32 size ) {
        if( m_FileImpl ) {
            return m_FileImpl->Write( buffer , size );
        }
        return TB_E_FAIL;
    }

    // ユーザー定義処理リクエスト
    TbResult RequestUserFuncAsync(const TbFileUserFunc& userFunc)
    {
        if (m_FileImpl) {
            return m_FileImpl->RequestUserFuncAsync(userFunc);
        }
        return TB_E_FAIL;
    }

    // 非同期処理を強制的に待つ
    void Wait() {
        if( m_FileImpl ) {
            m_FileImpl->Wait();
        }
    }

public:

    // 読み込み、書き込み完了時に自動クローズ可能か
    TbBool IsEnableCloseAuto() const {
        if( m_FileImpl ) {
            return m_FileImpl->IsEnableCloseAuto();
        }
        return TB_FALSE;
    }

    // 読み込み、書き込み完了時に自動クローズ可能か
    void SetEnableCloseAuto( TbBool isEnable ) {
        if( m_FileImpl ) {
            return m_FileImpl->SetEnableCloseAuto(isEnable);
        }
    }

    // ファイルが開かれている
    TbBool IsOpened() const {
        if( m_FileImpl ) {
            return m_FileImpl->IsOpened();
        }
        return TB_FALSE;
    }

    // オペレートモード取得
    TbFileOperateMode GetOperateMode() const {
        if( m_FileImpl ) {
            return m_FileImpl->GetOperateMode();
        }
        return TB_FILE_OPERATE_READ;
    }

    // 同期モード取得
    TbFileSyncMode GetSyncMode() const {
        if( m_FileImpl ) {
            return m_FileImpl->GetSyncMode();
        }
        return TB_FILE_SYNC;
    }

    // 読み込み呼び出し可能
    TbBool IsEnableCallRead() const {
        if( m_FileImpl ) {
            return m_FileImpl->IsEnableCallRead();
        }
        return TB_FALSE;
    }

    // 書き込み呼び出し可能
    TbBool IsEnableCallWrite() const {
        if( m_FileImpl ) {
            return m_FileImpl->IsEnableCallWrite();
        }
        return TB_FALSE;
    }

    // ファイルパスを持っているか
    TbBool HasFilePath() const {
        if( m_FileImpl ) {
            return m_FileImpl->HasFilePath();
        }
        return TB_FALSE;
    }

    // ファイルパス取得
    const TbChar8* GetFilePath() const { 
        if( m_FileImpl ) {
            return m_FileImpl->GetFilePath();
        }
        return NULL;
    }

    //パス取得
    const TbChar8* GetPath() const { 
        if( m_FileImpl ) {
            return m_FileImpl->GetPath();
        }
        return NULL;
    }

    // 状態変数
    TbIOState GetState() const {
        if( m_FileImpl ) {
            return m_FileImpl->GetState();
        }
        return TB_IO_STATE_READY;
    }

    // ビジー状態か
    TbBool IsBusy() const {
        if( m_FileImpl ) {
            return m_FileImpl->IsBusy();
        }
        return TB_FALSE;
    }

    // 処理完了コールバック
    TB_FORCE_INLINE void SetCompletedCallback( const CallbackType& callback ){
        if( m_FileImpl ) {
            return m_FileImpl->SetCompletedCallback(callback);
        }
    }

public: // 読み込み処理専用

    // 読み込んだバッファ取得
    void* GetReadBuffer() {
        if( m_FileImpl ) {
            return m_FileImpl->GetReadBuffer();
        }
        return NULL;
    }

    // 読み込んだバッファ取得
    const void* GetReadBuffer() const{
        if( m_FileImpl ) {
            return m_FileImpl->GetReadBuffer();
        }
        return NULL;
    }

    // バッファサイズ取得
    TbUint32 GetReadBufferSize() const {
        if( m_FileImpl ) {
            return m_FileImpl->GetReadBufferSize();
        }
        return 0;
    }
    
private:

    TbFileImpl* m_FileImpl; // 実体

};

}

#endif
