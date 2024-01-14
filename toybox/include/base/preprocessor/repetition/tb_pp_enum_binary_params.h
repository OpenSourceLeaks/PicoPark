/*!
 * 番号を付与してコンマ区切りで列挙するためのマクロ
 * (例) TB_PP_ENUM_BINARY_PARAMS(5,T,test) => T1 test1,T2 test2,T3 test3,T4 test4,T5 test5
 * @author Miyake_Shunsuke
 * @since 2012.04.16
 */

#ifndef _INCLUDED_TB_PP_ENUM_BINARY_PARAMS_H_
#define _INCLUDED_TB_PP_ENUM_BINARY_PARAMS_H_

#include <base/preprocessor/repetition/tb_pp_repeat.h>
#include <base/preprocessor/punctuation/tb_pp_comma_if.h>
#include <base/preprocessor/tb_pp_tuple.h>

// ユーザーが利用するマクロ
#define TB_PP_ENUM_BINARY_PARAMS(count,param1,param2) TB_PP_ENUM_BINARY_PARAMS_I(count,param1,param2)
#define TB_PP_ENUM_BINARY_PARAMS_I(count,param1,param2) TB_PP_REPEAT(count,TB_PP_ENUM_BINARY_PARAMS_M,(param1,param2))

// 展開
#if TB_COMPILER_IS_MSVC
#    define TB_PP_ENUM_BINARY_PARAMS_M(z,n,param) TB_PP_ENUM_BINARY_PARAMS_M_1(z,n,TB_PP_TUPLE_ELEM_2_0 ## param , TB_PP_TUPLE_ELEM_2_1 ## param )
#else
#    define TB_PP_ENUM_BINARY_PARAMS_M(z,n,param) TB_PP_ENUM_BINARY_PARAMS_M_1(z,n,TB_PP_TUPLE_ELEM_2_0 param , TB_PP_TUPLE_ELEM_2_1 param )
#endif

#define TB_PP_ENUM_BINARY_PARAMS_M_1(z,n,param1,param2) TB_PP_COMMA_IF(n) TB_PP_CAT(param1,n) TB_PP_CAT(param2,n)

#endif
