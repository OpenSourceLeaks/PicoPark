/*!
 * @file
 * @brief パッドの機種依存コード
 */

#ifndef _INCLDUED_TB_PAD_DEPEND_H_
#define _INCLDUED_TB_PAD_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
    #include "./win32/tb_pad_win32.h"
#elif (TB_PLATFORM_IS_MAXOSX)
    #include "./mac/tb_pad_mac.h"
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
    #include "./emscripten/tb_pad_emscripten.h"
#elif (TB_PLATFORM_IS_WIIU)
    #include "./wiiu/tb_pad_wiiu.h"
#elif (TB_PLATFORM_IS_NX)
    #include "./nx/tb_pad_nx.h"
#elif (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_ANDROID)
    #include "./manual/tb_pad_manual.h"
#endif

#endif