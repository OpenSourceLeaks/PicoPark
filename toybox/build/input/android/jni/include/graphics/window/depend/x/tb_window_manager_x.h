/*!
 * @file
 * @brief 
 * @author Miyake Shunsuke
 * @since 2010.4.10
 */
 
#ifndef _INCLUDED_TB_WINDOW_MANAGER_WIN_H_
#define _INCLUDED_TB_WINDOW_MANAGER_WIN_H_

#include <base/string/tb_static_string.h>
#include <X11/Xlib.h>

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

    Display*    display;

};

}

#endif
