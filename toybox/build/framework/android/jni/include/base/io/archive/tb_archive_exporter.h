/*! 
 * アーカイブExporter
 * @author Miyake_Shunsuke
 * @since 2014.6.10
 */
 
#ifndef _INCLUDED_TB_ARCHIVE_EXPORTER_H_
#define _INCLUDED_TB_ARCHIVE_EXPORTER_H_

#include <base/io/archive/tb_archive.h>
#include <base/io/tb_file.h>
#include <base/container/tb_array.h>
#include <base/string/tb_string.h>
#include <base/crypt/tb_crc32.h>

namespace toybox
{

class TbArchiveExporter
{
public:
    
    // コンストラクタ
    TbArchiveExporter() {
        m_FileInfos.SetEnableSetCapacityAuto(TB_TRUE);
        m_DirectoryInfo.SetEnableSetCapacityAuto(TB_TRUE);
        m_RootDirHash = TbCrc32::GetHash(".");

        // ルートディレクトリ
        DirectoryInfo info;
        info.hash = m_RootDirHash;
        info.parentHash = TbCrc32::INVALID;
        info.path = ".";
        m_DirectoryInfo.PushBack(info);
    }
    
    // デストラクタ
    ~TbArchiveExporter() {
        TbArray<FileInfo>::Iterator ite = m_FileInfos.Begin();
        TbArray<FileInfo>::Iterator endIte = m_FileInfos.End();
        // ファイル情報追加
        for( ; ite != endIte; ++ite )
        {
            TbUint8* buf = reinterpret_cast<TbUint8*>(ite->data);
            TB_SAFE_DELETE_ARRAY(buf);
            ite->data = nullptr;
        }
    }
    
public:
    
    // 出力
    TbResult Export( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );
    
public:

    // ファイル情報追加
    void AddFileInfo( const char* mapPath , const char* filePath , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル情報追加
    void AddFileInfo( const char* path , TbUint32 size , void* data );

private:

    struct FileInfo
    {
        TbBool operator<( const FileInfo& info ) {
            return hash < info.hash;
        }
        TbBool operator>( const FileInfo& info ) {
            return hash > info.hash;
        }
        TbString path;
        TbUint32 hash;
        TbUint32 dirHash;
        TbUint32 size;
        void*    data;
    };

    struct DirectoryInfo
    {
        TbBool operator<( const DirectoryInfo& info ) {
            return hash < info.hash;
        }
        TbBool operator>( const DirectoryInfo& info ) {
            return hash > info.hash;
        }
        TbString path;
        TbUint32 hash;
        TbUint32 parentHash;
    };

    struct DirectoryInfoFindFunc
    {
        DirectoryInfoFindFunc( DirectoryInfo* in )
        {
            src = in;
        }
        TbBool operator()( DirectoryInfo& target ) const {
            return src->hash == target.hash && src->path == target.path;
        }
        DirectoryInfo* src;
    };

private:

    // サイズ計算
    TbUint32 calcFileSize() const;

    // ディレクトリ内のサブディレクトリ数取得
    void getSubdirCountInDir( TbUint32* dst , TbUint32* dstAll , TbUint32 dirHash ) const;

    // ディレクトリ内のファイル数取得
    void getFileCountInDir( TbUint32* dst , TbUint32* dstAll , TbUint32 dirHash ) const;
    
    // ディレクトリ内のサブディレクトリのインデックス取得
    void getSubdirIndexInDir( TbUint32* dst , TbUint32 dstCount , TbUint32 dirHash );

    // ディレクトリ内のファイルのインデックス取得
    void getFileIndexInDir( TbUint32* dst , TbUint32 dstCount ,TbUint32 dirHash );

private:

    TbArray<FileInfo>       m_FileInfos;
    TbArray<DirectoryInfo>  m_DirectoryInfo;
    TbUint32                m_RootDirHash;

};

}

#endif
