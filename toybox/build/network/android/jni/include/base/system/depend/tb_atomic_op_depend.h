/*! 
 * @file
 * @brief atomic
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_ATOMIC_OP_DEPEND_H_
#define _INCLUDED_TB_ATOMIC_OP_DEPEND_H_


#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
    #include "./win32/tb_atomic_op_win32.h"
#elif TB_PLATFORM_IS_ANDROID
    #include "./android/tb_atomic_op_android.h"
#elif TB_PLATFORM_IS_FLASH
    #include "./flash/tb_atomic_op_flash.h"
#elif TB_PLATFORM_IS_EMSCRIPTEN
    #include "./fake/tb_atomic_op_fake.h"
#elif TB_PLATFORM_IS_APPLE
    #include "./apple/tb_atomic_op_apple.h"
#elif TB_PLATFORM_IS_WIIU
    #include "./wiiu/tb_atomic_op_wiiu.h"
#elif TB_PLATFORM_IS_NX
    #include "./gcc/tb_atomic_op_gcc.h"
#else
    #error "invalid platform"
#endif

#endif
