/*!
 * 文字エンコーディングクラス
 * @author Miyake Shunsuke
 * @since @date
 */

#include "tb_fwd.h"
#include "base/string/encoding/tb_string_encoding.h"

namespace toybox
{

namespace
{

/*!
 * 数値か
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */
TbBool isNumber( const TbChar8* str )
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
TbBool   isInteger( const TbChar8* str )
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

}

/*!
 * ASCII文字列 -> Sin32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbSint32 TbStringEncoding::ConvertASCIIToSint32( const TbChar8* string )
{
    if( isInteger( string ) ){
        return static_cast<TbSint32>(atoi( string ));
    }
    return 0;
}

/*!
 * ASCII文字列 -> Uint32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbUint32 TbStringEncoding::ConvertASCIIToUint32( const TbChar8* string )
{
    if( isInteger( string ) ){
        return static_cast<TbUint32>(atoi( string ));
    }
    return 0U;
}

/*!
 * ASCII文字列 -> Float32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbFloat32 TbStringEncoding::ConvertASCIIToFloat32( const TbChar8* string )
{
    if( isNumber( string ) ){
        return static_cast<TbFloat32>(atof( string ));
    }
    return 0.0;
}

/*!
 * ASCII文字列 -> Bool
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncoding::ConvertASCIIToBool( const TbChar8* string )
{
    if( TbStrCmp(string,"true") ){
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ASCII文字列 -> Sin32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncoding::TryConvertASCIIToSint32( const TbChar8* string , TbSint32& result )
{
    if( isInteger( string ) ){
        result = static_cast<TbSint32>(atoi( string ));
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ASCII文字列 -> Uint32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncoding::TryConvertASCIIToUint32( const TbChar8* string , TbUint32& result )
{
    if( isInteger( string ) ){
        result = static_cast<TbUint32>(atoi( string ));
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ASCII文字列 -> Float32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncoding::TryConvertASCIIToFloat32( const TbChar8* string , TbFloat32& result )
{
    if( isNumber( string ) ){
        result = static_cast<TbFloat32>(atof( string ));
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ASCII文字列 -> Bool
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncoding::TryConvertASCIIToBool( const TbChar8* string , TbBool& result )
{
    if( TbStrCmp(string,"true") ){
        result = TB_TRUE;
        return TB_TRUE;
    }else if( TbStrCmp(string,"false") ){
        result = TB_FALSE;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * Sin32 -> ASCII文字列
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbResult TbStringEncoding::ConvertSint32ToASCII( TbStaticString64& string , TbSint32 src )
{
    return TB_E_FAIL;
}

/*!
 * Uint32 -> ASCII文字列
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbResult TbStringEncoding::ConvertUint32ToASCII( TbStaticString64& string , TbUint32 src )
{
    return TB_E_FAIL;
}

/*!
 * Float32 -> ASCII文字列
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbResult TbStringEncoding::ConvertFloat32ToASCII( TbStaticString64& string , TbFloat32 src )
{
    return TB_E_FAIL;
}

/*!
 * Bool -> ASCII文字列
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbResult TbStringEncoding::ConvertBoolToASCII( TbStaticString8& string , TbBool src )
{
    return TB_E_FAIL;
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbStringEncoding::TbStringEncoding() 
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbStringEncoding::~TbStringEncoding() {}
    
}
