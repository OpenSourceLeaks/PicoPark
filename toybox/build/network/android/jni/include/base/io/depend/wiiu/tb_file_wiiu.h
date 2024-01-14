/*!
 * @file
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_WIIU_H_
#define _INCLUDED_TB_FILE_WIIU_H_

#include <base/io/tb_file_impl.h>
#include <base/container/tb_array.h>

namespace toybox
{

class TbFileManager;

/*!
 * @class 
 */
class TbFileWiiU : public TbFileImpl
{
    typedef TbFileImpl Super;
    friend class TbFileManager;
    friend class TbFileMapperWiiU;

public:

    // ファイルサイズ取得
    static TbUint32 GetFileSize( const char* path , TbFilePathId pathId );

    // ファイル存在しているか
    static TbBool ExistsFile( const char* path , TbFilePathId pathId );

protected:

    // コンストラクタ
    TbFileWiiU( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // デストラクタ
    virtual ~TbFileWiiU();

private:

    TB_DISALLOW_COPY_AND_ASSIGN( TbFileWiiU );

public:

    // 開く
    virtual TbResult Open();

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize() const;

    // 閉じる
    virtual TbResult Close();

    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type);
    
    // 現在のシーク位置を取得
    virtual TbUint32 GetSeek() const;

protected:

   // 同期読みこみ
    virtual TbResult ReadSyncImpl( void* readBuffer ,
                                   TbUint32 readSize ,
                                   TbSint32 seekPos ,
                                   TbUint32* resultReadSize ) TB_OVERRIDE;

    // 書き込み
    virtual TbResult WriteSyncImpl( const void* buffer , TbUint32 size );
    
private:
    
    FSFileHandle                    m_Handle;

};

}

#endif
