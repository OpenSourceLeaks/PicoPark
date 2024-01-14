/*!
 * パス管理クラス
 * @author Miyake_Shunsuke
 * @since 2013.11.12
 */

#ifndef _INCLUDED_TB_PATH_H_
#define _INCLUDED_TB_PATH_H_

#include <base/io/tb_file_types.h>
#include <base/io/tb_file_util_private.h>
#include <base/string/tb_static_string.h>

namespace toybox
{

template <typename String>
class TbPathT
{
public:

    typedef String StringType;

public:
    
    // コンストラクタ
    TbPathT() : m_PathId(TB_FILE_PATH_ID_NONE) {}

    // コンストラクタ
    TbPathT( const String& directory , const String& fileName ) 
        : m_PathId(TB_FILE_PATH_ID_NONE)
    {
        Set( directory , fileName );
    }

    // コンストラクタ
    TbPathT( const String& directory , const String& fileName , TbFilePathId pathId )
        : m_PathId(TB_FILE_PATH_ID_NONE)
    {
        Set( directory , fileName , pathId );
    }

    // コンストラクタ
    TbPathT( const String& path )
        : m_PathId(TB_FILE_PATH_ID_NONE)
    {
        Set( path );
    }

    // コンストラクタ
    TbPathT( const String& path , TbFilePathId pathId )
        : m_PathId(TB_FILE_PATH_ID_NONE)
    {
        Set( path , pathId );
    }

    // デストラクタ
    ~TbPathT() {}

public:

    // パス設定
    TbPathT<String>& operator=( const String& path ) {
        m_Storage = path;
    }

    // パス取得
    void Set( const String& directory , const String& fileName ) {
        m_Storage = directory;
        typename String::CharType back = directory.GetBack();
        if( back != TbGetChar<typename String::CharType>(TB_CHAR_TABLE_YEN) && 
            back != TbGetChar<typename String::CharType>(TB_CHAR_TABLE_SLASH) ) 
        {
            m_Storage += TbGetChar<typename String::CharType>(TB_CHAR_TABLE_SLASH);
        }
        m_Storage += fileName;
    }

    // パス取得
    void Set( const String& path ) {
        m_Storage = path;
    }

    // パス取得
    void Set( const String& directory , const String& fileName , TbFilePathId pathId ) {
        Set(directory,fileName);
        SetRootPathId(pathId);
    }

    // パス取得
    void Set( const String& path , TbFilePathId pathId ) {
        Set(path);
        SetRootPathId(pathId);
    }

public:

    // パスを取得
    const typename String::CharType* GetBuffer() const {
        return m_Storage.GetBuffer();
    }

    // ローカルパス取得
    const String& GetLocalPath() const {
        return m_Storage;
    }

    // 全体パス取得
    String GetFullPath() const {
        const TbStaticString512& src = inner::TbFileUtilPrivate::GetFullPath(m_Storage.GetBuffer(),m_PathId);
        return src.GetBuffer();
    }

    // フォルダパス取得
    String GetDirectoryPath() const {
        String result;
        inner::TbFileUtilPrivate::GetDirectoryPath(result,m_Storage.GetBuffer());
        return result;
    }

    // ベース名取得
    String GetBaseName() const {
        String result;
//        TbFileUtilPrivate::GetBaseName(result,m_Storage.GetBuffer());
        return result;
    }
    
    // ファイル名取得
    String GetFileName() const {
        String result;
        inner::TbFileUtilPrivate::GetFileName(result,m_Storage.GetBuffer());
        return result;
    }

    // ファイル名取得
    String GetExtension() const {
        String result;
        inner::TbFileUtilPrivate::GetExtension(result,m_Storage.GetBuffer());
        return result;
    }

public:

    // 空か
    TbBool IsEmpty() const {
        return m_PathId == TB_FILE_PATH_ID_NONE && m_Storage.IsEmpty();
    }

    // ファイル名を持っている
    TbBool HasFileName() const {
        return TB_STRING_NPOS != m_Storage.FindLastOf(TbGetChar<typename String::CharType>(TB_CHAR_TABLE_YEN),
                                                      TbGetChar<typename String::CharType>(TB_CHAR_TABLE_SLASH));
    }

    // 絶対パスか
    TbBool IsAbsolute();

    // 相対パスか
    TbBool IsRelative();

public:

    // 拡張子設定
    void SetExtension( const char* ext ) {
        TbSizeT pos = m_Storage.FindLastOf(TbGetChar<typename String::CharType>(TB_CHAR_TABLE_PERIOD));
        if( pos == TB_STRING_NPOS ) {
            m_Storage = m_Storage + "." + ext;
        }else{
            m_Storage = m_Storage.Substr(0,pos) + "." + ext;
        }
    }

    // 拡張子が一致しているか
    TbBool EqualsExtension( const char* ext ) {
        TbSizeT pos = m_Storage.FindLastOf(TbGetChar<typename String::CharType>(TB_CHAR_TABLE_PERIOD));
        if( pos != TB_STRING_NPOS ) {
            return 0 == TbStrCmp( &m_Storage[pos+1] , ext );
        }
        return TB_FALSE;
    }

    // ファイル名除去
    void RemoveFileName();

public:

    // ルートパスID設定
    void SetRootPathId( TbFilePathId pathId ) {
        m_PathId = pathId;
    }

    // ルートパスID取得
    TbFilePathId GetRootPathId() const {
        return m_PathId;
    }
public:

    TbFilePathId    m_PathId;
    String          m_Storage;

};

typedef TbPathT<TbString> TbPath;
typedef TbPathT<TbStaticString32>  TbStaticPath32;
typedef TbPathT<TbStaticString64>  TbStaticPath62;
typedef TbPathT<TbStaticString128> TbStaticPath128;
typedef TbPathT<TbStaticString256> TbStaticPath256;
typedef TbPathT<TbStaticString512> TbStaticPath512;

}

#endif
