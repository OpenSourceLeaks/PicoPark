/*!
 * スレッドクラス(NX依存)
 * @author teco
 */
 
#ifndef _INCLUDED_TB_MUTEX_NX_H_
#define _INCLUDED_TB_MUTEX_NX_H_

namespace toybox
{

struct TbMutexDepend
{
    nn::os::MutexType   mutex;
};

}

#endif
