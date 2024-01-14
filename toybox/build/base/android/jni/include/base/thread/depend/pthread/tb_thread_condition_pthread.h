/*!
 * 条件変数(Win32依存)
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_CONDITION_WIN32_H_
#define _INCLUDED_TB_THREAD_CONDITION_WIN32_H_

#include <base/container/tb_deque.h>

namespace toybox
{

struct TbThreadConditionDepend
{
    pthread_cond_t cond;
};

}

#endif
