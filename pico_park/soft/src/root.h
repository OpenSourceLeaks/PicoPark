/*!
 * ルート
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ROOT_H_
#define _INCLUDED_ROOT_H_

#include <base/util/tb_singleton.h>
#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>
#include <graphics/resource/tb_resource_texture.h>
#include <base/io/resource/tb_resource.h>
#include <script/lua/tb_script_lua.h>
#include <sound/tb_sound_manager.h>
#include "common/cmn_global_data.h"
#include "common/cmn_simple_sprite.h"
#include "sequence/seq_manager.h"
#include "world_manager.h"
#include "base/io/archive/tb_archive.h"
#include "base/io/archive/tb_file_mapper_archive.h"
#include "system/shader/sys_shader_manager.h"
#include "ui/ui_manager.h"
#include "battle/battle_manager.h"
#include "save/save_file.h"
#include <crayon/input/cr_giant_input.h>
#include <crayon/input/cr_remote_input_server.h>

class CmnSpritePlayerMaterial;

class Root : public toybox::TbSingletonRef<Root>
{
public:

    static const TbFloat32 REPEAT_SEC;
    static const TbFloat32 REPEAT_FIRST_SEC;

public:

    // コンストラクタ
    Root();

    // デストラクタ
    ~Root();

public:

    // 更新
    void Update();

    // 描画
    void Draw();

public:

    // フォント
    TbFontBuffer2d* GetFont() {
        return m_FontBuffer;
    }

    // ローカルフォント(使う側でFlushを呼んでください)
    TbFontBuffer2d* GetLocalFont() {
        return m_LocalFontBuffer;
    }

    // デバッグフォント取得
    TbFontBuffer2d* GetDebugFont() {
        return m_DebugFontBuffer;
    }

    // 共通テクスチャ取得
    TbTexture* GetCommonTexture() {
        TbResourceDataTexture* data = m_ResTexture.GetData();
        return data ? data->GetTexture() : NULL;
    }

    // 共通テクスチャ取得
    TbTexture* GetAutoSaveTexture() {
        TbResourceDataTexture* data = m_ResAutoSaveTexture.GetData();
        return data ? data->GetTexture() : NULL;
    }

    // 共通テクスチャ取得
    TbTexture* GetPlayStyleTexture() {
        TbResourceDataTexture* data = m_ResPlayStyleTexture.GetData();
        return data ? data->GetTexture() : NULL;
    }


    CmnSpritePlayerMaterial* GetPlayerMaterial() {
        return m_PlayerMaterial;
    }

    // プレイヤー数設定
    void SetPlayerCount( TbUint32 count ) {
#if !CMN_ENABLE_BUILD_GIANT_INPUT
        m_PlayerCount = count;
#endif
    }

    // プレイヤー数取得
    TbUint32 GetPlayerCount() const {
        return m_PlayerCount;
    }

    // グローバルデータ取得
    CmnGlobalData& GetGlobalData() {
        return m_GlobalData;
    }

    // サウンドボリュームレベル設定
    void SetSoundVolumeLevel( TbSint32 level );

    // サウンドボリュームレベル取得
    TbSint32 GetSoundVolumeLevel() const {
        return m_SoundVolumeLevel;
    }

    // フルスクリーン設定
    void SetFullscreen( TbBool isFullscreen );

    // フルスクリーン取得
    TbBool IsFullscreen() const;

    // ゲーム上の矩形を画面上の矩形サイズに変換
    TbBool ConvertGameRectToScreenRect( TbRectF32* dst , const TbRectF32& src );

    // 強制的にアイドル時間をリセット
    void ResetIdleSec() {
        m_IdleSec = 0.0f;
    }

public:

    // 画面を半透明にする
    void SetEnableTranslucent( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_TRANSLUCENT,isEnable);
    }

    // 画面フラッシュ
    void Flash( const TbColor& color = TbColor::White() );

public: // セーブ

    // ユーザーデータロード
    void LoadUserData();

    // ユーザーデータセーブ
    void SaveUserData();

    // セーブがビジーか
    TbBool IsBusyUserData() const {
        return m_SaveFile.IsBusy();
    }

public: 

    // 自動テスト
    TbBool IsAutoTest() const {
        return m_BitArray.Test(FLAG_DEBUG_AUTO_TEST);
    }

private:

    enum Flag
    {
        FLAG_INITIALIZE , 
        FLAG_ENABLE_TRANSLUCENT,
        FLAG_FLASH ,

        FLAG_DEBUG_AUTO_TEST, // 自動テスト
    };

private:

    // 初期化待ち
    TbBool isInitialized();

    // 全ワールド強制クリア
    void clearAllWorldDebug();

private:

    TbBitArray32            m_BitArray;   // ビット配列

    SeqManager              m_SeqManager; // シーケンス管理
    WorldManager            m_WorldManager;
    BattleManager           m_BattleManager;
    TbUint32                m_PlayerCount;
    TbResourceHandleTexture m_ResTexture;
    TbResourceHandleTexture m_ResAutoSaveTexture;
    TbResourceHandleTexture m_ResPlayStyleTexture;
    TbScriptLuaManager*     m_ScriptManager;
    TbSoundManager*         m_SoundManager;
    TbSint32                m_SoundVolumeLevel;
#if CR_REMOTE_INPUT_IS_ENABLE
    CrRemoteInputServer*    m_CrRemoteInputServer;
#endif
    TbResourceHandleTexture m_FontTexture;
    TbFontSjis*             m_Font;
    TbFontBuffer2d*         m_FontBuffer;
    TbFontBuffer2d*         m_LocalFontBuffer;

    CmnGlobalData           m_GlobalData;
    SaveFile                m_SaveFile;

    TbResourceHandleTexture m_DebugFontTexture;
    TbFontSjis*             m_DebugFont;  // デバッグ用
    TbFontBuffer2d*         m_DebugFontBuffer;

    UiManager                m_UiManager;
    CrGiantInput             m_GiantInput;
    TbFloat32                m_IdleSec;
    SysShaderManager         m_ShaderManager;
    CmnSpritePlayerMaterial* m_PlayerMaterial;
    CmnWhiteSprite*          m_TransparentSprite;
    CmnWhiteSprite*          m_FlashSprite;

private: // アーカイブ

    TbFileMapperArchive     m_FileMapperArchive;
    TbFileMapperArchive     m_FileMapperShaderArchive;
    TbArchive               m_Archive;

};

#endif
