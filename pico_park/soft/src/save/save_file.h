/*!
 * セーブデータ 
 */

#ifndef _INCLUDED_SAVE_FILE_H_
#define _INCLUDED_SAVE_FILE_H_

#include "save_data_struct.h"
#include <base/save/tb_save_file.h>
#include <graphics/sprite/tb_sprite_all.h>

class SaveFile : public TbSingletonRef<SaveFile>
{
public:

    // コンストラクタ
    SaveFile();

    // デストラクタ
    ~SaveFile();

public:

    // 初期化
    void Initialize();

    // セーブ
    void Save();

    // ロード
    void Load();

    // 読み込み済み
    TbBool IsLoaded() const {
        return m_IsLoaded;
    }

    // ビジー状態か
    TbBool IsBusy() const;

public:

public:

    // プレイ回数設定
    void SetPlayCount(TbUint32 stage, TbUint32 playerCount, TbUint32 playCount);

    // プレイ回数カウントアップ
    void IncrementPlayCount(TbUint32 stage, TbUint32 playerCount);

    // プレイ回数取得
    TbUint32 GetPlayCount(TbUint32 stage, TbUint32 playerCount) const;

    // クリア回数設定
    void SetClearCount(TbUint32 stage, TbUint32 playerCount, TbUint32 clearCount);

    // クリア回数カウントアップ
    void IncrementClearCount(TbUint32 stage, TbUint32 playerCount);

    // クリア回数取得
    TbUint32 GetClearCount(TbUint32 stage, TbUint32 playerCount) const;

    // クリア回数取得(一番良いの)
    TbUint32 GetClearCount(TbUint32 stage) const;

    // クリア回数取得
    TbUint32 GetTotalClearCount(TbUint32 stage) const;

    // ベストタイム設定
    TbBool SetBestMillSec(TbUint32 stage, TbUint32 playerCount, TbUint32 ms);

    // ベストタイム取得
    TbUint32 GetBestMillSec(TbUint32 stage, TbUint32 playerCount) const;

    // ベストタイム設定
    TbBool SetHiscore(TbUint32 stage, TbUint32 playerCount, TbUint32 score);

    // ベストタイム取得
    TbUint32 GetHiscore(TbUint32 stage, TbUint32 playerCount) const;

    // ジャンプボタン切り替えか
    TbBool IsChangeJumpButton() const;

    // ジャンプボタン切り替えか
    void SetChangeJumpButton( TbBool isChangeJumpButton );

    // 背景カラー取得
    TbUint32 GetBgColorType() const;

    // 背景カラー設定
    void SetBgColorType( TbUint32 bgColor );

    // サウンドボリュームレベル設定
    void SetSoundBgmVolumeLevel(TbSint32 level);

    // サウンドボリュームレベル取得
    TbSint32 GetSoundBgmVolumeLevel() const;

    // プレイヤー番号表示
    void SetVisiblePlayerNumber( TbBool isVisibile );

    // プレイヤー番号表示
    TbBool IsVisiblePlayerNumber() const;

public:

    // 定期処理
    void Update();

    // アイコン描画
    void DrawIcon();

public:

    // データ取得
    const SaveData& GetData() const {
        return m_Data;
    }

private:

    enum
    {
        PHASE_IDLE , 
        PHASE_LOAD = 10 , 
        PHASE_SAVE = 20
    };

private:

    // 読み込み
    void load();

private:

    TbSint32        m_Phase;
    TbBool          m_IsDirty;
    TbBool          m_IsLoaded;
    SaveData        m_Data;
    TbSaveFile*     m_SaveFile;
    TbSprite                m_IconSprite;
    TbSpriteFixedMaterial   m_IconSpriteMaterial;   // スプライト用マテリアル
    TbFloat32               m_IconSpriteTimer;

};

#endif
