/*!
 * @file
 * @brief アーカイブを利用したファイルマッパークラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_MAPPER_ARCHIVE_H_
#define _INCLUDED_TB_FILE_MAPPER_ARCHIVE_H_

#include <base/io/tb_file_mapper.h>

namespace toybox
{

class TbArchive;
struct TbArchiveDirectoryInfo;

class TbFileMapperArchive : public TbFileMapper
{
public:

    // コンストラクタ
    TbFileMapperArchive() : TbFileMapper(TB_FALSE,TB_TRUE) , m_Archive(NULL) {}

public:
    
    // 対応しているかパスIDかどうか
    virtual TbBool IsEnable( TbFilePathId pathId ) const {
        return m_MappingPath.GetRootPathId() == pathId;
    }

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル存在するか
    virtual TbBool ExistsFile( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリは存在するか
    virtual TbBool ExistsDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル実体生成
    virtual TbFileImpl* CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // ディレクトリ作成
    virtual TbResult CreateDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) {
        return TB_E_FAIL;
    }

    // ディレクトリ削除
    virtual TbResult DeleteDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) { 
        return TB_E_FAIL;
    }

    // ファイル一覧取得
    virtual void GetFiles( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリ一覧取得
    virtual void GetDirectories( PathList& result ,const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル、ディレクトリ一覧取得
    virtual void GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

public:

    // セットアップ
    void Setup( const char* path , TbFilePathId pathId , TbArchive* archive , TbSint32 priority )
    {
        SetMappingPath( path , pathId );
        SetArchive( archive );
        SetPriority( priority );
    }

    // マッピング先設定
    void SetMappingPath( const char* path , TbFilePathId pathId )
    {
        m_MappingPath.Set( path , pathId );
    }

    // アーカイブ設定
    void SetArchive( TbArchive* archive ) {
        m_Archive = archive;
    }

private:

    // Archiveに渡すパスを取得
    const char* getLocalPath( const char* path );

    // サブディレクトリを追加
    void addSubdir( PathList& result , const TbArchiveDirectoryInfo* info , TbUint32 searchFlag , const TbStaticString512& basePath );

    // ファイルを追加
    void addFile( PathList& result , const TbArchiveDirectoryInfo* info , TbUint32 searchFlag , const TbStaticString512& basePath );

private:

    TbStaticPath512     m_MappingPath;
    TbArchive*          m_Archive;

};

}

#endif
