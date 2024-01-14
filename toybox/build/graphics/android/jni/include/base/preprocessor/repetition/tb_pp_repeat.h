/*!
 * 繰り返しプリプロセス(再帰は未対応)
 * @author Miyake Shunsuke
 * @since 2011.10.09
 */
 
#ifndef _INCLUDED_TB_PP_REPEAT_H_
#define _INCLUDED_TB_PP_REPEAT_H_

#include <base/preprocessor/tb_pp_cat.h>

# if 0
#    // 使用例
#    define TB_PP_REPEAT(count,macro,data)
# endif

#define TB_PP_REPEAT TB_PP_CAT(TB_PP_REPEAT_,1)

#define TB_PP_REPEAT_1(c,m,d) TB_PP_REPEAT_1_ ## c(m,d)

#define TB_PP_REPEAT_1_0(m,d)
#define TB_PP_REPEAT_1_1(m,d) m(2,0,d) 
#define TB_PP_REPEAT_1_2(m,d) TB_PP_REPEAT_1_1(m,d) m(2,1,d)
#define TB_PP_REPEAT_1_3(m,d) TB_PP_REPEAT_1_2(m,d) m(2,2,d)
#define TB_PP_REPEAT_1_4(m,d) TB_PP_REPEAT_1_3(m,d) m(2,3,d)
#define TB_PP_REPEAT_1_5(m,d) TB_PP_REPEAT_1_4(m,d) m(2,4,d)
#define TB_PP_REPEAT_1_6(m,d) TB_PP_REPEAT_1_5(m,d) m(2,5,d)
#define TB_PP_REPEAT_1_7(m,d) TB_PP_REPEAT_1_6(m,d) m(2,6,d)
#define TB_PP_REPEAT_1_8(m,d) TB_PP_REPEAT_1_7(m,d) m(2,7,d)
#define TB_PP_REPEAT_1_9(m,d) TB_PP_REPEAT_1_8(m,d) m(2,8,d)
#define TB_PP_REPEAT_1_10(m,d) TB_PP_REPEAT_1_9(m,d) m(2,9,d)
#define TB_PP_REPEAT_1_11(m,d) TB_PP_REPEAT_1_10(m,d) m(2,10,d)
#define TB_PP_REPEAT_1_12(m,d) TB_PP_REPEAT_1_11(m,d) m(2,11,d)
#define TB_PP_REPEAT_1_13(m,d) TB_PP_REPEAT_1_12(m,d) m(2,12,d)
#define TB_PP_REPEAT_1_14(m,d) TB_PP_REPEAT_1_13(m,d) m(2,13,d)
#define TB_PP_REPEAT_1_15(m,d) TB_PP_REPEAT_1_14(m,d) m(2,14,d)
#define TB_PP_REPEAT_1_16(m,d) TB_PP_REPEAT_1_15(m,d) m(2,15,d)
#define TB_PP_REPEAT_1_17(m,d) TB_PP_REPEAT_1_16(m,d) m(2,16,d)
#define TB_PP_REPEAT_1_18(m,d) TB_PP_REPEAT_1_17(m,d) m(2,17,d)
#define TB_PP_REPEAT_1_19(m,d) TB_PP_REPEAT_1_18(m,d) m(2,18,d)
#define TB_PP_REPEAT_1_20(m,d) TB_PP_REPEAT_1_19(m,d) m(2,19,d)
#define TB_PP_REPEAT_1_21(m,d) TB_PP_REPEAT_1_20(m,d) m(2,20,d)
#define TB_PP_REPEAT_1_22(m,d) TB_PP_REPEAT_1_21(m,d) m(2,21,d)
#define TB_PP_REPEAT_1_23(m,d) TB_PP_REPEAT_1_22(m,d) m(2,22,d)
#define TB_PP_REPEAT_1_24(m,d) TB_PP_REPEAT_1_23(m,d) m(2,23,d)
#define TB_PP_REPEAT_1_25(m,d) TB_PP_REPEAT_1_24(m,d) m(2,24,d)
#define TB_PP_REPEAT_1_26(m,d) TB_PP_REPEAT_1_25(m,d) m(2,25,d)
#define TB_PP_REPEAT_1_27(m,d) TB_PP_REPEAT_1_26(m,d) m(2,26,d)
#define TB_PP_REPEAT_1_28(m,d) TB_PP_REPEAT_1_27(m,d) m(2,27,d)
#define TB_PP_REPEAT_1_29(m,d) TB_PP_REPEAT_1_28(m,d) m(2,28,d)
#define TB_PP_REPEAT_1_30(m,d) TB_PP_REPEAT_1_29(m,d) m(2,29,d)
#define TB_PP_REPEAT_1_31(m,d) TB_PP_REPEAT_1_30(m,d) m(2,30,d)
#define TB_PP_REPEAT_1_32(m,d) TB_PP_REPEAT_1_31(m,d) m(2,31,d)

#endif
