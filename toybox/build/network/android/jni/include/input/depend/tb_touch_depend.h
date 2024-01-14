/*!
 * @file
 * @brief タッチの機種依存コード
 */

#ifndef _INCLDUED_TB_TOUCH_DEPEND_H_
#define _INCLDUED_TB_TOUCH_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
//    #include "./manual/tb_touch_manual.h"
    #include "./win32/tb_touch_win32.h"
#elif (TB_PLATFORM_IS_MAXOSX)
    #include "./mac/tb_touch_mac.h"
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
    #include "./emscripten/tb_touch_emscripten.h"
#elif (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_WIIU) || (TB_PLATFORM_IS_NX)
    #include "./manual/tb_touch_manual.h"
//    #include "./ios/tb_touch_ios.h"
#endif

#endif