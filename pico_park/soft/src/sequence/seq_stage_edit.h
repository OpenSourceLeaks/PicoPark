/*!
 * ステージエディットシーケンス
 * @author teco
 */

#ifndef _INCLUDED_SEQ_STAGE_EDIT_H_
#define _INCLUDED_SEQ_STAGE_EDIT_H_

#include "seq_base.h"
#include <crayon/stage/cr_stage.h>
#include "sound/tb_sound_source.h"
#include "edit/edit_mode_manager.h"

class SideActionStage;

class SeqStageEdit : public SeqBase
{
public:

    typedef SeqStageEdit Self;
    typedef SeqBase Super;

public:

    // コンストラクタ
    SeqStageEdit( TbSint32 id );

    // デストラクタ
    virtual ~SeqStageEdit();

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
        FLAG_MAX
    };

private:

    // ステージセットアップ
    void setupStage( const char* scriptName , const char* dirName );

    // ステージ更新
    void updateStage();

    // ステージセーブ
    void saveStage();

private:

    TbBitArray32            m_BitArray;
    CrStage*                m_Stage;
    TbSint32                m_Id; // ステージID
    EditModeManager         m_EditModeManager;

};

#endif
