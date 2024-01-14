/*!
 * @file
 * @brief キーボードの機種依存コード
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_KEYBORAD_DEPEND_H_
#define _INCLDUED_TB_KEYBORAD_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
    #include "./win32/tb_keyboard_win32.h"
#elif (TB_PLATFORM_IS_MAXOSX)
    #include "./mac/tb_keyboard_mac.h"
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
    #include "./emscripten/tb_keyboard_emscripten.h"
#elif (TB_PLATFORM_IS_NX)
    #include "./nx/tb_keyboard_nx.h"
#elif (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_WIIU)
    #include "./manual/tb_keyboard_manual.h"
#endif

#endif