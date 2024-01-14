/*!
 * 関数を呼び出すスレッドグループ
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_GROUP_H_
#define _INCLUDED_TB_THREAD_GROUP_H_

#include <base/container/tb_array.h>
#include <base/thread/tb_func_thread.h>

namespace toybox
{

/*!
 * 関数スレッドクラス
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
class TbThreadGroup
{
public:

    class ThreadHandle
    {
    public:
        // デフォルトコンストラクタ
        ThreadHandle()
            : m_Thread(NULL)
        {}
        // コンストラクタ
        ThreadHandle( TbThread* thread )
            : m_Thread(thread)
        {}
    public:

        // NULLオブジェクトかどうか
        TbBool IsNull() const {
            return m_Thread != NULL;
        }

        // プロセッサ割当
        TbResult SetProcessor( TbSint32 processor ) {
            if( m_Thread ) {
                return m_Thread->SetProcessor(processor);
            }
            return TB_E_FAIL;
        }

        // 優先順位設定
        TbResult SetPriority( TbThreadPriority priority ) {
            if( m_Thread ) {
                return m_Thread->SetPriority(priority);
            }
            return TB_E_FAIL;
        }

    private:
        TbThread* m_Thread;
    };

public:
    
    // コンストラクタ
    TbThreadGroup( TbSizeT max )
        : m_ThreadList(max)
    {
    }
    
    // デストラクタ
    virtual ~TbThreadGroup() 
    {}

public:

    // スレッド作成(とともにスタート)
    ThreadHandle CreateThread( const char* name , const TbFuncThread::UpdateFunc& updateFunc ) 
    {
        if( !m_ThreadList.IsFull() ){
            TbFuncThread* thread = TB_NEW TbFuncThread();
            thread->Initialize(name,updateFunc);
            m_ThreadList.PushBack(thread);
            return thread;
        }
        return ThreadHandle(NULL);
    }

    // スレッド作成(とともにスタート)
    ThreadHandle CreateThread( const TbThreadParam& param , const TbFuncThread::UpdateFunc& updateFunc ) 
    {
        if( !m_ThreadList.IsFull() ){
            TbFuncThread* thread = TB_NEW TbFuncThread();
            thread->Initialize(param,updateFunc);
            m_ThreadList.PushBack(thread);
            return ThreadHandle(thread);
        }
        return ThreadHandle(NULL);
    }

    // スレッド作成(とともにスタート)
    ThreadHandle CreateThread( const char* name , 
                           const TbFuncThread::Func& begin , 
                           const TbFuncThread::UpdateFunc& update , 
                           const TbFuncThread::Func& end ) 
    {
        if( !m_ThreadList.IsFull() ){
            TbFuncThread* thread = TB_NEW TbFuncThread();
            thread->Initialize(name,begin,update,end);
            m_ThreadList.PushBack(thread);
            return ThreadHandle(thread);
        }
        return ThreadHandle(NULL);
    }

    // スレッド作成(とともにスタート)
    ThreadHandle CreateThread( const TbThreadParam& param , 
                           const TbFuncThread::Func& begin , 
                           const TbFuncThread::UpdateFunc& update , 
                           const TbFuncThread::Func& end ) 
    {
        if( !m_ThreadList.IsFull() ){
            TbFuncThread* thread = TB_NEW TbFuncThread();
            thread->Initialize(param,begin,update,end);
            m_ThreadList.PushBack(thread);
            return ThreadHandle(thread);
        }
        return ThreadHandle(NULL);
    }

    // 全スレッド開始
    void StartAll() {
        TbArray<TbThread*>::Iterator ite = m_ThreadList.Begin();
        TbArray<TbThread*>::Iterator endIte = m_ThreadList.End();
        for( ; ite != endIte; ++ite ) {
            (*ite)->Start();
        }
    }

    // 全スレッド終了待ち
    void JoinAll()
    {
        TbArray<TbThread*>::Iterator ite = m_ThreadList.Begin();
        TbArray<TbThread*>::Iterator endIte = m_ThreadList.End();
        for( ; ite != endIte; ++ite ) {
            (*ite)->Join();
        }
    }

private:
    TbArray<TbThread*>  m_ThreadList;
};

}

#endif
