/*!
 * 結合プリプロセス
 * @author Miyake Shunsuke
 * @since 2011.10.09
 */
 
#ifndef _INCLUDED_TB_PP_CAT_H_
#define _INCLUDED_TB_PP_CAT_H_


/*
#define TB_PP_CAT(a, b) TB_PP_CAT_OO((a, b))
#define TB_PP_CAT_OO(par) TB_PP_CAT_I ## par
#define TB_PP_CAT_I(a,b) TB_PP_CAT_II(a,b)
#define TB_PP_CAT_II(a,b) a##b
*/

#define TB_PP_CAT(a,b) TB_PP_CAT_I(a,b)
#define TB_PP_CAT_I(a,b) a##b

#endif
