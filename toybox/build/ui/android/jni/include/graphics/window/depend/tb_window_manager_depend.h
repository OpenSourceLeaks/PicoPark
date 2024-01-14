/*!
 * @file
 * @brief 依存クラスの分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */

#ifndef _INCLUDED_TB_WINDOW_MANAGER_DEPEND_H_
#define _INCLUDED_TB_WINDOW_MANAGER_DEPEND_H_

#include "./tb_window_depend_types.h"

#if (TB_PLATFORM_IS_WIN)
    #include "./win32/tb_window_manager_win32.h"
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
    #include "./x/tb_window_manager_x.h"
#elif (TB_PLATFORM_IS_NX)
    #include "./nx/tb_window_manager_nx.h"
#elif (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_FLASH)
namespace toybox {
    class TbWindowManagerDepend {};
}
#else
    #error "invalid platform"
#endif

#endif
