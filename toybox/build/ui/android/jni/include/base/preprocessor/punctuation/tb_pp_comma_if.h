/*!
 * 0でなければコンマになるマクロ
 * @author Miyake_Shunsuke
 * @since 2012.04.16
 */

#ifndef _INCLUDED_TB_PREPROCESS_COMMA_IF_H_
#define _INCLUDED_TB_PREPROCESS_COMMA_IF_H_
 
#include <base/preprocessor/tb_pp_define.h>
#include <base/preprocessor/control/tb_pp_iif.h>
#include <base/preprocessor/logical/tb_pp_bool.h>

#define TB_PP_COMMA_IF(cond) TB_PP_COMMA_IF_I(cond)
#define TB_PP_COMMA_IF_I(cond) TB_PP_IIF(TB_PP_BOOL(cond), TB_PP_COMMA, TB_PP_EMPTY)()

#endif
