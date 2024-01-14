/*!
 * @file
 * @brief 文字列関数郡
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_STRING_UTIL_H_
#define _INCLUDED_TB_STRING_UTIL_H_

#include <base/string/tb_string_types.h>

namespace toybox
{

// 文字列コピー
void TbStrCopy(TbChar8* dst , TbSizeT dstCount,const TbChar8* src);

// 文字列コピー
void TbStrCopy(TbChar8* dst , TbSizeT dstCount,const TbChar8* src,TbUint32 srcCount);

// 文字列コピー
void TbStrCopy( TbChar8* dst , TbSizeT dstCount , const TbChar16* src );

// 文字列比較
TbSint32 TbStrCmp(const TbChar8* str1,const TbChar8* str2);

// 文字列比較(文字数指定)
TbSint32 TbStrCmp(const TbChar8* str1,const TbChar8* str2,TbSizeT count);

// 文字列設定
TbSint32 TbStrPrintf( TbChar8* buffer , TbUint32 count , const TbChar8 *format , ... );

// 引数リストから文字列書き込み
TbSint32 TbVStrPrintf( TbChar8 *buffer , TbUint32 count , const TbChar8 *format , va_list argptr );

// 文字列内の文字数取得(終端文字は含まれない)
TbSint32 TbStrCharCount(const TbChar8* str);

// 文字列のバッファサイズ(終端文字は含む)
TbSint32 TbStrBufSize(const TbChar8* str);

#if TB_CHAR16_IS_WCHAR
// 文字列コピー
void TbStrCopy( TbChar16* dst , TbSizeT dstCount , const TbChar8* src );
// 文字列コピー
void TbStrCopy(TbChar16* dst,TbUint32 dstCount,const TbChar16* src);
// ワイド文字列比較
TbSint32 TbStrCmp(const TbChar16* str1,const TbChar16* str2);
// 引数リストから文字列書き込み
TbSint32 TbVStrPrintf( TbChar16* buffer , TbUint32 numberOfElements , const TbChar16* format , va_list argptr );
// ワイド文字列内の文字数取得(終端文字は含まない)
TbSint32 TbStrCharCount(const TbChar16* str);
// ワイド文字列内のバッファサイズ(終端文字を含む)
TbSint32 TbStrBufSize(const TbChar16* str);
#endif

// 文字列分割
TbChar8 TbStrTok( TbChar8* s1 , TbChar8* s2 );

// 文字列検索
TbSint32 TbStrFind( const TbChar8* str , const char* search );

// 文字列検索(逆から)
TbSint32 TbStrReverseFind( const TbChar8* str , const char* search );

}

#include "./tb_string_util_inl.h"

#endif