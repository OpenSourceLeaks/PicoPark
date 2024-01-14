/*!
 * @file
 * @brief 文字列クラステンプレートのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_STRING_H_
#define _INCLUDED_TB_STRING_H_

#include <base/string/tb_string_types.h>
#include <base/string/tb_string_util.h>

namespace toybox
{

/*!
 * @class
 * 文字列クラステンプレート
 */
template <typename CHAR_TYPE>
class TbStringBase
{
public:
    typedef CHAR_TYPE CharType;
public:

    // コンストラクタ
    TbStringBase();
    TbStringBase(const TbStringBase<CHAR_TYPE>& string);
    TbStringBase( const CHAR_TYPE* string );
    // デストラクタ
    ~TbStringBase();

public:

    // 代入演算子
    TbStringBase<CHAR_TYPE>& operator=(const TbStringBase<CHAR_TYPE>& string);
    TbStringBase<CHAR_TYPE>& operator=(const CHAR_TYPE* string);
    
    // 加算演算子
    TbStringBase<CHAR_TYPE> operator+(const TbStringBase<CHAR_TYPE>& string);
    TbStringBase<CHAR_TYPE> operator+(const CHAR_TYPE* string);

    // 加算代入演算子
    TbStringBase<CHAR_TYPE>& operator+=(const TbStringBase<CHAR_TYPE>& string) {
        Append(string.GetBuffer(),string.GetCharCount());
        return (*this);
    }

    // 加算代入演算子
    TbStringBase<CHAR_TYPE>& operator+=(const CHAR_TYPE* string) {
        Append(string);
        return (*this);
    }

    // 比較演算子
    TbBool operator==(const TbStringBase<CHAR_TYPE>& string);
    TbBool operator!=(const TbStringBase<CHAR_TYPE>& string);

    // 配列演算子
    CHAR_TYPE operator[](TbSint32 index);

private:
    
    // フォーマット生成
    static void format( TbStringBase<CHAR_TYPE>& dst , const CHAR_TYPE* format , va_list argList );

public:

    // フォーマット形式で生成
    static TbStringBase<CHAR_TYPE> Format( const CHAR_TYPE* format, ... );
    
public:

    // クリア
    void Clear() {
        releaseData();
    }

    // 空かどうか
    TbBool IsEmpty() const {
        return !m_Data || !m_Data->buffer || m_Data->buffer[0] == 0;
    }

    //! バッファ取得
    const CHAR_TYPE* GetBuffer() const;

    //! 文字数取得
    TbSint32 GetBufferSize() const;

    //! 文字数取得
    TbSint32 GetCharCount() const;

    //! 追加
    TbStringBase<CHAR_TYPE>& Append(const CHAR_TYPE* string){
        TbSizeT count = TbStrCharCount(string);
        return Append( string , count );
    }

    //! 追加
    TbStringBase<CHAR_TYPE>& Append(const CHAR_TYPE* string , TbSizeT count ) {
        if( 0 == count ){
            return *this;
        }
        TbSizeT charCount = GetCharCount();
        TbSizeT newSize = sizeof(CHAR_TYPE)*(charCount+count+1);
        CHAR_TYPE* buf = createBuffer( newSize );
        if( charCount > 0 ){
            TbStrCopy( buf , newSize , GetBuffer() , charCount );
        }
        TbStrCopy( &buf[charCount] , count+1 , string , count );
        buf[charCount+count] = 0;
        releaseData();
        createData();
        m_Data->buffer = buf;
        return *this;
    }

    // 後ろから文字検索
    TbSizeT FindLastOf( const CHAR_TYPE ch ) const {
        TbSizeT count = GetCharCount();
        for( TbSint32 i = count-1; i >= 0; --i ) {
            if( m_Data->buffer[i] == ch ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    // 後ろから文字検索
    TbSizeT FindLastOf( const CHAR_TYPE ch1 , const CHAR_TYPE ch2 ) const {
        TbSizeT count = GetCharCount();
        for( TbSint32 i = count-1; i >= 0; --i ) {
            if( m_Data->buffer[i] == ch1 || m_Data->buffer[i] == ch2 ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    // 文字を置換
    void Replace( const CHAR_TYPE src , const CHAR_TYPE dst )
    {
        TbBool isRebuilded = TB_FALSE;
        TbSizeT count = GetCharCount();
        for( TbSizeT i = 0; i < count; ++i ) {
            if( m_Data->buffer[i] == src ){
                if( !isRebuilded ){
                    CHAR_TYPE* buf = createBuffer( GetBufferSize() );
                    TbStrCopy( buf , count+1 , GetBuffer() );
                    releaseData();
                    createData();
                    m_Data->buffer = buf;
                    isRebuilded = TB_TRUE;
                }
                m_Data->buffer[i] = dst;
            }
        }
    }

private:

    // データ生成
    void createData();

    // データ解放
    void releaseData();

    // バッファ生成
    CHAR_TYPE* createBuffer( TbSint32 size );

private:
    
    struct Data
    {
        Data() : buffer( NULL ) , refCount( 1 ) {}
        CHAR_TYPE* buffer;
        TbSint32 refCount;
    };

private:

    TbStringBase(const CHAR_TYPE* str1,const CHAR_TYPE* str2);

private:

    Data* m_Data;       //!< データ

};

#include "tb_string_inl.h"

typedef TbStringBase<TbChar8>   TbString;
#if TB_CHAR16_IS_WCHAR
typedef TbStringBase<TbChar16>  TbU16String;
#endif

}

#endif
