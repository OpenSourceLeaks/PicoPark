/*!
 * ファイル系ユーティリティクラス
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
 
#ifndef _INCLUDED_TB_FILE_UTIL_H_
#define _INCLUDED_TB_FILE_UTIL_H_

#include <base/io/tb_file_types.h>
#include <base/io/tb_file_manager.h>
#include <base/io/tb_file_util_private.h>
#include <base/io/tb_io_types.h>
#include <base/io/tb_path.h>
#include <base/string/tb_static_string.h>
#include <base/string/tb_raw_string.h>
#include <base/util/tb_function.h>
#include <base/container/tb_array.h>

namespace toybox
{

class TbFileUtil
{
public:

    typedef TbArray<TbStaticPath512> PathList;

public:

        // ディレクトリ名取得
    template <typename T>
    static void GetDirectoryName( T& str , const char* path ) {
        inner::TbFileUtilPrivate::GetDirectoryName(str,path);
    }

    // ディレクトリ名取得
    template <typename T>
    static void GetDirectoryPath( T& str , const char* path ) {
        inner::TbFileUtilPrivate::GetDirectoryPath(str,path);
    }

    // ベース名取得
    template <typename T>
    static void GetBaseName( T& str , const char* path ) {
        inner::TbFileUtilPrivate::GetBaseName(str,path);
    }

    // ファイル名取得
    template <typename T>
    static void GetFileName( T& str , const char* path ) {
        inner::TbFileUtilPrivate::GetFileName(str,path);
    }

    // 拡張子取得
    template <typename T>
    static TbBool GetExtension( T& str , const char* path ) {
        return inner::TbFileUtilPrivate::GetExtension(str,path);
    }

    // パス取得
    static const TbStaticString512 GetFullPath( const char* path , TbFilePathId pathId ) {
        return inner::TbFileUtilPrivate::GetFullPath(path,pathId);
    }

    // ファイル名位置取得
    template<typename String>
    static TbSizeT GetFileNameIndex( const String& str ) {
        TbSizeT result = 0;
        TbSizeT pos = str.FindLastOf('\\','/');
        if (pos != TB_STRING_NPOS ){
            result = pos+1;
        }
        return result;
    }

    // ディレクトリ名位置取得
    template<typename String>
    static TbSizeT GetDirectoryNameIndex( const String& str ) {
        return GetFileNameIndex(str);
    }

public:

    // ファイル一覧取得
    static void GetFiles( PathList& result , const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT , TbUint32 searchFlag = 0 ) {
        TbFileManager::GetInstance().GetFiles( result , path , searchFlag , pathId );
    }

    // ディレクトリ一覧取得
    static void GetDirectories( PathList& result ,const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT , TbUint32 searchFlag = 0 ) {
        TbFileManager::GetInstance().GetDirectories( result , path , searchFlag , pathId );
    }

    // ファイル、ディレクトリ一覧取得
    static void GetFileSystemEntries( PathList& result , const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT , TbUint32 searchFlag = 0 ) {
        TbFileManager::GetInstance().GetFileSystemEntries( result , path , searchFlag , pathId );
    }

public:

    // ディレクトリ作成
    static TbResult CreateDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT , TbBool canCreateAll = TB_FALSE ) {
        if( canCreateAll ){
            // 全て作る場合は親フォルダも作る
            if( !ExistsDirectory(path,pathId) ){
                TbStaticString512 parent;
                GetDirectoryPath(parent,path);
                CreateDirectory(parent.GetBuffer(),pathId,canCreateAll);
                return TbFileManager::GetInstance().CreateDirectory( path , pathId );
            }else{
                return TB_S_OK;
            }
        }
        return TbFileManager::GetInstance().CreateDirectory( path , pathId );
    }

    // ディレクトリ削除
    static TbResult DeleteDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) {
        return TbFileManager::GetInstance().DeleteDirectory( path , pathId );
    }

    // ファイルサイズ取得
    static TbUint32 GetFileSize( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) {
        return TbFileManager::GetInstance().GetFileSize( path , pathId );
    }

    // ファイル存在するか
    static TbBool ExistsFile( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) {
        return TbFileManager::GetInstance().ExistsFile( path , pathId );
    }

    // ディレクトリは存在するか
    static TbBool ExistsDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) {
        return TbFileManager::GetInstance().ExistsDirectory( path , pathId );
    }

private:
    TbFileUtil();
    ~TbFileUtil();
};

}

#endif
