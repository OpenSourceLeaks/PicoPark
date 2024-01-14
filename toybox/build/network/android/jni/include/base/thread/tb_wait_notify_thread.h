/*!
 * Wait/Notify用スレッド
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_WAIT_NOTIFY_THREAD_H_
#define _INCLUDED_TB_WAIT_NOTIFY_THREAD_H_

#include <base/thread/tb_thread.h>
#include <base/thread/tb_thread_condition.h>
#include <base/thread/tb_scoped_lock.h>
#include <base/util/tb_function.h>

namespace toybox
{

/*!
 * 関数スレッドクラス
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
class TbWaitNotifyThread : public TbThread
{
public:
    typedef TbThread Super;
    typedef TbStaticFunction32<void ()> Func;
public:
    
    // コンストラクタ
    TbWaitNotifyThread()
        : Super()
        , m_Condition(1)
        , m_IsEnableUpdate(TB_FALSE)
    {
    }

    // デストラクタ
    virtual ~TbWaitNotifyThread() 
    {}

public:

    // 初期化
    void Initialize( const char* name , const Func& updateFunc )
    {
        Super::Initialize(name);
        m_UpdateFunc = updateFunc;
    }

    // 初期化
    void Initialize( const TbThreadParam& param , const Func& updateFunc )
    {
        Super::Initialize(param);
        m_UpdateFunc = updateFunc;
    }

public:

    // 起こす
    void Notify() {
        TbScopedLock lock(m_Mutex);
        m_IsEnableUpdate = TB_TRUE;
        m_Condition.Notify();
    }

protected:

    // スレッドループ処理
    virtual TbThreadUpdateResult Update(){
        while ( 1 )
        {
            if( IsRequestedFinish() ){
                break;
            }else{
                {
                    TbScopedLock lock(m_Mutex);
                    if( m_IsEnableUpdate ) {
                        m_IsEnableUpdate = TB_FALSE;
                    }else{
                        m_Condition.Wait(m_Mutex);
                    }
                }
                if( m_UpdateFunc.IsValid() ){
                    m_UpdateFunc();
                }
            }
        }
        return TB_THREAD_UPDATE_RESULT_FINISH;
    };

private:
    TbMutex             m_Mutex;
    TbThreadCondition   m_Condition;
    Func                m_UpdateFunc;
    TbBool              m_IsEnableUpdate;
};

}

#endif
