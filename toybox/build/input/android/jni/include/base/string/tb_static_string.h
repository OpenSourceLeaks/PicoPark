/*!
 * @file
 * @brief 固定文字列基本クラステンプレートのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_STATIC_STRING_H_
#define _INCLUDED_TB_STATIC_STRING_H_

#include <base/string/tb_string_types.h>
#include <base/string/tb_string_util.h>

namespace toybox
{

/*!
 * @class
 * 文字列クラステンプレート
 */
template <typename Char,TbUint32 SIZE>
class TbStaticStringBase
{
public:

    static const TbStaticStringBase<Char,SIZE>& EMPTY() {
        static TbStaticStringBase<Char,SIZE> result;
        return result;
    };

    typedef Char CharType;

public:

    // コンストラクタ
    TbStaticStringBase();

    // コンストラクタ
    TbStaticStringBase( const TbStaticStringBase<Char,SIZE>& string );

    // コンストラクタ
    TbStaticStringBase( const Char* string );

public:

    // 代入演算子
    TbStaticStringBase<Char,SIZE>& operator=(const TbStaticStringBase<Char,SIZE>& string) {
        TbStrCopy( m_Buffer , SIZE , string.GetBuffer() );
        return (*this);
    }

    // 代入演算子
    template<typename T>
    TbStaticStringBase<Char,SIZE>& operator=(const T* string) {
        if( string ){
            TbStrCopy( m_Buffer , SIZE , string );
        }else{
            TbStrCopy( m_Buffer , SIZE , "" );
        }
        return (*this);
    }

    // 加算代入演算子
    TbStaticStringBase<Char,SIZE>& operator+=(const TbStaticStringBase<Char,SIZE>& string) {
        Append(string.GetBuffer(),string.GetCharCount());
        return (*this);
    }

    // 加算代入演算子
    TbStaticStringBase<Char,SIZE>& operator+=(const TbChar8* string) {
        Append(string);
        return (*this);
    }

    // 加算代入演算子
    TbStaticStringBase<Char,SIZE>& operator+=(const TbChar16* string) {
        Append(string);
        return (*this);
    }

    // 加算代入演算子
    TbStaticStringBase<Char,SIZE>& operator+=(const Char& ch) {
        TbSizeT count = GetCharCount();
        if( count < SIZE-1 ) {
            m_Buffer[count] = ch;
            m_Buffer[count+1] = '\0';
        }
        return (*this);
    }

    // 加算演算子
    TbStaticStringBase<Char,SIZE> operator+(const TbStaticStringBase<Char,SIZE>& string) const {
        TbStaticStringBase<Char,SIZE> result( *this );
        result.Append(string.GetBuffer(),string.GetCharCount());
        return result;
    }

    // 加算演算子
    template<typename T>
    TbStaticStringBase<Char,SIZE> operator+(const T* string) const {
        TbStaticStringBase<Char,SIZE> result( *this );
        result.Append(string);
        return result;
    }

    // 比較演算子
    bool operator==(const TbStaticStringBase<Char,SIZE>& string) const;
    bool operator!=(const TbStaticStringBase<Char,SIZE>& string) const;
    bool operator<(const TbStaticStringBase<Char,SIZE>& string) const;
    bool operator<=(const TbStaticStringBase<Char,SIZE>& string) const;
    bool operator>(const TbStaticStringBase<Char,SIZE>& string) const;
    bool operator>=(const TbStaticStringBase<Char,SIZE>& string) const;

    // 配列演算子
    const Char& operator[](TbSint32 index) const;
    
    // 配列演算子
    Char& operator[](TbSint32 index);

public:

    // フォーマット形式で生成
    static TbStaticStringBase<Char,SIZE> Format( const char* format, ... );

public:

    // クリア
    void Clear();

    // 空かどうか
    TbBool IsEmpty() const {
        return m_Buffer[0] == 0;
    }

    // フォーマットで値設定
    void SetFormatedString(const char* format, ...);

    //! バッファ取得
    Char* GetBuffer() { 
        return m_Buffer; 
    }

    const Char* GetBuffer() const{ 
        return m_Buffer; 
    }

    //! バッファサイズ取得
    TbSint32 GetBufferSize() const{ 
        return TbStrBufSize(m_Buffer); 
    }

