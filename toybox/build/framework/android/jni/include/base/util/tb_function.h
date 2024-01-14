/*!
 * functionテンプレートヘッダーインクルードファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_FUNCTION_H_
#define _INCLDUED_TB_FUNCTION_H_

#include "base/util/tb_ref.h"
#include "base/preprocessor/tb_pp_cat.h"
#include "base/preprocessor/repetition/tb_pp_enum_params.h"
#include "base/preprocessor/repetition/tb_pp_enum_binary_params.h"

// 引数が0以上だとコンマ
#define TB_FUNCTION_COMMA TB_PP_COMMA_IF(TB_FUNCTION_ARGS_MAX)
// テンプレート引数列挙マクロ typename T0 , typename T1 , typename T2
#define TB_FUNCTION_TEMPLATE_ARGS TB_FUNCTION_COMMA TB_PP_ENUM_PARAMS(TB_FUNCTION_ARGS_MAX,typename T)
// テンプレート引数型変数の列挙マクロ T0 t0 , T1 t1 , T2 t2
#define TB_FUNCTION_DECL_ARGS     TB_PP_ENUM_BINARY_PARAMS(TB_FUNCTION_ARGS_MAX,T,t)
// 変数列挙 t0 , t1 , t2
#define TB_FUNCTION_ARGS          TB_PP_ENUM_PARAMS(TB_FUNCTION_ARGS_MAX,t)
// 変数列挙 TbRef(t0) , TbRef(t1) , TbRef(t2)
#define TB_FUNCTION_INVOKE_ARGS TB_FUNCTION_INVOKE_ARGS_I(TB_FUNCTION_ARGS_MAX,t)
#define TB_FUNCTION_INVOKE_ARGS_I(count,param) TB_PP_REPEAT(count,TB_FUNCTION_PARAMS_M,param)
#define TB_FUNCTION_PARAMS_M(z,n,param) TB_PP_COMMA_IF(n) TbRef( TB_PP_CAT(param,n) )

// ヒープを使うFunctionテンプレート
// メンバ関数ポインタ

#define TB_FUNCTION_ARGS_MAX 0
#include "./function/tb_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 1
#include "./function/tb_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 2
#include "./function/tb_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 3
#include "./function/tb_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 4
#include "./function/tb_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 5
#include "./function/tb_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

// ヒープを一切使用しないFunctionテンプレート
#define TB_STATIC_FUNCTION_BUF_SIZE 16
#define TB_STATIC_FUNCTION_CLASS_NAME TbStaticFunction16

#define TB_FUNCTION_ARGS_MAX 0
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 1
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 2
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 3
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 4
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 5
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#undef TB_STATIC_FUNCTION_CLASS_NAME
#undef TB_STATIC_FUNCTION_BUF_SIZE

#define TB_STATIC_FUNCTION_BUF_SIZE 32
#define TB_STATIC_FUNCTION_CLASS_NAME TbStaticFunction32

#define TB_FUNCTION_ARGS_MAX 0
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 1
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 2
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 3
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 4
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 5
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#undef TB_STATIC_FUNCTION_CLASS_NAME
#undef TB_STATIC_FUNCTION_BUF_SIZE

#define TB_STATIC_FUNCTION_BUF_SIZE 64

#define TB_FUNCTION_ARGS_MAX 0
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 1
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 2
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 3
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 4
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 5
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#undef TB_STATIC_FUNCTION_CLASS_NAME
#undef TB_STATIC_FUNCTION_BUF_SIZE

#define TB_STATIC_FUNCTION_BUF_SIZE 128

#define TB_FUNCTION_ARGS_MAX 0
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 1
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 2
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 3
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 4
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 5
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

// 後片付け
#undef TB_STATIC_FUNCTION_CLASS_NAME
#undef TB_STATIC_FUNCTION_BUF_SIZE

#define TB_STATIC_FUNCTION_BUF_SIZE 256

#define TB_FUNCTION_ARGS_MAX 0
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 1
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 2
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 3
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 4
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

#define TB_FUNCTION_ARGS_MAX 5
#include "./function/tb_static_function_inl.h"
#undef TB_FUNCTION_ARGS_MAX

// 後片付け
#undef TB_STATIC_FUNCTION_CLASS_NAME
#undef TB_STATIC_FUNCTION_BUF_SIZE

#endif