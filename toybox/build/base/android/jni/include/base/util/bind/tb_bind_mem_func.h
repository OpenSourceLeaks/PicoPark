/*!
 * Bindの実装部分
 * @author Miyake Shunsuke
 * @since 2012.03.11
 */
 
 #ifndef _INCLUDED_TB_BIND_MEM_FUNC_H_
 #define _INCLUDED_TB_BIND_MEM_FUNC_H_

#include "base/preprocessor/tb_pp_cat.h"
#include "base/preprocessor/repetition/tb_pp_enum_params.h"
#include "base/preprocessor/repetition/tb_pp_enum_binary_params.h"

namespace toybox
{

#define TB_BIND_MEM_FUNC_COMMA         TB_PP_COMMA_IF(TB_BIND_MEM_FUNC_ARGS_MAX)
// テンプレート引数列挙マクロ typename T0 , typename T1 , typename T2
#define TB_BIND_MEM_FUNC_DECL_TEMPLATE_ARGS TB_PP_ENUM_PARAMS(TB_BIND_MEM_FUNC_ARGS_MAX,typename A)
// テンプレート引数列挙マクロ T0 , T1 , T2
#define TB_BIND_MEM_FUNC_TEMPLATE_ARGS TB_PP_ENUM_PARAMS(TB_BIND_MEM_FUNC_ARGS_MAX,A)
// テンプレート引数型変数の列挙マクロ T0 t0 , T1 t1 , T2 t2
#define TB_BIND_MEM_FUNC_DECL_ARGS     TB_PP_ENUM_BINARY_PARAMS(TB_BIND_MEM_FUNC_ARGS_MAX,A,a)
// 変数列挙 a0 , a1 , a2
#define TB_BIND_MEM_FUNC_ARGS          TB_PP_ENUM_PARAMS(TB_BIND_MEM_FUNC_ARGS_MAX,a)

#define TB_BIND_MEM_FUNC_ARGS_MAX 0
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 1
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 2
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 3
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 4
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 5
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 6
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 7
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 8
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

#define TB_BIND_MEM_FUNC_ARGS_MAX 9
#include "./tb_bind_mem_func_inl.h"
#undef TB_BIND_MEM_FUNC_ARGS_MAX

}

#endif