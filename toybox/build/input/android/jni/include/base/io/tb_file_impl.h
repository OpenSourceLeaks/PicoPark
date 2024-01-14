/*!
 * @file
 * @brief ファイルクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_IMPL_H_
#define _INCLUDED_TB_FILE_IMPL_H_

#include <base/io/tb_file_types.h>
#include <base/io/tb_io_types.h>
#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>

namespace toybox
{

class TbFileImpl
{
    friend class TbFileManager;
    typedef TbFileImpl Self;

public:

    // パス取得
    static const TbStaticString256 GetPath( const char* path , TbFilePathId pathId );

protected:

    // コンストラクタ
    TbFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // デストラクタ
    virtual ~TbFileImpl();

private:

    TB_DISALLOW_COPY_AND_ASSIGN( TbFileImpl );

public: // 継承

    // 開く
    virtual TbResult Open() = 0;

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize() const = 0;

    // シーク
    virtual TbResult Seek( TbSint32 offset , TbIOSeekType type ) = 0;
    
    // 現在のシーク位置を取得
    virtual TbUint32 GetSeek() const = 0;

    // 読み込み
    TbResult Read( TbFileAllocMode allocMode = TB_FILE_ALLOC_FORWARD  ) {
        return Read( 0 , allocMode );
    }

    // 読み込み
    TbResult Read( TbUint32 size , TbFileAllocMode allocMode = TB_FILE_ALLOC_FORWARD  ) {
        if(!isEnableRead()) {
            TB_ASSERT(!"Invalid Call");
            return TB_E_FAIL;
        }
        return read( size , allocMode , NULL , 0 );
    }

    // ユーザーデータに読み込み
    TbResult Read(  TbUint32 size , 
                    void* userBuffer , 
                    TbUint32 userBufferSize , 
                    TbUint32* resultReadSize ,
                    const TbFileCallback& callback = TbFileCallback() ) 
    {
        if(!isEnableRead()) {
            TB_ASSERT(!"Invalid Call");
            return TB_E_FAIL;
        }
        return read( size , TB_FILE_ALLOC_USER , userBuffer , userBufferSize , -1 , resultReadSize , callback );
    }

    // シーク位置からユーザーデータに読み込み
    TbResult ReadFromSeek( TbUint32 seekPos , 
                           TbUint32 size , 
                           void* userBuffer , 
                           TbUint32 userBufferSize , 
                           TbUint32* resultReadSize ,
                           const TbFileCallback& callback = TbFileCallback() ) {
        if(!isEnableRead()) {
            TB_ASSERT(!"Invalid Call");
            return TB_E_FAIL;
        }
        return read( size , TB_FILE_ALLOC_USER , userBuffer , userBufferSize , static_cast<TbSint32>(seekPos) , resultReadSize , callback );
    }

    // ユーザーデータに読み込み
    template <typename T>
    TbResult Read( T* dst ) {
        if(!isEnableRead()) {
            TB_ASSERT(!"Invalid Call");
            return TB_E_FAIL;
        }
        return read( sizeof(T) , TB_FILE_ALLOC_USER , dst , sizeof(T) );
    }

    // 書き込み
    TbResult Write( const void* buffer , TbUint32 size ) {
        if( !IsEnableCallWrite() || TB_IO_STATE_READY != GetState() ){
            TB_ASSERT(!"Invalid Call");
            return TB_E_FAIL;
        }
        return write( buffer , size );
    }

    // 閉じる
    virtual TbResult Close() = 0;

    // 待ち
    void Wait() {
        while( IsBusy() ){}
    }

public:

    // 読み込み、書き込み完了時に自動クローズ可能か
    TbBool IsEnableCloseAuto() const {
        return m_BitArray.Test( FLAG_CLOSE_AUTO );
    }

    // 読み込み、書き込み完了時に自動クローズ可能か
    void SetEnableCloseAuto( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_CLOSE_AUTO , isEnable );
    }

    // ファイルが開かれている
    TbBool IsOpened() const {
        return (m_State != TB_IO_STATE_CLOSED && m_State != TB_IO_STATE_ERROR);
    }

    // オペレートモード取得
    TbFileOperateMode GetOperateMode() const {
        return m_OperateMode;
    }

    // 同期モード取得
    TbFileSyncMode GetSyncMode() const {
        return m_SyncMode;
    }

    // 読み込み呼び出し可能
    TbBool IsEnableCallRead() const {
        return m_OperateMode == TB_FILE_OPERATE_READ;
    }

    // 書き込み呼び出し可能
    TbBool IsEnableCallWrite() const {
        return m_OperateMode == TB_FILE_OPERATE_WRITE;
    }

    // ファイルパスを持っているか
    TbBool HasFilePath() const {
        return !m_FilePath.IsEmpty();
    }

    // ファイルパス取得
    const TbChar8* GetFilePath() const { 
        return m_FilePath.GetBuffer(); 
    }

    //パス取得
    const TbChar8* GetPath() const { 
        return m_FilePath.GetBuffer(); 
    }

    // ファイルパスID取得
    TbFilePathId GetFilePathId() const {
        return m_FilePathId;
    }

    // 状態変数
    TbIOState GetState() const {
        return m_State;
    }

    // ビジー状態か
    TbBool IsBusy() const {
        return m_AsyncCount.Load() != 0;
    }

    // 処理完了コールバック
    TB_FORCE_INLINE void SetCompletedCallback( const TbFileCallback& callback ){
        m_CompletedCallback = callback;
    }

public: // 読み込み処理専用(内部確保)

    // 読み込んだバッファ取得
    void* GetReadBuffer() {
        return m_ReadBuffer;
    }

    // 読み込んだバッファ取得
    const void* GetReadBuffer() const{
        return m_ReadBuffer;
    }

    // 読みこみバッファを確保
    void* AllocReadBuffer( TbUint32 size );

    // バッファサイズ取得
    TbUint32 GetReadBufferSize() const {
        return m_ReadBufferSize;
    }

public:

    // 非同期ユーザー処理リクエスト
    TbResult RequestUserFuncAsync(const TbFileUserFunc& userFunc);

protected: // 派生クラスから呼び出し可能

    // 状態設定
    void SetState( TbIOState state ) {
        if( 0 <= state && state < TB_IO_STATE_MAX ){
            m_State = state;
        }
    }

    // 処理完了
    void NotifyComplete();

protected: // 派生クラスからの継承関係

    // 非同期読みこみ
    virtual TbResult ReadAsyncImpl(TbUint32 size,
                                    TbFileAllocMode allocMode,
                                    void* userBuffer ,
                                    TbUint32 userBufferSize,
                                    TbSint32 seekPos , 
                                    TbUint32* resultReadSize ,
                                    const TbFileCallback& completedCallback );

    // 同期読みこみ
    virtual TbResult ReadSyncImpl( void* readBuffer ,
                                   TbUint32 readSize ,
                                   TbSint32 seekPos ,
                                   TbUint32* resultReadSize ) = 0;

    // 書き込み
    virtual TbResult WriteAsyncImpl( const void* buffer , 
                                     TbUint32 size , 
                                     const TbFileCallback& completedCallback );

    // 書き込み
    virtual TbResult WriteSyncImpl( const void* buffer , TbUint32 size ) = 0;

private:

    enum {
        FLAG_CLOSE_AUTO , 
    };

private:

    // セットアップ
    void setup( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // 読みこみ可能
    TbBool isEnableRead() const {
        if( !IsEnableCallRead() ){
            return TB_FALSE;
        }
        // 非同期読みこみのみ同時読みこみを許可
        return TB_IO_STATE_READY==GetState();
    }

    // 読み込み
    TbResult read( TbUint32 size , 
                   TbFileAllocMode allocMode , 
                   void* userBuffer , 
                   TbUint32 userBufferSize ,
                   TbSint32 seeksPos = -1 , 
                   TbUint32* resultReadSize = nullptr ,
                   const TbFileCallback& completedCallback= TbFileCallback() );

    // 読み込み
    TbResult write( const void* buffer ,
                    TbUint32 size , 
                    const TbFileCallback& completedCallback= TbFileCallback() );

private:

    // 同期読みこみ
    TbResult readSync( TbUint32 size, 
                       TbFileAllocMode allocMode,
                       void* userBuffer ,
                       TbUint32 userBufferSize ,
                       TbSint32 seekPos , 
                       TbUint32* resultReadSize  ,
                       const TbFileCallback& completedCallback );

    // 同期書き込み
    TbResult writeSync( const void* buffer ,
                        TbUint32 size ,
                        const TbFileCallback& completedCallback );

    // ユーザー処理
    TbBool   procUserFunc( const TbFileUserFunc& userFunc );

    // 非同期カウンタ加算
    void incAsyncCount() {
        ++m_AsyncCount;
    }

    // 非同期カウンタ減算
    void decAsyncCount() {
        --m_AsyncCount;
    }

private:

    TbBitArray32       m_BitArray;                //!< ビット配列
    TbIOState          m_State;                   //!< 状態変数
    TbStaticString512  m_FilePath;                //!< ファイルパス
    TbFilePathId       m_FilePathId;              //!< ファイルパスID
    TbFileOperateMode  m_OperateMode;             //!< 挙動モード
    TbFileAllocMode    m_AllocMode;               //!< ヒープ確保モード
    TbFileSyncMode     m_SyncMode;                //!< 読み込みモード
    TbFileCallback     m_CompletedCallback;       //!< 完了コールバック

private: // 読みこみ用

    TbUint8*           m_ReadBuffer;              //!< 内部読み込みバッファ
    TbUint32           m_ReadBufferSize;          //!< 内部読み込みバッファサイズ

private: // 非同期用

    TbAtomic<TbSint32> m_AsyncCount;              //!< 非同期処理カウンタ

};

}

#endif
