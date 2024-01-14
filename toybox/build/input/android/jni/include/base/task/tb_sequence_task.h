/*!
 * 準備、更新、終了処理を順番に行うシーケンスタスク
 * @author Miyake Shunsuke
 * @since 2011.09.06
 */

#ifndef _INCLUDED_TB_SEQUENCE_TASK_H_
#define _INCLUDED_TB_SEQUENCE_TASK_H_

#include <base/task/tb_task.h>

namespace toybox
{

class TbSequenceTask : public TbTask
{
public:
    typedef TbSequenceTask Self;
    typedef TbTask Super;
public:
    
    // コンストラクタ
    TbSequenceTask( TbUint32 lineCount , const char* name , TbTaskLine mainTaskLine );
    
    // デストラクタ
    virtual ~TbSequenceTask();
    
public:
    
    // Update開始するための準備できているか
    TbBool IsReadyForUpdate() const { 
        return m_State == STATE_READY_FOR_UPDATE || 
               m_State == STATE_UPDATE;
    }
    
    // 更新許可 (準備が完了しても、これを呼び出さないとOnUpdateは呼ばれません)
    TbResult SetEnableUpdate() { 
        if( IsReadyForUpdate() ){
            setState( STATE_UPDATE );
            return TB_S_OK;
        }else{
            m_BitArray.SetBit(FLAG_REQUEST_UPDATE);
        }
        return TB_E_FAIL;
    }
    
    // 終了中か
    TbBool IsFinishing() const { 
        return m_State == STATE_FINISHING; 
    }
    
    // 終了完了済みか
    TbBool IsFinished() const { 
        return m_State == STATE_FINISHED; 
    }
    
    // 終了リクエスト
    void RequestFinish() { 
        m_BitArray.SetBit( FLAG_REQUEST_FINISH ); 
    }
    
protected:
    
    // フラグ
    enum {
        FLAG_REQUEST_UPDATE , 
        FLAG_REQUEST_FINISH , 
    };
    
    // 状態
    enum State {
        STATE_PREPARE           , // 準備中
        STATE_READY_FOR_UPDATE  , // 更新許可待ち
        STATE_UPDATE            , // 更新中
        STATE_FINISHING         , // 終了中
        STATE_FINISHED          , // 終了した
    };
    
    enum Result {
        RESULT_PENDING , // 作業中
        RESULT_FINISH ,  // 終了
        RESULT_MAX , 
    };
    
protected:

    // メイン関数
    virtual void Main();
    
    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate() = 0;
    
    // 終了処理(終了完了したらRESULT_FINISHを返す)
    virtual Result OnFinish() { return RESULT_FINISH; };
    
    // 状態遷移コールバック
    virtual void OnChangedState( State newState , State oldState ) {};

protected:
    
    // 状態取得
    State GetState() const { 
        return m_State; 
    }
    
    // フェーズ設定
    void SetPhase( TbUint32 phase ) { 
        m_Phase = phase; 
    }
    
    // フェーズ設定
    void AddPhase( TbUint32 phase ) { 
        m_Phase += phase; 
    }
    
    // フェーズ取得
    TbUint32 GetPhase() const { 
        return m_Phase; 
    }
    
private:
    
    // 状態設定
    void setState( State state ){
        if (m_State != state)
        {
            OnChangedState(state, m_State);
            m_State = state;
            m_Phase = 0;
        }
    }
    
private:
    TbBitArray16         m_BitArray; //!< ビット配列
    State                m_State;    //!< 状態
    TbUint32             m_Phase;    //!< フェーズ
};

}

#endif
