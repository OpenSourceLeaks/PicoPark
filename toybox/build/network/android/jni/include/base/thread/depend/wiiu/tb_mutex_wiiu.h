/*!
 * スレッドクラス(WiiU依存)
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_MUTEX_WIIU_H_
#define _INCLUDED_TB_MUTEX_WIIU_H_

namespace toybox
{

struct TbMutexDepend
{
    OSMutex mutex;
};

}

#endif
