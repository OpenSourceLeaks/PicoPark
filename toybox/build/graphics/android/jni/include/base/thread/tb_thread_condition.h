/*!
 * 条件変数
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */

#ifndef _INCLUDED_TB_THREAD_CONDITION_H_
#define _INCLUDED_TB_THREAD_CONDITION_H_

#include <base/thread/tb_thread_types.h>
#include <base/thread/tb_mutex.h>
#include "./depend/tb_thread_condition_depend.h"

namespace toybox
{

class TbThreadCondition
{
public:
    
    /*!
     * コンストラクタ
     * @param capacity 受け入れ可能な数
     */
    TbThreadCondition( TbUint32 capacity );
    
    // デストラクタ
    ~TbThreadCondition();

public:
    
    // 待ち
    TbResult Wait( TbMutex& mutex );
    
    // 起こす
    void Notify();
    
    // 全て起こす
    void NotifyAll();

private:
    TbMutex                 m_InternalMutex;
    TbThreadConditionDepend m_Depend;
};

#if 0
class TbThreadCountDownLatch
{
public:

    // コンストラクタ
    TbThreadCountDownLatch( TbSizeT count );

    // デストラクタ
    ~TbThreadCountDownLatch();

public:

    // カウントダウン
    void CountDown();

    // カウントが0になるまで待機する
    void Awake();

};
#endif

}

#endif
