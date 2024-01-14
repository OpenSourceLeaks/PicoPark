/*!
 * スコープロック
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */

#ifndef _INCLUDED_TB_SCOPED_LOCK_H_
#define _INCLUDED_TB_SCOPED_LOCK_H_

#include <base/thread/tb_mutex.h>

namespace toybox
{

class TbScopedLock
{
public:
    // コンストラクタ
    TbScopedLock( TbMutex& mutex ) 
        : m_Mutex(mutex) 
    {
        m_Mutex.Lock();
    }
    // デストラクタ
    ~TbScopedLock() {
        m_Mutex.Unlock();
    }
private:
    TbMutex& m_Mutex;
};

}

#endif
