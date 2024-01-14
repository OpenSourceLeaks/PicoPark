/*! 
 * アーカイブ
 * @author Miyake_Shunsuke
 * @since 2014.6.10
 */

#include "tb_fwd.h"
#include "base/io/tb_file.h"
#include "base/io/archive/tb_archive.h"
#include "base/crypt/tb_crc32.h"
#include "base/algorithm/tb_search.h"
#include <base/io/tb_file_util.h>

namespace toybox
{

/*!
 * アーカイブファイルを展開出力
 * @author Miyake_Shunsuke
 * @since 2014.06.21
 */
void TbArchive::Expand( const TbPath& output , const TbPath& input )
{
    TbArchive archive;
    archive.Initialize( input.GetBuffer() , input.GetRootPathId() , TB_TRUE );
    archive.Expand( &output );
}

/*!
 * 展開出力(ファイルパスと同じフォルダに出力)
 * @author Miyake_Shunsuke
 * @since 2014.06.21
 */
void TbArchive::Expand( const char* path , TbFilePathId pathId )
{
    TbPath input( path , pathId );
    TbStaticString512 dir;
    TbFileUtil::GetDirectoryPath(dir,path);
    TbPath output( dir.GetBuffer() , pathId );
    Expand( output , input );
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbArchive::TbArchive()
    : m_Header()
    , m_FileInfo(NULL)
    , m_DirectoryInfo(NULL)
    , m_InfoReadBuf(NULL)
    , m_FileDataBuffer(NULL)
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbArchive::~TbArchive()
{
    TB_SAFE_DELETE_ARRAY(m_InfoReadBuf);
}

/*!
 * メモリ上から読みこみ(メモリ管理はしません)
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void TbArchive::InitializeFromMemory( void* buf , TbSizeT size )
{
    m_BitArray.SetBit(FLAG_READ_ALL,TB_TRUE);
    parse( buf );
    m_FileDataBuffer = reinterpret_cast<TbUint8*>(buf);
}

/*!
 * 設定
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void TbArchive::load( const char* path , TbFileSyncMode syncMode , TbFilePathId pathId , TbBool isEnableReadlAll )
{
    TB_RESQUE(TB_FILE_SYNC == syncMode);
    m_File.Open( path , TB_FILE_OPERATE_READ , syncMode , pathId );
    m_File.SetCompletedCallback( TbCreateMemFunc(this,&Self::onRead) );
    m_BitArray.SetBit(FLAG_READ_ALL,isEnableReadlAll);
    if( isEnableReadlAll ) {
        m_File.Read();
    }else{
        m_File.Read( &m_Header ); 
    }
}

/*!
 * ファイル番号取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
TbUint32 TbArchive::GetFileIndexFromPath( const char* path ) const
{
    if( !m_FileInfo ) {
        return INVALID_INDEX;
    }
    TbCrc32::Alias alias = {'\\','/'};
    TbUint32 hash = TbCrc32::GetHash(path,alias);
    TbUint32 index = TbLowerBoundIndex(m_FileInfo,m_FileInfo+m_Header.fileCount,hash);
    if( index < GetFileCount() ) {
        return index;
    }
    return  INVALID_INDEX;
}

/*!
 * ファイル情報取得
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
const TbArchiveFileInfo* TbArchive::GetFileInfoFromIndex( TbUint32 index ) const
{
    if( !m_FileInfo ) {
        return NULL;
    }
    if( GetFileCount() <= index ) {
        return NULL;
    }
    return &m_FileInfo[index];
}

/*!
 * ファイル情報取得
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
const TbArchiveFileInfo* TbArchive::GetFileInfoFromPath( const char* path ) const
{
    return GetFileInfoFromIndex( GetFileIndexFromPath(path) );
}

/*!
 * ファイルデータ取得
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void* TbArchive::GetFileDataFromIndex( TbUint32 index )
{
    if( !m_BitArray.Test(FLAG_READ_ALL) ){
        // 全部読みをしていないと取得付加
        return NULL;
    }
    const TbArchiveFileInfo* fileInfo = GetFileInfoFromIndex(index);
    return getFileData( fileInfo );
}

/*!
 * ファイルデータ取得
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void* TbArchive::GetFileDataFromPath( const char* path )
{
    if( !m_BitArray.Test(FLAG_READ_ALL) ){
        // 全部読みをしていないと取得付加
        return NULL;
    }
    const TbArchiveFileInfo* fileInfo = GetFileInfoFromPath(path);
    return getFileData( fileInfo );
}

/*!
 * ディレクトリ番号取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
TbUint32 TbArchive::GetDirectoryIndexFromPath( const char* path ) const
{
    if( !m_DirectoryInfo ) {
        return INVALID_INDEX;
    }
    TbCrc32::Alias alias = {'\\','/'};
    TbUint32 hash = TbCrc32::GetHash(path,alias);
    TbSint32 index = TbLowerBoundIndex(m_DirectoryInfo,m_DirectoryInfo+m_Header.dirCount,hash);
    if( 0 <= index && index < static_cast<TbSint32>(m_Header.dirCount) ) {
        return index;
    }
    return  INVALID_INDEX;
}

/*!
 * ディレクトリ情報取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const TbArchiveDirectoryInfo* TbArchive::GetDirectoryInfoFromIndex( TbUint32 index ) const
{
    if( index < m_Header.dirCount ) {
        return &m_DirectoryInfo[index];
    }
    return NULL;
}

/*!
 * ディレクトリ情報取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const TbArchiveDirectoryInfo* TbArchive::GetDirectoryInfoFromPath( const char* path ) const
{
    return GetDirectoryInfoFromIndex( GetDirectoryIndexFromPath(path) );
}

/*!
 * サブディレクトリ情報取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const TbArchiveDirectoryInfo* TbArchive::GetSubDirectoryInfoFromDir( TbUint32 index , TbUint32 subdirIndex ) const
{
    const TbArchiveDirectoryInfo* info = GetDirectoryInfoFromIndex(index);
    return GetSubDirectoryInfoFromDir( info , subdirIndex );
}

/*!
 * サブディレクトリ情報取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const TbArchiveDirectoryInfo* TbArchive::GetSubDirectoryInfoFromDir( const TbArchiveDirectoryInfo* info , TbUint32 subdirIndex ) const
{
    if( info && subdirIndex < info->subdirCount ) {
        TbAddr addr = info->subdirOffset - sizeof(TbArchiveHeader) + reinterpret_cast<TbAddr>(m_FileInfo);
        TbUint32* subdirs = reinterpret_cast<TbUint32*>(addr);
        return GetDirectoryInfoFromIndex(subdirs[subdirIndex]);
    }
    return NULL;
}

/*!
 * ファイル情報取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const TbArchiveFileInfo* TbArchive::GetFileInfoFromDir( TbUint32 index , TbUint32 fileIndex ) const
{
    const TbArchiveDirectoryInfo* info = GetDirectoryInfoFromIndex(index);
    return GetFileInfoFromDir( info , fileIndex );
}

/*!
 * ファイル情報取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
const TbArchiveFileInfo* TbArchive::GetFileInfoFromDir( const TbArchiveDirectoryInfo* info , TbUint32 fileIndex ) const
{
    if( info && fileIndex < info->fileCount ) {
        TbAddr addr = info->fileOffset - sizeof(TbArchiveHeader) + reinterpret_cast<TbAddr>(m_FileInfo);
        TbUint32* files = reinterpret_cast<TbUint32*>(addr);
        return GetFileInfoFromIndex(files[fileIndex]);
    }
    return NULL;
}


/*!
 * ファイルデータ取得
 * @author Miyake_Shunsuke
 * @since 2014.06.22
 */
void* TbArchive::getFileData( const TbArchiveFileInfo* info )
{
    if( !m_BitArray.Test(FLAG_READ_ALL) ){
        // 全部読みをしていないと取得付加
        return NULL;
    }
    if( !info || !m_FileDataBuffer ) {
        return NULL;
    }
    return &m_FileDataBuffer[info->offset];
}

/*!
 * 読みこみ完了
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void TbArchive::onRead()
{
    if( !IsReady() )
    {
        if( m_BitArray.Test(FLAG_READ_ALL) ){
            parse( m_File.GetReadBuffer() );
            m_FileDataBuffer = reinterpret_cast<TbUint8*>(m_File.GetReadBuffer());
        }else{
            if( !m_FileInfo ) {
                TB_ASSERT(TB_SUCCEEDED(parseHeader(&m_Header)));
                TbUint32 readSize = m_Header.offsetData - sizeof(TbArchiveHeader);
                m_InfoReadBuf = TB_NEW TbUint8[readSize];
                m_FileInfo = reinterpret_cast<TbArchiveFileInfo*>(m_InfoReadBuf);
                if( m_Header.dirCount > 0 ){
                    m_DirectoryInfo = reinterpret_cast<TbArchiveDirectoryInfo*>(&m_FileInfo[m_Header.fileCount]);
                }
                m_File.Read(readSize,m_InfoReadBuf,readSize,nullptr);
            }else{
                resolveAddress();
                m_BitArray.SetBit(FLAG_READY);
            }
        }
    }
    else
    {
//        TbPrintf("read\n");
    }
}

/*!
 * アドレス解決
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void TbArchive::resolveAddress()
{
    if( !m_FileInfo || 0 == m_Header.fileCount ) {
        return;
    }
    if( !(m_Header.flag & TbArchiveHeader::FLAG_BIT_RESOLVE) )
    {
        for( TbUint32 i = 0; i < m_Header.fileCount; ++i ) 
        {
            m_FileInfo[i].path = resolveInfoString(m_FileInfo[i].path);
        }
        for( TbUint32 i = 0; i < m_Header.dirCount; ++i ) 
        {
            m_DirectoryInfo[i].path = resolveInfoString(m_DirectoryInfo[i].path);
        }
        m_Header.flag |= TbArchiveHeader::FLAG_BIT_RESOLVE;
    }
}

/*!
 * ヘッダーのみパース
 * @author Miyake_Shunsuke
 * @since 2014.06.22
 */
TbResult TbArchive::parseHeader( void* data )
{
    TbArchiveHeader* header = reinterpret_cast<TbArchiveHeader*>(data);
    if( header->version == TB_ARCHIVE_VERSION )
    {
        if( &m_Header != data ){
            m_Header = *header;
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * パース
 * @author Miyake_Shunsuke
 * @since 2014.06.22
 */
TbResult TbArchive::parse( void* data )
{
    if( TB_FAILED(parseHeader(data)) ){
        return TB_E_FAIL;
    }
    TbArchiveHeader* header = reinterpret_cast<TbArchiveHeader*>(data);
    if( m_Header.fileCount > 0 ){
        m_FileInfo = reinterpret_cast<TbArchiveFileInfo*>(header+1);
    }
    if( m_Header.dirCount > 0 ){
        m_DirectoryInfo = reinterpret_cast<TbArchiveDirectoryInfo*>(&m_FileInfo[m_Header.fileCount]);
    }
    resolveAddress();
    m_BitArray.SetBit(FLAG_READY);
    return TB_S_OK;
}

/*!
 * 展開(ツール用)
 * @author Miyake_Shunsuke
 * @since 2014.06.21
 */
TbResult TbArchive::Expand( const TbPath* output )
{
    if( !IsReady() || !m_BitArray.Test(FLAG_READ_ALL) ){
        return TB_E_FAIL;
    }
    TbStaticString512 dir;
    if( output ){
        dir = output->GetFullPath().GetBuffer();
    }else if( m_File.HasFilePath() ){
        TbFileUtil::GetDirectoryPath(dir,m_File.GetFilePath());
    }else{
        dir = ".";
    }
    for( TbUint32 i = 0; i < m_Header.fileCount; ++i ) 
    {
        TbStaticString512 path = dir;
        if( !path.IsEmpty() ){
            path += "/";
        }
        path += m_FileInfo[i].path;

        {
            TbStaticString512 localDir;
            TbFileUtil::GetDirectoryPath(localDir,path.GetBuffer());
            if( !TbFileUtil::ExistsDirectory(localDir.GetBuffer(),TB_FILE_PATH_ID_NONE ) ){
                TbFileUtil::CreateDirectory(localDir.GetBuffer(),TB_FILE_PATH_ID_NONE,TB_TRUE);
            }
        }

        TbFile outputFile( path.GetBuffer() , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
        if( TB_SUCCEEDED( outputFile.Open() ) ){
            outputFile.Write( GetFileDataFromIndex(i) , m_FileInfo[i].size );
        }
    }
    return TB_S_OK;
}

/*!
 * アーカイブ内のファイル読みこみ
 * @author Miyake_Shunsuke
 * @since 2014.06.22
 */
TbResult TbArchive::ReadFileAsync( TbUint32 index , 
                                   void* readBuf , 
                                   TbSizeT readSize , 
                                   TbSizeT offset , 
                                   TbUint32* resultReadSize ,
                                   const TbFile::CallbackType& finishedCallback )
{
    if( !IsReady() ){
        return TB_E_FAIL;
    }
    const TbArchiveFileInfo* fileInfo = GetFileInfoFromIndex( index );
    if( !fileInfo ){
        return TB_E_FAIL;
    }
    TbUint8* data = reinterpret_cast<TbUint8*>(getFileData( fileInfo ));
    if( data ){
        // 読みこみ済み
        TbMemCopy(readBuf,&data[offset],readSize);
        if( resultReadSize ){
            *resultReadSize = readSize;
        }
        if( finishedCallback.IsValid() ){
            finishedCallback();
        }
        return TB_S_OK;
    }
    return m_File.ReadFromSeek(fileInfo->offset+offset,readSize,readBuf,readSize,resultReadSize,finishedCallback);
}

}
