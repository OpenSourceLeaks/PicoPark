/*!
 * 1か0で分岐するマクロ
 * @author Miyake Shunsuke
 * @since 2012.03.12
 */

#ifndef _INCLUDED_TB_PP_IF_H_
#define _INCLUDED_TB_PP_IF_H_

#include <base/preprocessor/control/tb_pp_iif.h>
#include <base/preprocessor/logical/tb_pp_bool.h>

// 0でなければf、そうでなければt
#if TB_COMPILER_IS_MSVC
#   define TB_PP_IF(cond, t, f) TB_PP_II_I(cond, t, f)
#   define TB_PP_IF_I(cond, t, f) TB_PP_IIF(TB_PP_BOOL(cond), t, f)
#else
#   define TB_PP_IF(cond, t, f) TB_PP_IIF(TB_PP_BOOL(cond), t, f)
#endif

#endif
