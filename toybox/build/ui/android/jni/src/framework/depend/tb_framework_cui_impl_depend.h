/*!
 * フレームワークの機種依存実装用ヘッダーの依存分岐
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAMEWORK_CUI_IMPL_DEPEND_H_
#define _INCLUDED_TB_FRAMEWORK_CUI_IMPL_DEPEND_H_

#if TB_PLATFORM_IS_WIN
#   include "./win32/tb_framework_cui_impl_win32.h"
#elif TB_PLATFORM_IS_NX
#   include "./nx/tb_framework_cui_impl_nx.h"
#elif TB_PLATFORM_IS_EMSCRIPTEN
#   include "./emscripten/tb_framework_cui_impl_emscripten.h"
#else
#   error "Invalid Platform"
#endif

#endif
