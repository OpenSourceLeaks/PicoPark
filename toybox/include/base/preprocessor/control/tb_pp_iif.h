/*!
 * 1か0で分岐するマクロ
 * @author Miyake Shunsuke
 * @since 2012.03.12
 */

#ifndef _INCLUDED_TB_PREPROCESSOR_IIF_H_
#define _INCLUDED_TB_PREPROCESSOR_IIF_H_

// bitが1ならt、0ならfを返す
#define TB_PP_IIF(bit,t,f) TB_PP_IIF_I(bit,t,f)

#if TB_COMPILER_IS_MSVC
#   define TB_PP_IIF_I(bit,t,f) TB_PP_IIF_II( TB_PP_IIF_## bit(t,f) )
#   define TB_PP_IIF_II(id) id
#else
#   define TB_PP_IIF_I(bit,t,f) TB_PP_IIF_## bit(t,f)
#endif

// FALSEならf、TRUEならt
#define TB_PP_IIF_0(t, f) f
#define TB_PP_IIF_1(t, f) t

#endif
