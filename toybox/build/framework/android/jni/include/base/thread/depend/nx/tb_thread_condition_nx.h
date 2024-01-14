/*!
 * 条件変数(NX依存)
 * @author teco
 */
 
#ifndef _INCLUDED_TB_THREAD_CONDITION_NX_H_
#define _INCLUDED_TB_THREAD_CONDITION_NX_H_

#include <base/container/tb_deque.h>

namespace toybox
{

struct TbThreadConditionDepend
{
    nn::os::ConditionVariableType cond;
};

}

#endif
