/*!
 * スレッドクラス
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_H_
#define _INCLUDED_TB_THREAD_H_

#include <base/thread/tb_thread_types.h>
#include <base/string/tb_static_string.h>
#include "./depend/tb_thread_depend.h"

namespace toybox
{

/*!
 * スレッドクラス
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
class TbThread
{
public:
    
    // コンストラクタ
    explicit TbThread();
    
    // デストラクタ
    virtual ~TbThread();

public:

    // 初期化
    void Initialize( const char* name );

    // 初期化
    void Initialize( const TbThreadParam& param );
    
    // 開始
    TbResult Start();
    
    // 終了待ち
    TbResult Join();
    
    // スリープ
    TbResult Sleep( TbUint32 msec );
    
    // プロセッサ割当
    TbResult SetProcessor( TbSint32 processor );

    // 優先順位設定
    TbResult SetPriority( TbThreadPriority priority );

    // 終了をリクエスト(実際終了できるかどうかは派生クラス次第)
    void RequestFinish() {
        m_IsRequestdFinish = TB_TRUE;
    }

    // 終了がリクエストされている
    TbBool IsRequestedFinish() const {
        return m_IsRequestdFinish;
    }

public: // 依存クラス
    
    // 依存クラス取得
    TbThreadDepend& GetDepend() { 
        return m_Depend; 
    }

    // 依存クラス取得
    const TbThreadDepend& GetDepend() const { 
        return m_Depend; 
    }

protected:

    // スレッド開始処理
    virtual void Begin(){};

    // スレッドループ処理(Falseを返したら終了)
    virtual TbThreadUpdateResult Update(){ 
        return TB_THREAD_UPDATE_RESULT_FINISH; 
    };

    // スレッド終了処理
    virtual void End(){};

private:

    enum State
    {
        STATE_PRE   , // 初期化前
        STATE_READY , // スレッド開始前
        STATE_RUN   , // スレッド処理中 
        STATE_END   , // スレッド終了
    };

private:

    // 状態設定
    void setState( State state ) {
        m_State = state;
    }

private:

    TbAtomicU32         m_State;            // 状態
    TbStaticString64    m_Name;             // 名前
    TbSint32            m_Processor;        // 利用するプロセッサ
    TbThreadPriority    m_Priority;         // 優先順位
    TbUint32            m_StackSize;        // スタックサイズ
    TbBool              m_IsRequestdFinish; // 終了がリクエストされている
    TbThreadDepend      m_Depend;           // 依存クラス

private:
    friend class TbThreadDepend; // 依存クラスからはアクセス可能
};

}

#endif
