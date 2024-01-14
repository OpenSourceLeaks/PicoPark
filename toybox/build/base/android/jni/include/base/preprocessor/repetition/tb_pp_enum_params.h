/*!
 * 番号を付与してコンマ区切りで列挙するためのマクロ
 * (例) TB_PP_ENUM_PARAMS(5,test) => test1,test2,test3,test4,test5
 * @author Miyake_Shunsuke
 * @since 2012.04.16
 */

#ifndef _INCLUDED_TB_PP_ENUM_PARAMS_H_
#define _INCLUDED_TB_PP_ENUM_PARAMS_H_

#include <base/preprocessor/repetition/tb_pp_repeat.h>
#include <base/preprocessor/punctuation/tb_pp_comma_if.h>

// ユーザーが利用するマクロ
#define TB_PP_ENUM_PARAMS(count,param) TB_PP_ENUM_PARAMS_I(count,param)
#define TB_PP_ENUM_PARAMS_I(count,param) TB_PP_REPEAT(count,TB_PP_ENUM_PARAMS_M,param)

// 展開
#define TB_PP_ENUM_PARAMS_M(z,n,param) TB_PP_COMMA_IF(n) TB_PP_CAT(param,n)

#endif
