/*!
 * @file
 * @brief スレッド依存部分
 * @author Miyake Shunsuke
 * @since 2011.6.5
 */

#ifndef _INCLUDED_TB_MUTEX_DEPEND_H_
#define _INCLUDED_TB_MUTEX_DEPEND_H_

#if (TB_THREADAPI_IS_WIN32)
    #include "./win32/tb_mutex_win32.h"
#elif (TB_THREADAPI_IS_PTHREAD)
    #include "./pthread/tb_mutex_pthread.h"
#elif (TB_THREADAPI_IS_WIIU)
    #include "./wiiu/tb_mutex_wiiu.h"
#elif (TB_THREADAPI_IS_NX)
    #include "./nx/tb_mutex_nx.h"
#elif (TB_THREADAPI_IS_TASK)
    class TbMutexDepend {
        TbMutexDepend() {}; // リンクエラーを発生させる
    };
#else
    #error ""
#endif

#endif