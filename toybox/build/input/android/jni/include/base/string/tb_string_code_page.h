/*!
 * @file
 * @brief 文字列文字コード関連
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_STRING_CODE_PAGE_H_
#define _INCLUDED_TB_STRING_CODE_PAGE_H_

#include "base/string/tb_string_util.h"

namespace toybox
{

// コードページ
enum TbCodePage
{
    TB_CODE_PAGE_SJIS , // 932
    TB_CODE_PAGE_UTF16 , // UTF16
    TB_CODE_PAGE_MAX 
};

/*!
 * 指定された文字が先行バイトかどうか
 * @author Miyake Shunsuke
 * @since 2011.08.19
 */
inline TbBool TbIsDBCSLeadByte( TbCodePage codePage , TbChar8 ch) {
    if( codePage < 0 && codePage >= TB_CODE_PAGE_MAX ){
        return TB_FALSE;
    }
    TbUint8 uch = static_cast<TbUint8>(ch);
    switch (codePage) {
    case TB_CODE_PAGE_SJIS:
        // Shift_jis
        if( ((uch >= 0x81) && (uch <= 0x9F)) ||
            ((uch >= 0xE0) && (uch <= 0xEF)) )
        {
            return TB_TRUE;
        }
        break;
    case TB_CODE_PAGE_UTF16:
    default:
        TB_ASSERT(!"unsupported");
        break;
    }
    return TB_FALSE;
}

/*!
 * 文字コード別文字数取得(NULL文字は含めない)
 * @author Miyake Shunsuke
 * @since 2011.09.25
 */
inline TbUint32 TbStrCount( TbCodePage codePage , const TbChar8* str )
{
    if( codePage < 0 && codePage >= TB_CODE_PAGE_MAX ){
        return 0xFFFFFFFF;
    }
    TbUint32 result = 0;
    TbUint32 step = 1;
    TbUint32 count = TbStrCharCount(str);
    for( TbUint32 i = 0; i < count; i += step ){
        step = 1;
        if( TbIsDBCSLeadByte( TB_CODE_PAGE_SJIS , str[i] ) ){
            step = 2;
        }
        ++result;
    }
    return result;
}

}

#endif