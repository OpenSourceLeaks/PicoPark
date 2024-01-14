/*!
 * @file
 * @brief プロファイリング
 */

#ifndef _INCLUDED_TB_PROFILER_H_
#define _INCLUDED_TB_PROFILER_H_

// Profile可能か
#define TB_IS_ENABLE_PROFILER (TB_IS_DEBUG && 0)

#if TB_IS_ENABLE_PROFILER
#include "./depend/tb_profiler_depend.h"
#endif

#if TB_IS_ENABLE_PROFILER && defined(TB_PROFILER_THREAD_DEPEND)

#define TB_PROFILER_THREAD(name)  TB_PROFILER_THREAD_DEPEND(name)
#define TB_PROFILER_SCOPE(name)   TB_PROFILER_SCOPE_DEPEND(name)
#define TB_PROFILER_FRAME(name)   TB_PROFILER_FRAME_DEPEND(name)

#else

#define TB_PROFILER_THREAD(name)
#define TB_PROFILER_SCOPE(name)
#define TB_PROFILER_FRAME(name)

#endif

#endif