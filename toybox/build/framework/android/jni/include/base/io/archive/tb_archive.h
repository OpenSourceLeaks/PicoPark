/*! 
 * アーカイブ
 * @author Miyake_Shunsuke
 * @since 2014.6.10
 */
 
#ifndef _INCLUDED_TB_ARCHIVE_H_
#define _INCLUDED_TB_ARCHIVE_H_

#include <base/io/tb_file.h>
#include <base/io/archive/tb_archive_types.h>
#include <base/io/tb_path.h>

namespace toybox
{

class TbArchive
{
public:

    static const TbUint32 INVALID_INDEX = static_cast<TbUint32>(-1);

public:

    typedef TbArchive Self;

    // 展開出力(完了復帰)
    static void Expand( const TbPath& output , const TbPath& input );

    // 展開出力(ファイルパスと同じフォルダに出力)
    static void Expand( const char* path , TbFilePathId pathId );

public:
    
    // コンストラクタ
    TbArchive();
    
    // デストラクタ
    ~TbArchive();

public:

    // 初期化(即時復帰)
    void Initialize( const char* path , TbFilePathId pathId , TbBool isEnableReadAll ) {
        load( path , TB_FILE_SYNC , pathId , isEnableReadAll );
    }

    // メモリ上から初期化
    void InitializeFromMemory( void* buf , TbSizeT size );

    // 利用可能状態か
    TbBool IsReady() const {
        return m_BitArray.Test(FLAG_READY);
    }

    // ビジー中
    TbBool IsBusy() const {
        return m_File.IsBusy();
    }

    // 完了待ち
    void Wait() {
        m_File.Wait();
    }

public: // ヘッダー情報

    // ファイル情報取得
    TbUint32 GetFileIndexFromPath( const char* path ) const;

    // ファイル情報取得
    const TbArchiveFileInfo* GetFileInfoFromIndex( TbUint32 index ) const;

    // ファイル情報取得
    const TbArchiveFileInfo* GetFileInfoFromPath( const char* path ) const;

    // ディレクトリ番号取得
    TbUint32 GetDirectoryIndexFromPath( const char* path ) const;

    // ディレクトリ情報取得
    const TbArchiveDirectoryInfo* GetDirectoryInfoFromIndex( TbUint32 index ) const;

    // ディレクトリ情報取得
    const TbArchiveDirectoryInfo* GetDirectoryInfoFromPath( const char* path ) const;

    // サブディレクトリ情報取得
    const TbArchiveDirectoryInfo* GetSubDirectoryInfoFromDir( TbUint32 index , TbUint32 subdirIndex ) const;

    // サブディレクトリ情報取得
    const TbArchiveDirectoryInfo* GetSubDirectoryInfoFromDir( const TbArchiveDirectoryInfo* info , TbUint32 subdirIndex ) const;

    // ファイル情報取得
    const TbArchiveFileInfo* GetFileInfoFromDir( TbUint32 index , TbUint32 fileIndex ) const;

    // ファイル情報取得s
    const TbArchiveFileInfo* GetFileInfoFromDir( const TbArchiveDirectoryInfo* info , TbUint32 fileIndex ) const;

    // ファイル数
    TbUint32 GetFileCount() const {
        return m_Header.fileCount;
    }

    // ディレクトリ数
    TbUint32 GetDirecotryCount() const {
        return m_Header.dirCount;
    }

public: // 全て読み込ませている場合に有効

    // ファイルデータ取得
    void* GetFileDataFromIndex( TbUint32 index );

    // ファイルデータ取得
    void* GetFileDataFromPath( const char* path );

    // 展開出力(全部読み込んでいる必要がある)
    TbResult Expand( const TbPath* output );

public: // IO処理

    // アーカイブ内のファイル読みこみ
    TbResult ReadFileAsync( TbUint32 fileIndex , 
                            void* readBuf , 
                            TbSizeT readSize , 
                            TbSizeT offset = 0 , 
                            TbUint32* resultReadSize = nullptr ,
                            const TbFileCallback& finishedCallback =  TbFileCallback() );

    // アーカイブ内のファイル読みこみ
    TbResult ReadFileAsync( const char* filePath , 
                            void* readBuf , 
                            TbSizeT readSize , 
                            TbSizeT offset = 0 , 
                            TbUint32* resultReadSize = nullptr ,
                            const TbFileCallback& finishedCallback =  TbFileCallback() ) 
    {
        TbUint32 fileIndex = GetFileIndexFromPath( filePath );
        return ReadFileAsync( fileIndex , readBuf , readSize , offset ,resultReadSize ,finishedCallback );
    }

    // アーカイブ内のファイル読みこみ(完了復帰)
    TbResult ReadFileSync( TbUint32 index , 
                           void* readBuf , 
                           TbSizeT readSize , 
                           TbSizeT offset = 0 , 
                           TbUint32* resultReadSize = nullptr ,
                           const TbFileCallback& finishedCallback =  TbFileCallback() ) 
    {
        TbResult result = ReadFileAsync( index , readBuf , readSize , offset , resultReadSize ,finishedCallback );
        if( TB_E_PENDING == result ) {
            m_File.Wait();
            return TB_S_OK;
        }
        return result;
    }

    // アーカイブ内のファイル読みこみ(完了復帰)
    TbResult ReadFileSync( const char* filePath , 
                           void* readBuf , 
                           TbSizeT readSize , 
                           TbSizeT offset = 0 , 
                           TbUint32* resultReadSize = nullptr ,
                           const TbFileCallback& finishedCallback = TbFileCallback() ) 
    {
        TbUint32 fileIndex = GetFileIndexFromPath( filePath );
        return ReadFileSync( fileIndex , readBuf , readSize , offset ,resultReadSize ,finishedCallback );
    }

private:

    enum Flag
    {
        FLAG_READY , 
        FLAG_READ_ALL ,
    };

private:

    // 読みこみ完了
    void onRead();

    // 読みこみ
    void load( const char* path , TbFileSyncMode syncMode , TbFilePathId pathId , TbBool isEnableReadlAll);

    // ヘッダーのみパース
    TbResult parseHeader( void* data );

    // パース
    TbResult parse( void* data );

    // アドレス解決
    void resolveAddress();

    // 文字列アドレス解決
    const char* resolveInfoString( const char* addr ){
        TbAddr address = reinterpret_cast<TbAddr>(addr);
        address -= sizeof(TbArchiveHeader);
        address += reinterpret_cast<TbAddr>(m_FileInfo);
        return reinterpret_cast<const char*>(address);
    }

    // ファイルデータ取得
    void* getFileData( const TbArchiveFileInfo* info );

private:

    TbBitArray32        m_BitArray;
    TbFile              m_File;
    TbArchiveHeader     m_Header;
    TbArchiveFileInfo*  m_FileInfo;
    TbArchiveDirectoryInfo*  m_DirectoryInfo;
    TbUint8*            m_InfoReadBuf;
    TbUint8*            m_FileDataBuffer;

};

}

#endif
