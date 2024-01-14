/*!
 * ディスプレイ管理クラス( 3DAPI非依存 )
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */

#ifndef _INCLUDED_TB_DISPLAY_MANAGER_DEPEND_H_
#define _INCLUDED_TB_DISPLAY_MANAGER_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
#   include <graphics/display/depend/win32/tb_display_manager_win32.h>
#elif (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_EMSCRIPTEN) || (TB_PLATFORM_IS_NX)
#   include <graphics/display/depend/fixed/tb_display_manager_fixed.h>
#elif (TB_PLATFORM_IS_FLASH)
#   include <graphics/display/depend/flash/tb_display_manager_flash.h>
#elif (TB_PLATFORM_IS_IOS)
    struct TbDisplayManagerDepend {};
#else
#   error ""
#endif

#endif