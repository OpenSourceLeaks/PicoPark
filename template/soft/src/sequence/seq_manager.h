/*!
 * ルート
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_MANAGER_H_
#define _INCLUDED_SEQ_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <base/task/tb_sequence_task.h>
#include <framework/debug/menu/tb_debug_menu.h>
#include "seq_base.h"

#define SEQ_IS_ENABLE_DEBUG_MENU (1) // TB_IS_DEBUG

class SeqManager : public toybox::TbSingletonRef<SeqManager>
{
public:
    typedef SeqManager Self;
public:
    
    // コンストラクタ
    SeqManager();
    
    // デストラクタ
    ~SeqManager();
    
public:

    // 初期化
    void Initialize();

    // リセット
    void Reset();
    
    // 現在のシーケンス設定
    TbResult RequestSequence( SeqType type , TbSint32 arg1 = 0 , TbSint32 arg2 = 0 );
    
    // 現在のシーケンス取得
    SeqBase* GetCurrentSequence() const {
        return m_CurrentSeq;
    }

public: // デバッグ

    // デバッグメニュー作成
    void CreateDebugMenu( TbDebugMenu* parent = NULL );
    
public:
    
    // 更新
    void Update();

private:

    static const TbUint32 ARG_MAX = 2;

private:

    // シーケンス生成
    SeqBase* createSeq( SeqType type , TbSint32 arg1 , TbSint32 arg2 );

private:

    // スタート
    void onClickedStart();

private:

    SeqBase*            m_CurrentSeq;           // 現在のシーケンス
    SeqBase*            m_RequestSeq;           // リクエスト中のシーケンス

private:

#if SEQ_IS_ENABLE_DEBUG_MENU
    TbDebugMenu*            m_DebugMenu;
    TbDebugMenuItemAlias*   m_TypeItem;
    TbDebugMenuItemS32*     m_ArgItem[ARG_MAX];
    TbDebugMenuItemButton*  m_StartButtonItem;
#endif

};


#endif
