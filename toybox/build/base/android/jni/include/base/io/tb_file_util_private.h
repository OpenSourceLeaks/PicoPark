/*!
 * ファイルパス系ユーティリティ
 * アプリケーション側では直接インクルードしないでください
 * @author Miyake Shunsuke
 * @since 2013.07.21
 */
 
#ifndef _INCLUDED_TB_PATH_UTIL_RRIVATE_H_
#define _INCLUDED_TB_PATH_UTIL_RRIVATE_H_

#include <base/io/tb_file_types.h>
#include <base/io/tb_io_types.h>
#include <base/string/tb_static_string.h>
#include <base/string/tb_raw_string.h>

namespace toybox
{

namespace inner
{

class TbFileUtilPrivate
{
public:

    // パス取得
    static const TbStaticString512 GetFullPath( const char* path , TbFilePathId pathId );

    // ディレクトリ名取得
    template <typename T>
    static void GetDirectoryName( T& str , const char* path ) {
        GetFileName( str , path );
    }

    // ディレクトリ名取得
    template <typename T>
    static void GetDirectoryPath( T& str , const char* path ) {
        if( !path || 0 == TbStrCmp( path , "" ) ){
            str.Clear();
            return;
        }
        TbRawString pathStr(path);
        TbSizeT count = 0;
        TbSizeT pos = pathStr.FindLastOf('\\','/');
        if( pos != TB_STRING_NPOS ) {
            count = pos;
        }
        T tmp = path;
        str.Clear();
        str.Append(tmp.GetBuffer(),count);
    }

    // ベース名取得
    template <typename T>
    static void GetBaseName( T& str , const char* path ) {
        GetFileName(str,path);
        TbSizeT pos = str.FindLastOf('.');
        if( pos != TB_STRING_NPOS ) {
            str = str.Substr(0,pos);
        }
    }

    // ファイル名取得
    template <typename T>
    static void GetFileName( T& str , const char* path ) {
        TbRawString pathStr(path);
        TbSizeT index = 0;
        TbSizeT pos = pathStr.FindLastOf('\\','/');
        if( pos != TB_STRING_NPOS ) {
            index = pos+1;
        }
        str.Clear();
        str.Append(&path[index]);
    }

    // 拡張子取得
    template <typename T>
    static TbBool GetExtension( T& str , const char* path ) {
        str.Clear();
        TbRawString pathStr(path);
        TbSizeT pos = pathStr.FindLastOf('.');
        if( pos != TB_STRING_NPOS ) {
            str.Append( &pathStr[ pos+1] );
            return TB_TRUE;
        }
        return TB_FALSE;
    }

private:
    TbFileUtilPrivate();
    ~TbFileUtilPrivate();
};

}

}

#endif
