/*!
 * シーケンス基底
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_BASE_H_
#define _INCLUDED_SEQ_BASE_H_

#include <base/task/tb_sequence_task.h>
#include <framework/debug/menu/tb_debug_menu.h>
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
        , m_DebugMenu(NULL)
    {
        TbTask::AddTaskLine(CMN_TASK_LINE_DRAW,&Self::OnDraw);
        SetActiveTaskLine(CMN_TASK_LINE_DRAW,TB_FALSE);
    }
    
    // デストラクタ
    virtual ~SeqBase() {
        if( m_DebugMenu ){
            m_DebugMenu->Suicide();
        }
    }

public:

    // タイプ取得
    SeqType GetType() const {
        return m_Type;
    }

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

private:
    
    SeqType         m_Type;

private:

    TbDebugMenu*    m_DebugMenu;

};

#endif
