/*!
 * @file
 * @brief アーカイブから読みこむファイルクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_ARCHIVE_H_
#define _INCLUDED_TB_FILE_ARCHIVE_H_

#include <base/io/tb_file_impl.h>

namespace toybox
{

class TbFileManager;
class TbArchive;

/*!
 * @class 
 */
class TbFileArchive : public TbFileImpl
{
    typedef TbFileImpl Super;
    typedef TbFileArchive Self;
    friend class TbFileManager;
    friend class TbFileMapperArchive;

protected:

    // コンストラクタ
    TbFileArchive( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbArchive* archive );

    // デストラクタ
    virtual ~TbFileArchive();

private:

    TB_DISALLOW_COPY_AND_ASSIGN( TbFileArchive );

public:

    // 開く
    virtual TbResult Open() TB_OVERRIDE;

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize() const TB_OVERRIDE;

    // 閉じる
    virtual TbResult Close() TB_OVERRIDE;

    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type) TB_OVERRIDE;

    // シーク位置
    virtual TbUint32 GetSeek() const  TB_OVERRIDE {
        return m_Seek;
    }

protected:

#if 0
    // 非同期読みこみ
    virtual TbResult ReadAsyncImpl( void* readBuffer ,
                            TbUint32 readSize ,
                            TbSint32 seekPos , 
                            const TbFileCallback& completedCallback );
#endif
    
    // 同期読みこみ
    virtual TbResult ReadSyncImpl( void* readBuffer ,
                                   TbUint32 readSize ,
                                   TbSint32 seekPos ,
                                   TbUint32* resultReadSize ) TB_OVERRIDE;

    // 書き込み
    virtual TbResult WriteSyncImpl( const void* buffer , TbUint32 size ) TB_OVERRIDE;

private:

    // 読みこみ完了
    void onCompletedRead( TbUint32 nextSeek );

private:

    TbArchive*              m_Archive;
    TbUint32                m_FileIndex;
    TbUint32                m_Seek;

};

}

#endif
