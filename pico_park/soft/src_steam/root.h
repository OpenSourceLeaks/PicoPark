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
#include "system/sys_giant_input.h"
#include "base/io/archive/tb_archive.h"
#include "base/io/archive/tb_file_mapper_archive.h"
#include "system/shader/sys_shader_manager.h"
#include "ui/ui_manager.h"

class NetInputServer;

class CmnSpritePlayerMaterial;

class Root : public toybox::TbSingletonRef<Root>
{
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

    // 共通テクスチャ取得
    TbTexture* GetCommonTexture() {
        TbResourceDataTexture* data = m_ResTexture.GetData();
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

public: // セーブ

    // ユーザーデータロード
    void LoadUserData();

    // ユーザーデータセーブ
    void SaveUserData();

private:

    enum Flag
    {
        FLAG_INITIALIZE , 
        FLAG_ENABLE_TRANSLUCENT
    };

private:

    // 初期化待ち
    TbBool isInitialized();

private:

    TbBitArray32            m_BitArray;   // ビット配列

    SeqManager              m_SeqManager; // シーケンス管理
    TbUint32                m_PlayerCount;
    TbResourceHandleTexture m_ResTexture;
    TbScriptLuaManager*     m_ScriptManager;
    TbSoundManager*         m_SoundManager;
    TbSint32                m_SoundVolumeLevel;
    NetInputServer*         m_NetInputServer;

    TbResourceHandleTexture m_FontTexture;
    TbFontSjis*             m_Font;
    TbFontBuffer2d*         m_FontBuffer;
    TbFontBuffer2d*         m_LocalFontBuffer;

    CmnGlobalData           m_GlobalData;

    TbResourceHandleTexture m_DebugFontTexture;
    TbFontSjis*             m_DebugFont;  // デバッグ用
    TbFontBuffer2d*         m_DebugFontBuffer;

    UiManager               m_UiManager;
    SysGiantInput           m_GiantInput;
    TbFloat32               m_IdleSec;
    SysShaderManager        m_ShaderManager;
    CmnSpritePlayerMaterial* m_PlayerMaterial;
    CmnWhiteSprite*          m_TransparentSprite;

private: // アーカイブ

    TbFileMapperArchive     m_FileMapperArchive;
    TbArchive               m_Archive;

};

#endif
