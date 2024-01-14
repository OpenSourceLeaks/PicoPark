/*!
 * ダイナミックキャスト
 * @author Miyake Shunsuke
 * @since 2012.02.03
 */

#ifndef _INCLUDED_TB_DYNAMIC_CAST_H_
#define _INCLUDED_TB_DYNAMIC_CAST_H_

#include <base/util/tb_type_id.h>
#include <base/util/tb_type_traits.h>

namespace toybox
{

// DynamicCast用基底クラス埋め込みマクロ
#define TB_DEFINE_BASE_CLASS(Self) \
    virtual TbBool IsCastableTo(toybox::TbTypeId id) const { \
        return (toybox::TbGetTypeId<Self>() == id); \
    } \

// DynamicCast用派生クラス埋め込みマクロ
#define TB_DEFINE_SUB_CLASS(Self,Super) \
    virtual TbBool IsCastableTo(toybox::TbTypeId id) const TB_OVERRIDE { \
        return (toybox::TbGetTypeId<Self>() == id) ? TB_TRUE : Super::IsCastableTo(id); \
    } \

/*!
 * ダイナミックキャスト
 * @param source キャスト元
 * @retval Target キャスト成功
 * @retval NULL キャスト失敗
 * @author Miyake Shunsuke
 * @since 2012.02.03
 */
template<typename Target,typename Source>
Target TbDynamicCast(Source* source){
    if( source->IsCastableTo(toybox::TbGetTypeId<typename toybox::TbRemoveCV<typename toybox::TbRemovePointer<Target>::Type>::Type>()) ){
        return static_cast<Target>(source);
    }
    return NULL;
}

}

#endif
