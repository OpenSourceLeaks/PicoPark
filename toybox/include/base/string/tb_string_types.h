/*!
 * @file
 * @brief 文字列クラステンプレートのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_STRING_TYPES_H_
#define _INCLUDED_TB_STRING_TYPES_H_

namespace toybox
{

#define TB_STRING_NPOS (static_cast<TbSizeT>(-1))

template <typename T>
const T* TbGetEmptyString();

template<>
inline const TbChar8* TbGetEmptyString<TbChar8>() {
    return "";
}

#if TB_CHAR16_IS_WCHAR
template<>
inline const TbChar16* TbGetEmptyString<TbChar16>() {
    return L"";
}
#endif

}

#endif
