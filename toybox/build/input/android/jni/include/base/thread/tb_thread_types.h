/*!
 * スレッド定義
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_TYPES_H_
#define _INCLUDED_TB_THREAD_TYPES_H_

#define TB_THREADAPI_WIN32   1  // Win32API
#define TB_THREADAPI_PTHREAD 2  // pthread
#define TB_THREADAPI_TASK    3  // Taskで疑似実現
#define TB_THREADAPI_WIIU    4  // WiiU
#define TB_THREADAPI_NX      5  // NX

#if (TB_PLATFORM_IS_WIN)
    #define TB_THREADAPI_TARGET TB_THREADAPI_WIN32
#elif (TB_PLATFORM_IS_ANDROID || TB_PLATFORM_IS_FLASH || TB_PLATFORM_IS_APPLE)
    #define TB_THREADAPI_TARGET TB_THREADAPI_PTHREAD
    #include <pthread.h>
#elif (TB_PLATFORM_IS_EMSCRIPTEN)
    #define TB_THREADAPI_TARGET TB_THREADAPI_TASK
#elif (TB_PLATFORM_IS_WIIU)
    #define TB_THREADAPI_TARGET TB_THREADAPI_WIIU
#elif (TB_PLATFORM_IS_NX)
#define TB_THREADAPI_TARGET TB_THREADAPI_NX
#else
    #error "Invalid platform"
#endif

#define TB_THREADAPI_IS_WIN32 (TB_THREADAPI_TARGET==TB_THREADAPI_WIN32)
#define TB_THREADAPI_IS_PTHREAD (TB_THREADAPI_TARGET==TB_THREADAPI_PTHREAD)
#define TB_THREADAPI_IS_WIIU (TB_THREADAPI_TARGET==TB_THREADAPI_WIIU)
#define TB_THREADAPI_IS_NX   (TB_THREADAPI_TARGET==TB_THREADAPI_NX)
#define TB_THREADAPI_IS_TASK (TB_THREADAPI_TARGET==TB_THREADAPI_TASK)

namespace toybox
{

enum TbThreadUpdateResult
{
    TB_THREAD_UPDATE_RESULT_FINISH , // 完了
    TB_THREAD_UPDATE_RESULT_PENDING , // まだ終わってないよ
    TB_THREAD_UPDATE_RESULT_MAX
};

/*!
 * 優先順位
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
enum TbThreadPriority
{
    TB_THREAD_PRIORITY_BEGIN ,
    TB_THREAD_PRIORITY_LOWEST = TB_THREAD_PRIORITY_BEGIN , 
    TB_THREAD_PRIORITY_BELOW_NORMAL , 
    TB_THREAD_PRIORITY_NORMAL , 
    TB_THREAD_PRIORITY_ABOVE_NORMAL , 
    TB_THREAD_PRIORITY_HIGHETST ,
    TB_THREAD_PRIORITY_END ,
    TB_THREAD_PRIORITY_MAX = TB_THREAD_PRIORITY_END - TB_THREAD_PRIORITY_BEGIN , 
};

// スレッド指定のプロセッサ（全て）
#define TB_THREAD_PROCESSOR_ALL -1

// スレッド毎のスタックサイズデフォルト
#define TB_THREAD_STACK_SIZE_DEFAULT (64*1024)

/*!
 * スレッドパラメータ
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
struct TbThreadParam
{
    TbThreadParam() : 
        name() , 
        processor(TB_THREAD_PROCESSOR_ALL) , 
        priority(TB_THREAD_PRIORITY_NORMAL) ,
        stackSize(TB_THREAD_STACK_SIZE_DEFAULT)
    {}
    TbStaticString64 name;
    TbSint32         processor;
    TbThreadPriority priority;
    TbUint32         stackSize;
};

}

#endif
