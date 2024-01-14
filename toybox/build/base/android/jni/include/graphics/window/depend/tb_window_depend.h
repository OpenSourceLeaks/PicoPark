/*!
 * @file
 * @brief 依存クラスの分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_WINDOW_DEPEND_H_
#define _INCLUDED_TB_WINDOW_DEPEND_H_

#include "./tb_window_depend_types.h"

#if (TB_PLATFORM_IS_WIN)
#   include "./win32/tb_window_win32.h"
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
#   include "./x/tb_window_x.h"
#elif (TB_PLATFORM_IS_IOS)
#   include "./ios/tb_window_ios.h"
#elif (TB_PLATFORM_IS_NX)
#   include "./nx/tb_window_nx.h"
#elif (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_FLASH)
namespace toybox {
    class TbWindowDepend {};
}
#else
    #error "invalid platform"
#endif

#endif
