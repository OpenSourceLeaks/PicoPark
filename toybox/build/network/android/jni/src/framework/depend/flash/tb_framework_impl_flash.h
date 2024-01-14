/*!
* フレームワーク(Flash用)
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */

#ifndef _INCLUDED_TB_FRAMEWORK_IMPL_FLASH_H_
#define _INCLUDED_TB_FRAMEWORK_IMPL_FLASH_H_

#include "./tb_framework_entry_flash.h"

/*!
 * 毎フレーム処理
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */
AS3::ui::var enterFrame(void *arg, AS3::ui::var as3Args)
{
    using namespace toybox;
    try {
        // メインループ
        if( s_Impl ) {
            if( s_Impl->IsEnableMainLoop() ) {
                s_Impl->Update();
            }
        }
    } catch(AS3::ui::var e) {
        TbDummyFunc();
        char *err = AS3::ui::internal::utf8_toString(e);
        TbPrintf("Exception:%s\n",err);
        free(err);
    }
    return AS3::ui::internal::_undefined;
}

#endif
