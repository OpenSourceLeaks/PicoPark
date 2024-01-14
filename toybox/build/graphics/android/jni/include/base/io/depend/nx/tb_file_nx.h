/*!
 * @file
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_NX_H_
#define _INCLUDED_TB_FILE_NX_H_

#include <base/io/tb_file_impl.h>
#include <base/container/tb_array.h>

namespace toybox
{

class TbFileManager;

/*!
 * @class 
 */
class TbFileNX : public TbFileImpl
{
    typedef TbFileImpl Super;
    friend class TbFileManager;
    friend class TbFileMapperNX;

public:

    // ファイルサイズ取得
    static TbUint32 GetFileSize( const char* path , TbFilePathId pathId );

    // ファイル存在しているか
    static TbBool ExistsFile( const char* path , TbFilePathId pathId );

protected:

    // コンストラクタ
    TbFileNX( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // デストラクタ
    virtual ~TbFileNX();

private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbFileNX);

public:

    // 開く
    virtual TbResult Open() TB_OVERRIDE;

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize() const TB_OVERRIDE;

    // 閉じる
    virtual TbResult Close() TB_OVERRIDE;

    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type) TB_OVERRIDE;
    
    // 現在のシーク位置を取得
    virtual TbUint32 GetSeek() const TB_OVERRIDE;

protected:

   // 同期読みこみ
    virtual TbResult ReadSyncImpl( void* readBuffer ,
                                   TbUint32 readSize ,
                                   TbSint32 seekPos ,
                                   TbUint32* resultReadSize ) TB_OVERRIDE;

    // 書き込み
    virtual TbResult WriteSyncImpl( const void* buffer , TbUint32 size ) TB_OVERRIDE;

private:

    nn::fs::FileHandle      m_FileHandle;
    TbBool                  m_IsOpened;
    TbSizeT                 m_FilePos;

};

}

#endif
