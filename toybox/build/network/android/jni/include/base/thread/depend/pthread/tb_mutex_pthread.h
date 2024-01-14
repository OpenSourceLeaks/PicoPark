/*!
 * スレッドクラス(Win32依存)
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_MUTEX_WIN32_H_
#define _INCLUDED_TB_MUTEX_WIN32_H_

namespace toybox
{

struct TbMutexDepend
{
    pthread_mutex_t mutex;
};

}

#endif
