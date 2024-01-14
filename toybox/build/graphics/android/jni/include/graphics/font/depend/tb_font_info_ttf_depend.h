/*!
 * True Type Font 情報クラス分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2011.08.20
 */

#ifndef _INCLUDED_TB_FONT_INFO_TTF_DEPEND_H_
#define _INCLUDED_TB_FONT_INFO_TTF_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
    #include "./win32/tb_font_info_ttf_win32.h"
#else
    typedef TbUint32 TbFontDepend;
#endif

#endif
