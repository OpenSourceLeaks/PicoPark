/*!
 * @file
 * @brief 生ポインタの文字列テンプレートのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_RAW_STRING_H_
#define _INCLUDED_TB_RAW_STRING_H_

#include <base/string/tb_string_types.h>
#include <base/string/tb_string_util.h>

namespace toybox
{

template <typename Char>
class TbRawStringBase
{
public:

    typedef Char CharType;

public:

    // コンストラクタ
    TbRawStringBase()
    {
        Clear();
    }

    // コンストラクタ
    TbRawStringBase( const TbRawStringBase<Char>& string )
    {
        *this = string;
    }

    // コンストラクタ
    TbRawStringBase( const Char* string ) {
        *this = string;
    }

public:

    // 代入演算子
    TbRawStringBase<Char>& operator=(const TbRawStringBase<Char>& string)
    {
        m_Buffer = string.m_Buffer;
        m_Size = string.m_Size;
        m_CharCount = string.m_CharCount;
        return *this;
    }

    // 代入演算子
    TbRawStringBase<Char>& operator=(const Char* string)
    {
        m_Buffer = string;
        m_Size = TbStrBufSize(m_Buffer);
        m_CharCount = TbStrCharCount(m_Buffer);
        return *this;
    }

    // 配列演算子
    const Char& operator[](TbSint32 index) const {
        return m_Buffer[index];
    }
    
    // 比較演算子
    TbBool operator==(const Char* string) const {
        return 0 == TbStrCmp(m_Buffer,string);
    }

    // 比較演算子
    TbBool operator!=(const Char* string) const {
        return 0 != TbStrCmp(m_Buffer,string);
    }

public:

    // クリア
    void Clear() {
        m_Size = 0;
        m_CharCount = 0;
        m_Buffer = NULL;
    }

    //! バッファ取得
    const Char* GetBuffer() const{ 
        return m_Buffer; 
    }

    //! バッファサイズ取得
    TbSizeT GetBufferSize() const{ 
        return m_Size;
    }

    //! 文字数取得
    TbSizeT GetCharCount() const{ 
        return m_CharCount; 
    }
    
public:
    
    // 後ろから文字検索
    TbSizeT Find( const Char ch ) {
        TbSizeT count = GetCharCount();
        for( TbSizeT i = 0; i < count; ++i ) {
            if( m_Buffer[i] == ch ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    // 後ろから文字検索
    TbSizeT FindLastOf( const Char ch ) {
        TbSint32 count = static_cast<TbSint32>(GetCharCount());
        for( TbSint32 i = count-1; i >= 0; --i ) {
            if( m_Buffer[i] == ch ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

    // 後ろから文字検索
    TbSizeT FindLastOf( const Char ch1 , const Char ch2 ) {
        TbSint32 count = static_cast<TbSint32>(GetCharCount());
        for( TbSint32 i = count-1; i >= 0; --i ) {
            if( m_Buffer[i] == ch1 || m_Buffer[i] == ch2 ){
                return i;
            }
        }
        return TB_STRING_NPOS;
    }

private:

    TbSizeT     m_Size; // サイズ
    TbSizeT     m_CharCount; // 文字数
    const Char* m_Buffer; //!< バッファ

};

typedef TbRawStringBase<TbChar8>   TbRawString;
//typedef TbRawStringBase<TbChar16>  TbUnmanagedU16String;

}

#endif
