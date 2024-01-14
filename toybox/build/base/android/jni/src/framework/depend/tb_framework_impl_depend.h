/*!
 * フレームワークの機種依存実装用ヘッダーの依存分岐
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_FRAMEWORK_IMPL_DEPEND_H_
#define _INCLUDED_TB_FRAMEWORK_IMPL_DEPEND_H_

#if TB_PLATFORM_IS_WIN
#   include "./win32/tb_framework_impl_win32.h"
#elif TB_PLATFORM_IS_IOS
    // 何もしない
#elif TB_PLATFORM_IS_ANDROID
#   include "./android/tb_framework_impl_android.h"
#elif TB_PLATFORM_IS_FLASH
#   include "./flash/tb_framework_impl_flash.h"
#elif TB_PLATFORM_IS_EMSCRIPTEN
#   include "./emscripten/tb_framework_impl_emscripten.h"
#elif TB_PLATFORM_IS_NX
#   include "./nx/tb_framework_impl_nx.h"
#endif

#endif
