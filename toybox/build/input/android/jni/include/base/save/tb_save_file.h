/*!
 * ファイルハンドル
 * @author teco
 */ 
 
#ifndef _INCLUDED_TB_SAVE_FILE_H_
#define _INCLUDED_TB_SAVE_FILE_H_

#include <base/io/tb_file.h>

namespace toybox
{

class TbSaveFile
{
public:

    // コンストラクタ
    TbSaveFile() : m_SyncMode(TB_FILE_SYNC ) {
        m_File.SetEnableCloseAuto(TB_TRUE);
    }

    // デストラクタ
    virtual ~TbSaveFile() {}

private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbSaveFile);

public:

    //! 開く
    void Initialize(const char* path, TbFileSyncMode syncMode) {
        m_FilePath = path;
        m_SyncMode = syncMode;
    }

public:

    // 全読み込み
    TB_DEPEND TbResult Read( TbFileAllocMode allocMode = TB_FILE_ALLOC_FORWARD );

    // 書き込み
     TB_DEPEND TbResult Write( const void* buffer , TbUint32 size );

    // 非同期処理を強制的に待つ
    void Wait() {
        m_File.Wait();
    }

public:

    // 同期モード取得
    TbFileSyncMode GetSyncMode() const {
        return m_File.GetSyncMode();
    }

    //パス取得
    const TbChar8* GetPath() const { 
        return m_File.GetPath();
    }

    // 状態変数
    TbIOState GetState() const {
        return m_File.GetState();
    }

    // ビジー状態か
    TbBool IsBusy() const {
        return m_File.IsBusy();
    }

    // 処理完了コールバック
    void SetCompletedCallback( const TbFile::CallbackType& callback ){
        m_File.SetCompletedCallback(callback);
    }

public: // 読み込み処理専用

    // 読み込んだバッファ取得
    void* GetReadBuffer() {
        return m_File.GetReadBuffer();
    }

    // 読み込んだバッファ取得
    const void* GetReadBuffer() const{
        return m_File.GetReadBuffer();
    }

    // バッファサイズ取得
    TbUint32 GetReadBufferSize() const {
        return m_File.GetReadBufferSize();
    }
    
private:

    TbFile              m_File;
    TbStaticString256   m_FilePath;
    TbFileSyncMode      m_SyncMode;

};

}

#endif
