/*!
 * 条件変数(WiiU依存)
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_THREAD_CONDITION_WIIU_H_
#define _INCLUDED_TB_THREAD_CONDITION_WIIU_H_

#include <base/container/tb_deque.h>

namespace toybox
{

struct TbThreadConditionDepend
{
    // コンストラクタ
    TbThreadConditionDepend( TbUint32 capacity )
    {}
    // デストラクタ
    ~TbThreadConditionDepend()
    {}

    OSEvent          m_Event;
};

}

#endif
