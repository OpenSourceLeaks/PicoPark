/*!
 * @file
 * @brief プロファイリング
 */

#ifndef _INCLUDED_TB_PROFILER_DEPEND_H_
#define _INCLUDED_TB_PROFILER_DEPEND_H_

#if TB_IS_DEBUG

#if TB_PLATFORM_IS_WIN
#   include <Brofiler.h>
#if TB_IS_64BIT
#   pragma comment(lib, "ProfilerCore64.lib")
#else
#   pragma comment(lib, "ProfilerCore32.lib")
#endif

#define TB_PROFILER_THREAD_DEPEND(name) BROFILER_THREAD(name)
#define TB_PROFILER_SCOPE_DEPEND(name)  BROFILER_CATEGORY(name,Profiler::Color::Red)
#define TB_PROFILER_FRAME_DEPEND(name)  BROFILER_FRAME(name)
#endif

#else

#define TB_PROFILER_THREAD_DEPEND(name) 
#define TB_PROFILER_SCOPE_DEPEND(name)  
#define TB_PROFILER_FRAME_DEPEND(name)  

#endif

#endif