    // 最後尾文字取得
    Char GetBack() const {
        TbUint32 count = GetCharCount();
        return count > 0 ? m_Buffer[count-1] : 0;
    }

    // 先頭文字取得
    Char GetFront() const {
        TbUint32 count = GetCharCount();
        return count > 0 ? m_Buffer[0] : 0;
    }

    //! 文字数取得
    TbSint32 GetCharCount() const{ 
        return TbStrCharCount(m_Buffer); 
    }

    //! 追加
    template<typename T>
    TbStaticStringBase<Char,SIZE>& Append(const T* string){
        TbSizeT count = TbStrCharCount(string);
        return Append( string , count );
    }

    //! 追加
    template<typename T>
    TbStaticStringBase<Char,SIZE>& Append(const T* string , TbSizeT count ) {
        TbSizeT usedCount= GetCharCount();
        TbSizeT rest = SIZE-usedCount-1;
        if( count < rest ) {
            // まだ書き込める
            TbStrCopy( &m_Buffer[usedCount] , rest , string );
            m_Buffer[ usedCount + count ] = 0;
        }
        return *this;
    }

public:

    // 文字検索
    TbSizeT Find( const Char ch ) {
        TbSizeT count = GetCharCount();
        for( TbUint32 i = 0; i < count; ++i ) {
            if( m_Buffer[i] == ch ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    // 後ろから文字検索
    TbSizeT FindLastOf( const Char ch ) {
        TbSint32 count = GetCharCount();
        for( TbSint32 i = count-1; i >= 0; --i ) {
            if( m_Buffer[i] == ch ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    // 後ろから文字検索
    TbSizeT FindLastOf( const Char ch1 , const Char ch2 ) {
        TbSizeT count = GetCharCount();
        for (TbSint32 i = count - 1; i >= 0; --i) {
            if( m_Buffer[i] == ch1 || m_Buffer[i] == ch2 ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    //! 部分文字列を返す
    TbStaticStringBase<Char,SIZE> Substr( TbSizeT start ) {
        TbSizeT count = GetCharCount();
        TbStaticStringBase<Char,SIZE> result;
        if( start < count ) {
            result = &m_Buffer[start];
        }
        return result;
    }

    //! 部分文字列を返す
    TbStaticStringBase<Char,SIZE> Substr( TbSizeT start , TbSizeT count ) {
        TbSizeT usedCount = GetCharCount();
        TbStaticStringBase<Char,SIZE> result;
        if( start < usedCount ) {
            result.Append( &m_Buffer[start] , count );
        }
        return result;
    }

    // 文字を置換
    void Replace( const Char src , const Char dst )
    {
        TbSizeT count = GetCharCount();
        for( TbSizeT i = 0; i < count; ++i ) {
            if( m_Buffer[i] == src ){
                m_Buffer[i] = dst;
            }
        }
    }

private:
    
    // フォーマット
    static void format( TbStaticStringBase<Char,SIZE>& dst , const char* format , va_list argList );

private:

    Char m_Buffer[SIZE]; //!< バッファ

};

#include "tb_static_string_inl.h"

typedef TbStaticStringBase<TbChar8,8>   TbStaticString8;
typedef TbStaticStringBase<TbChar8,16>  TbStaticString16;
typedef TbStaticStringBase<TbChar8,32>  TbStaticString32;
typedef TbStaticStringBase<TbChar8,64>  TbStaticString64;
typedef TbStaticStringBase<TbChar8,128> TbStaticString128;
typedef TbStaticStringBase<TbChar8,256> TbStaticString256;
typedef TbStaticStringBase<TbChar8,512> TbStaticString512;

/*
typedef TbStaticStringBase<TbChar16,8>   TbStaticU16String8;
typedef TbStaticStringBase<TbChar16,16>  TbStaticU16String16;
typedef TbStaticStringBase<TbChar16,32>  TbStaticU16String32;
typedef TbStaticStringBase<TbChar16,64>  TbStaticU16String64;
typedef TbStaticStringBase<TbChar16,128> TbStaticU16String128;
typedef TbStaticStringBase<TbChar16,256> TbStaticU16String256;
typedef TbStaticStringBase<TbChar16,512> TbStaticU16String512;
*/

}

#endif
