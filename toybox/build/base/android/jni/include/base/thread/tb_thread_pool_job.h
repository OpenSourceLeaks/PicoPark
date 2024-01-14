/*!
 * スレッドプール用ジョブ
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_POOL_JOB_H_
#define _INCLUDED_TB_THREAD_POOL_JOB_H_

#include <base/thread/tb_thread_types.h>
#include <base/system/tb_atomic.h>
#include <base/util/tb_ref_counter.h>

namespace toybox
{

/*!
 * スレッドプールクラス
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
class TbThreadPoolJob : public TbRefCounter
{
public:
    
    // コンストラクタ
    explicit TbThreadPoolJob( const char* name );

protected:

    // デストラクタ
    virtual ~TbThreadPoolJob();

public:

    // 開始
    TbResult Start();

    // 終了待ち
    void Join();

    // 開始可能状態か
    TbBool IsReady() const {
        return m_State.Load() == STATE_READY;
    }

    // シーケンシャルジョブである
    void SetSequential( ) {
        m_BitArray.SetBit(FLAG_SEQUENTIAL,TB_TRUE);
    }

    // シーケンシャルジョブかどうか
    TbBool IsSequential() const {
        return m_BitArray.Test(FLAG_SEQUENTIAL);
    }

public:

    // 実行(ThreadPoolから呼ばれる)
    void ExecuteJob() {
        Execute();
        m_State.Store(STATE_READY); // 終了
    };

protected:

    // 実行(オーバーライド用)
    virtual void Execute() = 0;

private:

    enum State {
        STATE_READY , // 開始可能
        STATE_BUSY  , // ジョブキューにつまれている
    };

    enum Flag {
        FLAG_SEQUENTIAL , // ワーカースレッド内で並列には処理されない
        FLAG_MAX 
    };

private:

    TbStaticString64 m_Name;        // 名前
    TbBitArray32     m_BitArray;    // ビット配列
    TbAtomicU32      m_State;       // 状態

};

}

#endif
