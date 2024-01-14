/*!
 * 文字列変換クラス
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */

#ifndef _INCLUDED_TB_STRING_CONVERTER_H_
#define _INCLUDED_TB_STRING_CONVERTER_H_

#include <base/string/tb_string.h>

namespace toybox
{

class TbStringConverter
{
private: // インスタンス生成不可
    TbStringConverter();
    ~TbStringConverter();
public:
    // Sint32型変換
    static TbSint32 ParseSint32( const char* str );
    // Uint32型変換
    static TbUint32 ParseUint32( const char* str );
    // Float32型変換
    static TbFloat32 ParseFloat32( const char* str );
    // Float64型変換
    static TbFloat64 ParseFloat64( const char* str );
    // Sint32型変換(パース結果取得可能型)
    static TbBool   TryParseSint32( const char* str , TbSint32& dst );
    // Uint32型変換(パース結果取得可能型)
    static TbBool   TryParseUint32( const char* str , TbUint32& dst );
    // Float32型変換(パース結果取得可能型)
    static TbBool   TryParseFloat32( const char* str , TbFloat32& dst );
    // Float64型変換(パース結果取得可能型)
    static TbBool   TryParseFloat64( const char* str , TbFloat64& dst );

public:

    // SJISをUTF8の文字列に変換
    static TbString SJIStoUTF8( const char* str );

#if TB_CHAR16_IS_WCHAR
    // SJISをUTF16の文字列に変換
    static TbU16String SJIStoUTF16( const char* str );
#endif

    // UTF16をUTF8の文字列に変換
    static TbString UTF16toUTF8( const TbChar16* str );

private:
    // 数値かどうか調べる
    static TbBool isNumber( const char* str );
    // 整数かどうか調べる
    static TbBool isInteger( const char* str );
};

}

#endif