/*!
 * 依存クラスから共通クラスへのキャスト
 * @author Miyake Shunsuke
 * @since 2012.02.03
 */

#ifndef _INCLUDED_TB_DEPEND_CAST_H_
#define _INCLUDED_TB_DEPEND_CAST_H_

namespace toybox
{

#if 0

/*!
 * 依存クラスから共通クラスへのキャスト
 * @param source キャスト元
 * @retval Target キャスト成功
 * @retval NULL キャスト失敗
 * @author Miyake Shunsuke
 * @since 2012.02.03
 */
template<typename Target,typename Source>
Target TbDependCast(Source* source) {
    TbAddr offset = TB_OFFSET_OF(TbTypeRemovePointer<Target>::Type,m_Depend);
    TbAddr addr = reinterpret_cast<TbAddr>(source);
    return reinterpret_cast<Target>(addr-offset);
}

#endif

#define TB_DEPEND_TO_COMMON(type,member) \
    *reinterpret_cast<type*>(reinterpret_cast<TbAddr>(this) - TB_OFFSET_OF(type,member))

#define TB_DEPEND_TO_CONST_COMMON(type,member) \
    *reinterpret_cast<const type*>(reinterpret_cast<TbAddr>(this) - TB_OFFSET_OF(type,member))

}

#endif
