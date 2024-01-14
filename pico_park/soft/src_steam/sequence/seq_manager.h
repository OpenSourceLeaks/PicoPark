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
#include "common/cmn_fade.h"

#define SEQ_IS_ENABLE_DEBUG_MENU (1) // TB_IS_DEBUG

namespace toybox
{
class TbSoundSource;
}

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
    TbResult RequestSequence( SeqType type , TbSint32 arg1 = 0 );
    
    // 現在のシーケンス取得
    SeqBase* GetCurrentSequence() const {
        return m_CurrentSeq;
    }

public:

    // BGM生成
    toybox::TbSoundSource* CreateBgm( const char* bgm );

    // BGM取得
    toybox::TbSoundSource* GetBgm();

    // BGM破棄
    void DeleteBgm();

public:

    // フェードイン
    void FadeIn( TbFloat32 sec );

    // フェードアウト
    void FadeOut( TbFloat32 sec );

    // フェード中か
    TbBool IsFading() const;

public: // パラメータ

    // ステージ名取得
    const char* GetStageName( TbSint32 stageIndex ) const;

    // ステージタイプ取得
    CmnStageType GetStageType( TbSint32 stageIndex ) const {
        return m_StageType[stageIndex];
    }

    // ステージ番号取得
    TbSint32 GetStageIndex( const char* name ) const;

    // ステージ数取得
    TbSint32 GetStageCount() const{
        return m_StageCount;
    }

    // 有効なステージ数
    TbSint32 GetActiveStageCount() const {
        return  m_ActiveStageCount;
    }

public: // デバッグ

    // デバッグメニュー作成
    void CreateDebugMenu( TbDebugMenu* parent = NULL );
    
public:
    
    // 更新
    void Update();

private:

    static const TbUint32 STAGE_MAX = 256;

private:

    // シーケンス生成
    SeqBase* createSeq( SeqType type , TbSint32 arg );

private:

    // スタート
    void onClickedStart();

private:

    SeqBase*            m_CurrentSeq;           // 現在のシーケンス
    SeqBase*            m_RequestSeq;           // リクエスト中のシーケンス
    TbUint32            m_StageCount;           // ステージ最大数
    TbUint32            m_ActiveStageCount;     // 有効なステージ数
    TbStaticString32    m_StageName[STAGE_MAX]; // ステージ名
    CmnStageType        m_StageType[STAGE_MAX]; // ステージ名
    TbUint32                    m_BgmRefCount;
    toybox::TbSoundSource*      m_Bgm;                  // BGM
    CmnFade*                    m_Fade;

private:

#if SEQ_IS_ENABLE_DEBUG_MENU
    TbDebugMenu*            m_DebugMenu;
    TbDebugMenuItemAlias*   m_TypeItem;
    TbDebugMenuItemS32*     m_ArgItem;
    TbDebugMenuItemButton*  m_StartButtonItem;
#endif

};


#endif
