/*!
 * 文字エンコーディングクラス(SJIS)
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */

#include <tb_fwd.h>
#include <base/string/encoding/tb_string_encoding_sjis.h>

namespace toybox
{

namespace
{

/*!
 * 指定された文字が先行バイトかどうか
 * @author Miyake Shunsuke
 * @since 2011.08.19
 */
inline TbBool isDBCSLeadByte( TbChar8 ch) {
    TbUint8 uch = static_cast<TbUint8>(ch);
    if( ((uch >= 0x81) && (uch <= 0x9F)) ||
        ((uch >= 0xE0) && (uch <= 0xEF)) )
    {
        return TB_TRUE;
    }
    return TB_FALSE;
}

}
    
/*!
 * index文字目のバイト数を取得
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbUint32 TbStringEncodingSJIS::GetCharByteCount( TbUint32 index , const TbChar8* string ) const
{
    TbUint32 result = 0;
    TbUint32 charCount = 0;
    TbUint32 step = 1;
    TbUint32 count = TbStrCharCount(string);
    for( TbUint32 i = 0; i < count; i += step ){
        step = 1;
        if( isDBCSLeadByte(string[i] ) ){
            step = 2;
        }
        if( index == charCount ){
            result = step;
            break;
        }
        ++charCount;
    }
    return result;
}

/*!
 * バイト数取得
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbUint32 TbStringEncodingSJIS::GetByteCount( const TbChar8* string ) const
{
    return TbStrCharCount(string);
}
    
/*!
 * 文字数取得
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbUint32 TbStringEncodingSJIS::GetCharCount( const TbChar8* string ) const
{
    TbUint32 result = 0;
    TbUint32 step = 1;
    TbUint32 count = TbStrCharCount(string);
    for( TbUint32 i = 0; i < count; i += step ){
        step = 1;
        if( isDBCSLeadByte(string[i] ) ){
            step = 2;
        }
        ++result;
    }
    return result;
}

/*!
 * Sin32 -> 文字列
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbResult TbStringEncodingSJIS::ConvertSint32ToString( TbStaticString64& string , TbSint32 src )const
{
    return TB_E_FAIL;
}

/*!
 * Uint32 -> 文字列
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbResult TbStringEncodingSJIS::ConvertUint32ToString( TbStaticString64& string , TbUint32 src )const
{
    return TB_E_FAIL;
}

/*!
 * Float32 -> 文字列
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbResult TbStringEncodingSJIS::ConvertFloat32ToString( TbStaticString64& string , TbFloat32 src )const
{
    return TB_E_FAIL;
}

/*!
 * Bool -> 文字列
 * @author Miyake Shunsuke
 * @since 2012.03.21
 */
TbResult TbStringEncodingSJIS::ConvertBoolToString( TbStaticString8& string , TbBool src )const
{
    return TB_E_FAIL;
}

/*!
 * 全角文字列 -> Sin32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbSint32 TbStringEncodingSJIS::ConvertStringToSint32( const TbChar8* string )const
{
    return 0;
}
    
/*!
 * 全角文字列 -> Uint32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbUint32 TbStringEncodingSJIS::ConvertStringToUint32( const TbChar8* string )const
{
    return 0;
}

/*!
 * 全角文字列 -> Float32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbFloat32 TbStringEncodingSJIS::ConvertStringToFloat32( const TbChar8* string )const
{
    return 0.0f;
}

/*!
 * 全角文字列 -> Bool
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncodingSJIS::ConvertStringToBool( const TbChar8* string )const
{
    return TB_FALSE;
}

/*!
 * 全角文字列 -> Sin32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncodingSJIS::TryConvertStringToSint32( const TbChar8* string , TbSint32& result )
{
    return TB_FALSE;
}
    
/*!
 * 全角文字列 -> Uint32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncodingSJIS::TryConvertStringToUint32( const TbChar8* string , TbUint32& result )
{
    return TB_FALSE;
}

/*!
 * 全角文字列 -> Float32
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncodingSJIS::TryConvertStringToFloat32( const TbChar8* string , TbFloat32& result )
{
    return TB_FALSE;
}

/*!
 * 全角文字列 -> Bool
 * @author Miyake Shunsuke
 * @since 2012.03.26
 */
TbBool TbStringEncodingSJIS::TryConvertStringToBool( const TbChar8* string , TbBool& result )
{
    return TB_FALSE;
}

}
