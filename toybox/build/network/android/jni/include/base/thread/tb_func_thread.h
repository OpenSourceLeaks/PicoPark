/*!
 * 関数を呼び出すスレッド
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_FUNC_THREAD_H_
#define _INCLUDED_TB_FUNC_THREAD_H_

#include <base/thread/tb_thread.h>
#include <base/util/tb_function.h>
#include <base/util/tb_bind.h>

namespace toybox
{

/*!
 * 関数スレッドクラス
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
class TbFuncThread : public TbThread
{
public:
    typedef TbThread Super;
    typedef TbStaticFunction32<void ()> Func;
    typedef TbStaticFunction32<TbThreadUpdateResult ()> UpdateFunc;
public:
    
    // コンストラクタ
    TbFuncThread()
        : Super()
    {
    }

    // デストラクタ
    virtual ~TbFuncThread() {}

public:

    // 初期化
    void Initialize( const char* name , const UpdateFunc& updateFunc ) {
        Super::Initialize(name);
        m_UpdateFunc = updateFunc;
    }
    
    // 初期化
    void Initialize( const TbThreadParam& param , const UpdateFunc& updateFunc ) {
        Super::Initialize(param);
        m_UpdateFunc = updateFunc;
    }

    // 初期化
    void Initialize( const char* name , const Func& begin , const UpdateFunc& update , const Func& end ) {
        Super::Initialize(name);
        m_BeginFunc = begin;
        m_UpdateFunc = update;
        m_EndFunc = end;
    }

    // 初期化
    void Initialize( const TbThreadParam& param , const Func& begin , const UpdateFunc& update , const Func& end ) {
        Super::Initialize(param);
        m_BeginFunc = begin;
        m_UpdateFunc = update;
        m_EndFunc = end;
    }

protected:

    // スレッド開始処理
    virtual void Begin(){
        if( m_BeginFunc.IsValid() ){
            m_BeginFunc();
        }
    };

    // スレッドループ処理
    virtual TbThreadUpdateResult Update(){ 
        if (IsRequestedFinish()) {
            return TB_THREAD_UPDATE_RESULT_FINISH;
        }
        if( m_UpdateFunc.IsValid() ){
            return m_UpdateFunc();
        }
        return TB_THREAD_UPDATE_RESULT_FINISH;
    };

    // スレッド終了処理
    virtual void End(){
        if( m_EndFunc.IsValid() ){
            m_EndFunc();
        }
    }

private:
    Func       m_BeginFunc;
    UpdateFunc m_UpdateFunc;
    Func       m_EndFunc;
};

}

#endif
