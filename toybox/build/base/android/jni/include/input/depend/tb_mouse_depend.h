/*!
 * @file
 * @brief マウスの機種依存コード
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_MOUSE_DEPEND_H_
#define _INCLDUED_TB_MOUSE_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
    #include "./win32/tb_mouse_win32.h"
#elif (TB_PLATFORM_IS_MAXOSX)
    #include "./mac/tb_mouse_mac.h"
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
    #include "./emscripten/tb_mouse_emscripten.h"
#elif (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_WIIU) || (TB_PLATFORM_IS_NX)
    #include "./manual/tb_mouse_manual.h"
#endif

#endif