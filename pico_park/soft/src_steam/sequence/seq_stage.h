/*!
 * ステージシーケンス
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_STAGE_H_
#define _INCLUDED_SEQ_STAGE_H_

#include "seq_base.h"
#include "common/stage/stage.h"
#include "sound/tb_sound_source.h"
#include "common/ui/ui_pause_window.h"

class SeqStage : public SeqBase
{
public:

    typedef SeqStage Self;
    typedef SeqBase Super;

    enum StageType
    {
        STAGE_TYPE_NONE , 
        STAGE_TYPE_SIDE_ACTION       , // 横向きアクション
        STAGE_TYPE_PUZZLE_TETRIS     , // テトリス
        STAGE_TYPE_PUZZLE_TETRIS3    , // テトリス
        STAGE_TYPE_PUZZLE_DR         , // ドクターマリオ
        STAGE_TYPE_BREAKOUT          , // ブロック崩し

        STAGE_TYPE_PUZZLE_VS_TETRIS  , // 対戦テトリス
        STAGE_TYPE_PUZZLE_VS_TETRIS3  ,// 対戦テトリス3
        STAGE_TYPE_PUZZLE_VS_DR      , // 対戦ドクターマリオ
        STAGE_TYPE_MAX ,
    };

public:

    // コンストラクタ
    SeqStage( TbSint32 id , const char* scriptName );

    // デストラクタ
    virtual ~SeqStage();

public:

    // ラストステージか
    void  SetLastStage() {
        m_BitArray.SetBit(FLAG_LAST_STAGE);
    }

public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();
    
    // 描画関数
    virtual void OnDraw();

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent ) {
        if( m_Stage ) {
            m_Stage->OnCreateDebugMenu(parent);
        }
    }

private:

    enum Flag
    {
        FLAG_PAUSE ,
        FLAG_LAST_STAGE , 
        FLAG_BEST_TIME ,
        FLAG_MAX
    };

    enum PhaseType
    {
        PHASE_FIRST , 
        PHASE_STAGE = 10 , 
        PHASE_CLEAR = 20 ,
        PHASE_GAMEOVER = 30 , 
    };

    struct Phase
    {
        Phase() : val(0) , timer(0.0f) {}

        void operator=(TbSint32 v) {
            val = v;
            timer = 0.0f;
        }

        void operator++(int) {
            ++val;
            timer = 0.0f;
        }

        void operator++() {
            ++val;
            timer = 0.0f;
        }

        TbSint32  val;
        TbFloat32 timer;
    };

private:

    // ポーズ設定
    void setPause( TbBool isPause , TbUint32 playerIndex = 0 );

    // 更新
    void updateFirst();

    // ステージセットアップ
    void setupStage( const char* scriptName );

    // ステージ更新
    void updateStage();

    // ポーズ画面の選択時
    void onDecidedPause( UiPauseWindow::Menu result );

private:

    TbBitArray32            m_BitArray;
    StageType               m_Type;
    Stage*                  m_Stage;
    TbSint32                m_Id; // ステージID
    Phase                   m_Phase;
    UiPauseWindow           m_Pause;
    CmnSpritePlayerMaterial m_PlayerMaterial;

};

#endif
