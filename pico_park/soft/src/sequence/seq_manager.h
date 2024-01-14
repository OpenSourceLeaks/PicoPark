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
#include <sound/tb_sound_source_fade.h>
#include <sound/tb_sound_source_handle.h>

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
    
    // シーケンス遷移リクエスト
    TbResult RequestSequence( SeqType type , TbSint32 arg1 = 0 );

    // シーケンス遷移リクエスト用ユーザーデータ設定
    void SetRequestUserDataU32( TbUint32 param , TbUint32 index = 0 );

    // シーケンス遷移リクエスト用ユーザーデータ取得
    TbUint32 GetRequestUserDataU32(TbUint32 index ) const;
    
    // 現在のシーケンス取得
    SeqBase* GetCurrentSequence() const {
        return m_CurrentSeq;
    }

public:

    // キャッシュ用シーケンス追加
    void     AddCacheSeq(SeqBase* sequence);

    // キャッシュシーケンスクリア
    void     ClearCacheSeq();

    // キャッシュ用シーケンスか
    TbBool    IsCacheSeq(const SeqBase* sequence) const;

public:

    // BGM生成
    toybox::TbSoundSourceHandle* CreateBgm( const char* bgm );

    // BGM取得
    toybox::TbSoundSourceHandle* GetBgm();

    // BGM音量設定
    void SetBgmVolume( TbFloat32 volume );

    // BGM破棄
    void DeleteBgm();

public:

    // フェードイン
    void FadeIn( TbFloat32 sec );

    // フェードアウト
    void FadeOut( TbFloat32 sec );

    // BGMフェードアウト
    void FadeOutBgm( TbFloat32 sec );

    // フェード中か
    TbBool IsFading() const;

public: // パラメータ

    // ステージ名取得
    TbStaticString32 GetStageName( TbSint32 stageIndex ) const;

    // ステージフォルダ
    const char* GetStageDir(TbSint32 stageIndex ) const;

    // ステージタイプ取得
    CmnStageType GetStageType( TbSint32 stageIndex ) const {
        if(stageIndex < static_cast<TbSint32>(m_StageCount)){
            return m_StageInfo[stageIndex].type;
        }
        return CMN_STAGE_TYPE_BEST_TIME;
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

    struct StageInfo
    {
        StageInfo() : type(CMN_STAGE_TYPE_BEST_TIME) , manyBorder(0) {}
        TbStaticString32 name;
        TbStaticString32 dir;
        TbUint32         manyBorder;
        TbStaticString32 manyName;
        CmnStageType     type;
    };

    struct UserData
    {
        UserData() : paramU32(0) , paramF32(0.0f){}
        TbUint32   paramU32;
        TbFloat32  paramF32;
    };

private:

    // シーケンス生成
    SeqBase* createSeq( SeqType type , TbSint32 arg );
    
private:

    // スタート
    void onClickedStart();

private:

    SeqBase*            m_CurrentSeq;           // 現在のシーケンス
    SeqBase*            m_RequestSeq;           // リクエスト中のシーケンス
    UserData            m_RequestUserData;
    TbUint32            m_StageCount;           // ステージ最大数
    TbUint32            m_ActiveStageCount;     // 有効なステージ数
    StageInfo           m_StageInfo[STAGE_MAX]; // ステージ情報
    TbUint32                    m_BgmRefCount;
    toybox::TbSoundSourceHandle* m_Bgm;                  // BGM
    toybox::TbSoundSourceFade   m_BgmFade;
    CmnFade*                    m_Fade;

private:

    SeqBase*            m_CacheSeq[SEQ_MAX];

private:

#if SEQ_IS_ENABLE_DEBUG_MENU
    TbDebugMenu*            m_DebugMenu;
    TbDebugMenuItemAlias*   m_TypeItem;
    TbDebugMenuItemS32*     m_ArgItem;
    TbDebugMenuItemButton*  m_StartButtonItem;
#endif

};


#endif
