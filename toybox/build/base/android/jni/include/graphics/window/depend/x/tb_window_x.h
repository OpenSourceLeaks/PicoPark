/*!
 * X用ウィンドウ
 * @author teco
 */
 
#ifndef _INCLUDED_TB_WINDOW_X_H_
#define _INCLUDED_TB_WINDOW_X_H_

#include <X11/Xlib.h>

namespace toybox
{

class TbWindow;

/*!
 * @class 
 */
struct TbWindowDepend
{
    Window      window;
};

}

#endif
