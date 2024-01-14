/*!
 * ルート
 * @author teco
 */

#ifndef _INCLUDED_ROOT_H_
#define _INCLUDED_ROOT_H_

#include <base/util/tb_singleton.h>
#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <base/io/resource/tb_resource.h>
#include <script/lua/tb_script_lua.h>
#include <sound/tb_sound_manager.h>
#include <crayon/input/cr_input.h>
#include <base/io/archive/tb_archive.h>
#include <base/io/archive/tb_file_mapper_archive.h>

#include "common/cmn_global_data.h"
#include "sequence/seq_manager.h"
#include "ui/ui_manager.h"

class NetInputServer;

class Root : public toybox::TbSingletonRef<Root>
{
public:

    enum Texture
    {
        TEX_COMMON ,
        TEX_MAX
    };

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
    TbTexture* GetCommonTexture( Texture tex ) {
        TbResourceDataTexture* data = m_ResTexture[tex].GetData();
        return data ? data->GetTexture() : NULL;
    }

    TbSpriteFixedMaterial* GetSpriteMatrial(Texture tex)
    {
        return m_SpriteMaterial[tex];
    }

    // グローバルデータ取得
    CmnGlobalData& GetGlobalData() {
        return m_GlobalData;
    }

public:

    // 画面を半透明にする
    void SetEnableTranslucent( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_TRANSLUCENT,isEnable);
    }

private:

    enum Flag
    {
        FLAG_INITIALIZE , 
        FLAG_ENABLE_TRANSLUCENT
    };

private:

    TbBitArray32            m_BitArray;   // ビット配列

    SeqManager              m_SeqManager; // シーケンス管理
    TbResourceHandleTexture m_ResTexture[TEX_MAX];
    TbSpriteFixedMaterial*  m_SpriteMaterial[TEX_MAX];
    TbScriptLuaManager*     m_ScriptManager;
    TbSoundManager*         m_SoundManager;

    TbResourceHandleTexture m_FontTexture;
    TbFontSjis*             m_Font;
    TbFontBuffer2d*         m_FontBuffer;
    TbFontBuffer2d*         m_LocalFontBuffer;

    CmnGlobalData           m_GlobalData;

    TbResourceHandleTexture m_DebugFontTexture;
    TbFontSjis*             m_DebugFont;  // デバッグ用
    TbFontBuffer2d*         m_DebugFontBuffer;

    UiManager               m_UiManager;

private: // アーカイブ

    TbFileMapperArchive     m_FileMapperArchive;
    TbArchive               m_Archive;

};

#endif
