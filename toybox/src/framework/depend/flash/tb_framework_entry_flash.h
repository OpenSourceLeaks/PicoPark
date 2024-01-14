/*!
* フレームワーク(Flash用)
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */

#ifndef _INCLUDED_TB_FRAMEWORK_ENTRY_FLASH_H_
#define _INCLUDED_TB_FRAMEWORK_ENTRY_FLASH_H_

/*!
 * 毎フレーム処理
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */
extern AS3::ui::var enterFrame(void *arg, AS3::ui::var as3Args);

namespace toybox
{

// 何もしないダミー関数(これを実装する.cppをリンクするために)
extern void TbDummyFunc();

}

#endif
