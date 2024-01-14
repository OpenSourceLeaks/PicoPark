/*!
 * 文字列変換クラス
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */

#include "tb_fwd.h"
#include "base/string/tb_string_converter.h"

namespace toybox
{

/*!
 * Sint32型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbSint32 TbStringConverter::ParseSint32( const char* str )
{
    if( isInteger( str ) ){
        return static_cast<TbSint32>(atoi( str ));
    }
    return 0;
}

/*!
 * Uint32型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbUint32 TbStringConverter::ParseUint32( const char* str )
{
    if( isInteger( str ) ){
        return static_cast<TbUint32>(atoi( str ));
    }
    return 0U;
}

/*!
 * Float32型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbFloat32 TbStringConverter::ParseFloat32( const char* str )
{
    if( isNumber( str ) ){
        return static_cast<TbFloat32>(atof( str ));
    }
    return 0.0f;
}

/*!
 * Float64型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbFloat64 TbStringConverter::ParseFloat64( const char* str )
{
    if( isNumber( str ) ){
        return atof( str );
    }
    return 0.0;
}

/*!
 * Sint32型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool   TbStringConverter::TryParseSint32( const char* str , TbSint32& dst )
{
    if( isInteger( str ) ){
        dst = static_cast<TbSint32>(atoi( str ));
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * Uint32型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool   TbStringConverter::TryParseUint32( const char* str , TbUint32& dst )
{
    if( isInteger( str ) ){
        dst = static_cast<TbUint32>(atoi( str ));
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * Float32型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool   TbStringConverter::TryParseFloat32( const char* str , TbFloat32& dst )
{
    if( isNumber( str ) ){
        dst = static_cast<TbFloat32>(atof( str ));
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * Float64型変換
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool   TbStringConverter::TryParseFloat64( const char* str , TbFloat64& dst )
{
    if( isNumber( str ) ){
        dst = atof( str );
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 数値か
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool   TbStringConverter::isNumber( const char* str )
{
    TbBool result = TB_FALSE;
    TbBool decimalFirstFlag = TB_FALSE;
#if TB_IS_DEBUG
    TbSint32 limit = 5000;
#endif
    const char* p = str;
    while( *p != '\0' ){
#if TB_IS_DEBUG
        if( --limit == 0 ) {
            result = TB_FALSE; 
            break; 
        }
#endif
        if( '0' <= *p && *p <= '9' ){
            result = TB_TRUE;
        }else{
            if( *p != '.' || decimalFirstFlag ){
                result = TB_FALSE;
                break;
            }
            decimalFirstFlag = TB_TRUE;
        }
        ++p;
    }
    return result;
}

/*!
 * 整数値
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool   TbStringConverter::isInteger( const char* str )
{
    TbBool result = TB_FALSE;
#if TB_IS_DEBUG
    TbSint32 limit = 5000;
#endif
    const char* p = str;
    while( *p != '\0' ){
#if TB_IS_DEBUG
        if( --limit == 0 ) {
            result = TB_FALSE; 
            break; 
        }
#endif
        if( '0' <= *p && *p <= '9' ){
            result = TB_TRUE;
        }else{
            result = TB_FALSE;
            break;
        }
        ++p;
    }
    return result;
}

/*!
 * SJISをUTF8の文字列に変換
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
TbString TbStringConverter::SJIStoUTF8( const char* str )
{
    TbString result;
    return result;
}

#if TB_CHAR16_IS_WCHAR
/*!
 * SJISをUTF16の文字列に変換
 * @author teco
 */
TbU16String TbStringConverter::SJIStoUTF16( const char* str )
{
    TbU16String result;
#if TB_PLATFORM_IS_WIN
    wchar_t bufUnicode[512];
    
    // まずUniocdeに変換する
    // サイズを計算する
    TbSint32 len = ::MultiByteToWideChar(CP_ACP, 0, str , TbStrCharCount(str)+1, NULL, 0);
    if (len <= TB_ARRAY_COUNT_OF(bufUnicode))
    {
        ::MultiByteToWideChar(CP_ACP, 0, str , strlen(str)+1, bufUnicode, MAX_PATH);
        result = bufUnicode;
    }
#else
    TB_ASSERT(0);
#endif
    return result;
}
#endif

/*!
 * UTF16をUTF8の文字列に変換
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
TbString TbStringConverter::UTF16toUTF8( const TbChar16* str )
{
    TbString result;
    return result;
}

}
