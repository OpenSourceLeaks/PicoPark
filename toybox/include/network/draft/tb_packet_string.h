/*!
 * @file
 * @brief  パケット用固定サイズ文字列
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_PACKET_STRING_H_
#define _INCLUDED_TB_PACKET_STRING_H_

namespace toybox
{

/*!
 * 文字列クラステンプレート
 */
template <typename CharType,TbUint32 SIZE>
class TbPacketStringBase
{
public:

    // コンストラクタ
    TbPacketStringBase();
    TbPacketStringBase( const TbPacketStringBase<CharType,SIZE>& string );
    TbPacketStringBase( const CharType* string );

public:

    // 代入演算子
    TbPacketStringBase<CharType,SIZE>& operator=(const TbPacketStringBase<CharType,SIZE>& string);
    TbPacketStringBase<CharType,SIZE>& operator=(const CharType* string);

    // 加算代入演算子
    TbPacketStringBase<CharType,SIZE>& operator+=(const TbPacketStringBase<CharType,SIZE>& string);
    TbPacketStringBase<CharType,SIZE>& operator+=(const CharType* string);

    // 加算演算子
    TbPacketStringBase<CharType,SIZE> operator+(const TbPacketStringBase<CharType,SIZE>& string) const;
    TbPacketStringBase<CharType,SIZE> operator+(const CharType* string) const;

    // 比較演算子
    TbBool operator==(const TbPacketStringBase<CharType,SIZE>& string) const;
    TbBool operator!=(const TbPacketStringBase<CharType,SIZE>& string) const;

    // 配列演算子
    TbChar8 operator[](TbSint32 index) const;

private:
    
    // フォーマット生成
    static void format( TbPacketStringBase<CharType,SIZE>& dst , const char* format , va_list argList );

    
public:

    // フォーマット形式で生成
    static TbPacketStringBase<CharType,SIZE> Format( const char* format, ... );

public:

    // クリア
    void Clear();

    // フォーマットで値設定
    void SetFormatedString(const char* format, ...);

    //! バッファ取得
    CharType* GetBuffer() { return m_Buffer; }
    const CharType* GetBuffer() const{ return m_Buffer; }

    //! バッファサイズ取得
    TbSint32 GetBufferSize() const{ return SIZE; }

    //! 文字数取得
    TbSint32 GetCharCount() const{ return TbStrCharCount(m_Buffer); }

    //! 追加
    TbPacketStringBase<CharType,SIZE>& Append(const CharType* string){ return ((*this) += string); }

private:

    CharType m_Buffer[SIZE]; //!< バッファ

};

#include "tb_packet_string_inl.h"

typedef TbPacketStringBase<TbChar8,8>   TbStaticString8;
typedef TbPacketStringBase<TbChar8,16>  TbStaticString16;
typedef TbPacketStringBase<TbChar8,32>  TbStaticString32;
typedef TbPacketStringBase<TbChar8,64>  TbStaticString64;
typedef TbPacketStringBase<TbChar8,128> TbStaticString128;
typedef TbPacketStringBase<TbChar8,256> TbStaticString256;
typedef TbPacketStringBase<TbChar8,512> TbStaticString512;

}
#endif
