/*!
 * Tuple系マクロ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_PP_TUPLE_H_
#define _INCLUDED_TB_PP_TUPLE_H_

// 要素取り出し
#define TB_PP_TUPLE_ELEM_1_0(a) a

#define TB_PP_TUPLE_ELEM_2_0(a, b) a
#define TB_PP_TUPLE_ELEM_2_1(a, b) b

#define TB_PP_TUPLE_ELEM_3_0(a, b, c) a
#define TB_PP_TUPLE_ELEM_3_1(a, b, c) b
#define TB_PP_TUPLE_ELEM_3_2(a, b, c) c

// 独立させる
#define TB_PP_TUPLE_REM_0()
#define TB_PP_TUPLE_REM_1(a) a
#define TB_PP_TUPLE_REM_2(a, b) a, b
#define TB_PP_TUPLE_REM_3(a, b, c) a, b, c

#endif
