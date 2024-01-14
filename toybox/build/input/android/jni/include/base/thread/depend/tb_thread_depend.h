﻿/*!
 * @file
 * @brief スレッド依存部分
 * @author Miyake Shunsuke
 * @since 2011.6.5
 */

#ifndef _INCLUDED_TB_THREAD_DEPEND_H_
#define _INCLUDED_TB_THREAD_DEPEND_H_

#if (TB_THREADAPI_IS_WIN32)
    #include <base/thread/depend/win32/tb_thread_win32.h>
#elif (TB_THREADAPI_IS_PTHREAD)
    #include "./pthread/tb_thread_pthread.h"
#elif (TB_THREADAPI_IS_WIIU)
    #include "./wiiu/tb_thread_wiiu.h"
#elif (TB_THREADAPI_IS_NX)
    #include "./nx/tb_thread_nx.h"
#elif (TB_THREADAPI_IS_TASK)
    class TbThreadDepend {
        TbThreadDepend(){}; // リンクエラーを発生させる
    };
#else
    #error
#endif

#endif