/*!
 * シーケンス基底
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_BASE_H_
#define _INCLUDED_SEQ_BASE_H_

#include <base/task/tb_sequence_task.h>
#include <framework/debug/menu/tb_debug_menu.h>
#include <sound/tb_sound_source_handle.h>
#include "seq_types.h"

class SeqBase : public TbSequenceTask
{
public:

    typedef SeqBase Self;
    typedef TbSequenceTask Super;
    friend class SeqManager;

protected:
    
    // コンストラクタ
    SeqBase( SeqType type , const char* name )
        : Super(CMN_TASK_LINE_MAX,name,CMN_TASK_LINE_UPDATE)
        , m_Type( type )
        , m_IsEnableDeleteBgm(TB_TRUE)
        , m_Bgm(nullptr)
        , m_DebugMenu(NULL)
    {
        TbTask::AddTaskLine(CMN_TASK_LINE_DRAW,&Self::OnDraw);
        SetActiveTaskLine(CMN_TASK_LINE_DRAW,TB_FALSE);
    }
    
    // デストラクタ
    virtual ~SeqBase();

public:

    // タイプ取得
    SeqType GetType() const {
        return m_Type;
    }

    // リセット
    void Reset();

    // ポーズ設定
    void SetPause(TbBool isPause)
    {
        if (isPause) {
            SetActiveTaskLine(CMN_TASK_LINE_UPDATE, TB_FALSE);
            SetActiveTaskLine(CMN_TASK_LINE_DRAW, TB_FALSE);
        }else {
            SetActiveTaskLine(CMN_TASK_LINE_UPDATE, TB_TRUE);
            OnChangedState(GetState(), GetState());
        }
    }

public: // BGM操作

    // BGM生成
    void CreateBgm(const char* bgm );

    // BGM取得
    TbBool GetBgm();

    // BGM削除可能か
    void SetEnableDeleteBgm( TbBool isEnable ) {
        m_IsEnableDeleteBgm = isEnable;
    }

    // Bgm利用可能状態か
    TbBool IsReadyBgm() const {
        if (m_Bgm) {
            return m_Bgm->IsReady();
        }
        return TB_TRUE;
    }

    // BGM再生
    void PlayBgm();

    // BGMボリューム設定
    void SetBgmVolume(TbFloat32 volume)
    {
        if (m_Bgm){
            m_Bgm->SetVolume(volume);
        }
    }

public:

    // フェードイン
    void FadeIn( TbFloat32 sec ); 

    // フェードアウト
    void FadeOut(TbFloat32 sec);

    // BGMフェードアウト
    void FadeOutBgm(TbFloat32 sec);

    // フェード中か
    TbBool IsFading() const;

public:

    // 描画
    virtual void OnDraw() {}

    // 状態変化
    virtual void OnChangedState( State newState , State oldState )
    {
        switch( newState )
        {
        case STATE_UPDATE:
            SetActiveTaskLine(CMN_TASK_LINE_DRAW,TB_TRUE);
            break;
        case STATE_FINISHING:
            SetActiveTaskLine(CMN_TASK_LINE_DRAW,TB_FALSE);
            break;
        }
    }

    // 終了処理(終了完了したらRESULT_FINISHを返す)
    virtual Result OnFinish() TB_OVERRIDE;

public:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent ) {}

    // デバッグメニュー生成
    TbDebugMenu* CreateDebugMenu( TbDebugMenu* parent , const char* name ) {
        m_DebugMenu = parent->CreateItemMenu(name);
        return m_DebugMenu;
    }

    // デバッグメニュー取得
    TbDebugMenu* GetDebugMenu() {
        return m_DebugMenu;
    }

protected:

    // リセット
    virtual void OnReset() {}

private:
    
    SeqType                 m_Type;
    TbBool                  m_IsEnableDeleteBgm;
    TbSoundSourceHandle*    m_Bgm;

private:

    TbDebugMenu*    m_DebugMenu;

};

#endif
