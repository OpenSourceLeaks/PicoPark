/*!
 * ミューテックス
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
 
#ifndef _INCLUDED_TB_MUTEX_H_
#define _INCLUDED_TB_MUTEX_H_

#include <base/thread/tb_thread_types.h>
#include "./depend/tb_mutex_depend.h"

namespace toybox
{

class TbMutex
{
public:
    // コンストラクタ
    TbMutex();
    // コピーコンストラクタ
    TbMutex( const TbMutex& src );
    // デストラクタ
    ~TbMutex();
public:
    // ロック
    TB_DEPEND void Lock();
    // ロックを試みる
    TB_DEPEND TbBool TryLock();
    // アンロック
    TB_DEPEND void Unlock();
public:
    // 依存部分取得
    TbMutexDepend& GetDepend() {
        return m_Depend;
    }
    // 依存部分取得
    const TbMutexDepend& GetDepend() const {
        return m_Depend;
    }
private:
    TbMutexDepend m_Depend;
};

}

#endif
