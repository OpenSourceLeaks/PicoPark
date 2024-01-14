/*!
 * @file
 * @brief 
 */
 
#ifndef _INCLUDED_TB_WINDOW_MANAGER_NX_H_
#define _INCLUDED_TB_WINDOW_MANAGER_NX_H_

#include <base/string/tb_static_string.h>
#include <nn/vi.h>

namespace toybox
{

class TbWindow;

/*!
 * @class 
 */
struct TbWindowManagerDepend
{
    // コンストラクタ
    TbWindowManagerDepend()
        : display(nullptr)
    {}

    nn::vi::Display* display;

};

}

#endif
