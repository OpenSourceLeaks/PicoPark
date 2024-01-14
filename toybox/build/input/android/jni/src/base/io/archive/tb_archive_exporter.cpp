/*! 
 * アーカイブExporter
 * @author Miyake_Shunsuke
 * @since 2014.6.10
 */
 
#include "tb_fwd.h"
#include "base/io/archive/tb_archive_exporter.h"
#include "base/io/tb_file_util.h"
#include "base/util/tb_binary_write_buffer.h"
#include "base/crypt/tb_crc32.h"
#include "base/algorithm/tb_sort.h"

namespace toybox
{

/*!
 * 出力
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbResult TbArchiveExporter::Export( const char* path , TbFilePathId pathId  )
{
    TbFile file( path  , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , pathId );
    if( !TB_SUCCEEDED(file.Open()) ){
        return TB_E_FAIL;
    }

    // ルートディレクトリ情報構築
    

    TbInsertionSort(m_FileInfos.Begin(),m_FileInfos.End());
    TbInsertionSort(m_DirectoryInfo.Begin(),m_DirectoryInfo.End());

    TbUint32 size = calcFileSize();
    TbBinaryWriteBuffer buffer(size);

    TbArchiveHeader* header = buffer.Add<TbArchiveHeader>(1);
    header->version = TB_ARCHIVE_VERSION;
    header->fileCount = m_FileInfos.GetCount();
    header->dirCount = m_DirectoryInfo.GetCount();

    TbArray<TbArchiveFileInfo*> dataPtr;
    dataPtr.SetCapacity(m_FileInfos.GetCount());
    {
        TbArchiveFileInfo* files = buffer.Add<TbArchiveFileInfo>(m_FileInfos.GetCount());
        TbArray<FileInfo>::Iterator fileIte = m_FileInfos.Begin();
        TbArray<FileInfo>::Iterator fileEndIte = m_FileInfos.End();
        // ファイル情報追加
        for( ; fileIte != fileEndIte; ++fileIte )
        {
            TbArchiveFileInfo* result = files++;
            TbMemClear(*result);
            dataPtr.PushBack(result);
            buffer.AddStringPtr(fileIte->path.GetBuffer(),&result->path);
            result->nameIndex = TbFileUtil::GetFileNameIndex( fileIte->path );
            result->hash = fileIte->hash;
            result->size = fileIte->size;
        }
    }

    // ディレクトリ情報追加
    {
        TbArray<TbArchiveDirectoryInfo*> dirDataPtr;
        dirDataPtr.SetCapacity(m_DirectoryInfo.GetCount());

        TbArchiveDirectoryInfo* dirs = buffer.Add<TbArchiveDirectoryInfo>(m_DirectoryInfo.GetCount());
        TbArray<DirectoryInfo>::Iterator dirIte = m_DirectoryInfo.Begin();
        TbArray<DirectoryInfo>::Iterator dirEndIte = m_DirectoryInfo.End();
        for( ; dirIte != dirEndIte; ++dirIte )
        {
            TbArchiveDirectoryInfo* result = dirs++;
            TbMemClear(*result);
            dirDataPtr.PushBack(result);
            buffer.AddStringPtr(dirIte->path.GetBuffer(),&result->path);
            result->hash = dirIte->hash;
            result->nameIndex = TbFileUtil::GetDirectoryNameIndex(dirIte->path);
            getSubdirCountInDir(&result->subdirCount,&result->subdirAllCount,result->hash);
            getFileCountInDir(&result->fileCount,&result->fileAllCount,result->hash);
        }

        // サブディレクトリとファイル情報配列
        dirIte = m_DirectoryInfo.Begin();
        TbArray<TbArchiveDirectoryInfo*>::Iterator targetIte = dirDataPtr.Begin();
        for( ; dirIte != dirEndIte; ++dirIte , ++targetIte )
        {
            TbArchiveDirectoryInfo* result = *targetIte;
            // サブディレクトリ情報
            if( result->subdirCount > 0 ){
                TbUint32** subdirOffset = reinterpret_cast<TbUint32**>(&result->subdirOffset);
                TbUint32* data = buffer.Add<TbUint32>(result->subdirCount,
                                                      NULL,
                                                      subdirOffset);
                getSubdirIndexInDir(data,result->subdirCount,result->hash);
            }

            // ファイル情報
            if( result->fileCount > 0 ){
                TbUint32** fileOffset = reinterpret_cast<TbUint32**>(&result->fileOffset);
                TbUint32* data = buffer.Add<TbUint32>(result->fileCount,
                                                      NULL,
                                                      fileOffset);
                getFileIndexInDir(data,result->fileCount,result->hash);
            }
        }
    }
    
    // データ部までのオフセットを記録
    buffer.ResolveStringPtr();
    header->offsetData = buffer.GetBufferSize();

    // データ追加
    {
        TbArray<FileInfo>::Iterator fileIte = m_FileInfos.Begin();
        TbArray<FileInfo>::Iterator fileEndIte = m_FileInfos.End();
        TbArray<TbArchiveFileInfo*>::Iterator targetIte = dataPtr.Begin();
        for( ; fileIte != fileEndIte; ++fileIte , ++targetIte )
        {
            TbUint8** target = reinterpret_cast<TbUint8**>(&(*targetIte)->offset);
            TbUint8* data = buffer.Add<TbUint8>(fileIte->size,
                                                reinterpret_cast<TbUint8*>(fileIte->data),
                                                target);
        }
    }
    file.Write(buffer.GetBuffer(),buffer.GetBufferSize());
    return TB_S_OK;
}

/*!
 * ファイル情報追加(完了復帰)
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void TbArchiveExporter::AddFileInfo( const char* mapPath , const char* filePath , TbFilePathId pathId )
{
    TbFile file(filePath,TB_FILE_OPERATE_READ,TB_FILE_SYNC,pathId);
    if( !TB_SUCCEEDED(file.Open()) ){
        return;
    }
    file.Read();
    AddFileInfo(mapPath,file.GetReadBufferSize(),file.GetReadBuffer());
}

/*!
 * ファイル情報追加
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
void TbArchiveExporter::AddFileInfo( const char* path , TbUint32 size , void* data )
{
    FileInfo fileInfo;
    fileInfo.path = path;
    fileInfo.path.Replace('\\','/');
    fileInfo.hash = TbCrc32::GetHash(fileInfo.path.GetBuffer());
    fileInfo.dirHash = m_RootDirHash;
    fileInfo.size = size;
    fileInfo.data = TB_NEW TbUint8[size];
    TbMemCopy(fileInfo.data,data,size);
    
    // ディレクトリ情報
    TbString dir;
    TbFileUtil::GetDirectoryPath( dir , path );
    dir.Replace('\\','/');
    if( !dir.IsEmpty() ){
        // ディレクトリがあればそのハッシュをファイルに設定しておく
        fileInfo.dirHash = TbCrc32::GetHash(dir.GetBuffer(),dir.GetCharCount());
        do {
            DirectoryInfo dirInfo;
            dirInfo.path = dir;
            dirInfo.hash = TbCrc32::GetHash(dir.GetBuffer(),dir.GetCharCount());

            // さらに親ディレクトリを捜す
            TbFileUtil::GetDirectoryPath( dir , dir.GetBuffer() );
            if( dir.IsEmpty() ){
                dirInfo.parentHash = m_RootDirHash; // 親はルート
            }else{
                dirInfo.parentHash = TbCrc32::GetHash(dir.GetBuffer(),dir.GetCharCount());
            }
            TbArray<DirectoryInfo>::Iterator ite = TbFindIf(m_DirectoryInfo,DirectoryInfoFindFunc(&dirInfo));
            if( ite == m_DirectoryInfo.End() ){
                m_DirectoryInfo.PushBack(dirInfo);
            }
        }while( !dir.IsEmpty() );
    }

    m_FileInfos.PushBack(fileInfo);
}

/*!
 * サイズ計算
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbUint32 TbArchiveExporter::calcFileSize() const
{
    const TbSizeT ALIGN = 4;
    TbUint32 result = 0;
    result += TB_ALIGN_ROUND_UP(sizeof(TbArchiveHeader),4);
    {
        result += TB_ALIGN_ROUND_UP(sizeof(TbArchiveFileInfo) * m_FileInfos.GetCount(),ALIGN);
        TbArray<FileInfo>::ConstIterator ite = m_FileInfos.Begin();
        TbArray<FileInfo>::ConstIterator endIte = m_FileInfos.End();
        for( ; ite != endIte; ++ite )
        {
            result += TB_ALIGN_ROUND_UP(ite->size,4);
            result += TB_ALIGN_ROUND_UP(ite->path.GetBufferSize(),4);
        }
    }
    {
        result += TB_ALIGN_ROUND_UP(sizeof(TbArchiveDirectoryInfo) * m_DirectoryInfo.GetCount(),ALIGN);
        TbArray<DirectoryInfo>::ConstIterator ite = m_DirectoryInfo.Begin();
        TbArray<DirectoryInfo>::ConstIterator endIte = m_DirectoryInfo.End();
        for( ; ite != endIte; ++ite )
        {
            TbUint32 subdirCount = 0;
            TbUint32 fileCount = 0;
            getSubdirCountInDir(&subdirCount,NULL,ite->hash);
            getFileCountInDir(&fileCount,NULL,ite->hash);
            result += TB_ALIGN_ROUND_UP(sizeof(TbUint32)*(subdirCount+fileCount),4);
            result += TB_ALIGN_ROUND_UP(ite->path.GetBufferSize(),4);
        }
    }
    return result;
}

/*!
 * ディレクトリ内のサブディレクトリ数取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
void TbArchiveExporter::getSubdirCountInDir( TbUint32* dst , TbUint32* dstAll , TbUint32 dirHash ) const
{
    TbUint32 count = 0;
    TbArray<DirectoryInfo>::ConstIterator ite = m_DirectoryInfo.Begin();
    TbArray<DirectoryInfo>::ConstIterator endIte = m_DirectoryInfo.End();
    for( ; ite != endIte; ++ite )
    {
        if( ite->parentHash == dirHash ){
            if( dstAll ){
                getSubdirCountInDir(NULL,dstAll,ite->hash);
            }
            ++count;
        }
    }
    if( dst ) {
        *dst = count;
    }
    if( dstAll ){
        *dstAll += count;
    }
}

/*!
 * ディレクトリ内のファイル数取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
void TbArchiveExporter::getFileCountInDir( TbUint32* dst , TbUint32* dstAll , TbUint32 dirHash ) const
{
    TbUint32 count = 0;
    {
        TbArray<FileInfo>::ConstIterator ite = m_FileInfos.Begin();
        TbArray<FileInfo>::ConstIterator endIte = m_FileInfos.End();
        for( ; ite != endIte; ++ite )
        {
            if( ite->dirHash == dirHash ){
                ++count;
            }
        }
        if( dst ) {
            *dst = count;
        }
        if( dstAll ){
            *dstAll += count;
        }
    }
    // 子ディレクトリ
    if( dstAll ) {
        TbArray<DirectoryInfo>::ConstIterator ite = m_DirectoryInfo.Begin();
        TbArray<DirectoryInfo>::ConstIterator endIte = m_DirectoryInfo.End();
        for( ; ite != endIte; ++ite )
        {
            if( ite->parentHash == dirHash ){
                getFileCountInDir( NULL , dstAll , ite->hash );
            }
        }
    }
}

/*!
 * ディレクトリ内のサブディレクトリ数取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
void TbArchiveExporter::getSubdirIndexInDir( TbUint32* dst , TbUint32 dstCount , TbUint32 dirHash )
{
    TbUint32 count = m_DirectoryInfo.GetCount();
    TbUint32 dstIndex = 0;
    for (TbUint32 i = 0; i < count; ++i ) {
        if( m_DirectoryInfo[i].parentHash == dirHash )
        {
            TB_ASSERT(dstIndex<dstCount);
            dst[dstIndex] = i;
            ++dstIndex;
        }
    }
}

/*!
 * ディレクトリ内のファイル数取得
 * @author Miyake_Shunsuke
 * @since 2014.07.19
 */
void TbArchiveExporter::getFileIndexInDir( TbUint32* dst , TbUint32 dstCount ,TbUint32 dirHash )
{
    TbUint32 count = m_FileInfos.GetCount();
    TbUint32 dstIndex = 0;
    for (TbUint32 i = 0; i < count; ++i ) {
        if( m_FileInfos[i].dirHash == dirHash )
        {
            TB_ASSERT(dstIndex<dstCount);
            dst[dstIndex] = i;
            ++dstIndex;
        }
    }
}

}
