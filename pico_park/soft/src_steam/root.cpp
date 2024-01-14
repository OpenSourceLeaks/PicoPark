/*!
 * ルート
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "root.h"

#include "common/cmn_util.h"
#include "system/sys_input.h"
#include "network/net_input_server.h"
#include "common/material/cmn_sprite_player_material.h"

#include <base/task/tb_task_manager.h>
#include <base/io/resource/tb_resource_all.h>
#include <base/io/tb_file_manager.h>
#include <base/time/tb_tick.h>
#include <base/time/tb_time.h>
#include <base/util/tb_bind.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <framework/debug/menu/tb_debug_menu_all.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/window/tb_window.h>
#include <graphics/util/tb_screen_shot.h>
#include <input/tb_pad.h>
#include <input/tb_mouse.h>
#include <input/tb_keyboard.h>
#include <input/tb_input_setting.h>
#include <base/io/stream/tb_stream_text_writer.h>
#include <base/io/stream/tb_file_stream.h>
#include <base/io/stream/tb_memory_stream.h>
#include <base/save/tb_save_file.h>
#include <base/system/tb_debug.h>

#define IS_ENABLE_MEMORY_ARCHIVE 0 || (1 && !TB_IS_DEBUG)

#if IS_ENABLE_MEMORY_ARCHIVE
#if CMN_ENABLE_BUILD_ALPHA
#include "outbin_alpha.h"
#elif TB_PLATFORM_IS_NX
#include "outbin_nx.h"
#else
#include "outbin.h"
#endif
#endif

namespace
{
    static const TbFontCharInfoSjis FONT_SJIS[] =
    {
        #include "font_info_famania.h"
    };
    static const TbFontCharInfoSjis DEBUG_FONT_SJIS[] =
    {
        #include "font_info.h"
    };
    void finishSystem()
    {
        TbFramework::FinishMainLoop();
    }
}

static TbBool s_IsEnableDrawGuid = TB_FALSE;
#if CMN_ENABLE_BUILD_OLD
static const TbUint32 DEFAULT_FONT_SIZE = 32;
#else
static const TbUint32 DEFAULT_FONT_SIZE = 48;
#endif

/*!
 * 初期化
 * @author teco
 */
Root::Root()
    : m_SeqManager()
    , m_PlayerCount(CMN_PLAYER_MAX)
    , m_Font(nullptr)
    , m_FontBuffer(nullptr)
    , m_DebugFont(nullptr)
    , m_DebugFontBuffer(nullptr)
    , m_ScriptManager(nullptr)
    , m_SoundManager(nullptr)
    , m_SoundVolumeLevel(CMN_SOUND_VOLUME_LEVEL_DEFAULT)
    , m_NetInputServer(nullptr)
    , m_IdleSec(0.0f)
    , m_PlayerMaterial(nullptr)
    , m_TransparentSprite(nullptr)
{
#if CMN_ENABLE_BUILD_GIANT_INPUT
    m_PlayerCount = 1;
#endif
#if IS_ENABLE_MEMORY_ARCHIVE // アーカイブからの読みこみテスト
    m_Archive.InitializeFromMemory(RESOURCE_BIN,sizeof(RESOURCE_BIN));
    m_Archive.Wait();
    m_FileMapperArchive.Setup("",TB_FILE_PATH_ID_PRESET_ROOT,&m_Archive,1);
    TbFileManager::GetInstance().AddMapper(&m_FileMapperArchive);

    TbFileManager::PathList path;
    TbFileManager::GetInstance().GetFiles(path,"tex",TB_FILE_SEARCH_FLAG_RELATIVE);
#endif

#if CMN_ENABLE_BUILD_100P
    m_ResTexture.Create("tex/tex_picolecitta_100.tga",0,TB_FILE_SYNC);
#else
    m_ResTexture.Create("tex/tex_picolecitta.tga",0,TB_FILE_SYNC);
#endif
    m_ResTexture.Activate();
    m_ShaderManager.Initialize();

    // フォント設定(SJIS)
    {
        TbFontCreateParamSjis param;
        param.common.charCount = TB_ARRAY_COUNT_OF(FONT_SJIS);
        param.common.defaultFontSize = 64;
        param.common.uvBaseSize = TbDimensionF32(0.062500,0.062500);
        param.charInfoList = FONT_SJIS;
        m_FontTexture.Create("font/font_famania.tga",0,TB_FILE_SYNC);
        m_FontTexture.Activate();
        m_Font = new TbFontSjis(param);
        m_Font->SetTexture(m_FontTexture.GetTexture());
        m_FontBuffer = new TbFontBuffer2d(1024*100,TbFrameworkSimple::RENDER_PASS_2D);
        m_FontBuffer->SetFont(m_Font);
        m_FontBuffer->SetFontSize(48);
        m_FontBuffer->SetColor(CmnUtil::GetMainColor());

        m_LocalFontBuffer = new TbFontBuffer2d(1024*100,TbFrameworkSimple::RENDER_PASS_2D);
        m_LocalFontBuffer->SetFont(m_Font);
        m_LocalFontBuffer->SetFontSize(48);
        m_LocalFontBuffer->SetColor(CmnUtil::GetMainColor());

        param.common.charCount = TB_ARRAY_COUNT_OF(DEBUG_FONT_SJIS);
        param.common.defaultFontSize = 64;
        param.common.uvBaseSize = TbDimensionF32(0.062500,0.062500);
        param.charInfoList = DEBUG_FONT_SJIS;
        m_DebugFontTexture.Create("font/font.tga",0,TB_FILE_SYNC);
        m_DebugFontTexture.Activate();
        m_DebugFont = new TbFontSjis(param);
        m_DebugFont->SetTexture(m_DebugFontTexture.GetTexture());
        m_DebugFontBuffer = new TbFontBuffer2d(1024*100,TbFrameworkSimple::RENDER_PASS_2D);
        m_DebugFontBuffer->SetFont(m_DebugFont);
        m_DebugFontBuffer->SetFontSize(32);
        m_DebugFontBuffer->SetColor(TbColor::Red());
    }

    // スクリプト初期化
    {
        static const TbUint32 SCRIPT_VM_DEFAULT = 1;
        static const TbUint32 SCRIPT_THREAD_DEFAULT = 1;
        static const TbUint32 SCRIPT_STACK_DEFAULT = 10;
        m_ScriptManager = new TbScriptLuaManager(SCRIPT_VM_DEFAULT);
        m_ScriptManager->CreateVM(SCRIPT_THREAD_DEFAULT,SCRIPT_STACK_DEFAULT);
    }

    // シーケンス
    m_SeqManager.Initialize();

    // 半透明背景
    m_TransparentSprite = new CmnWhiteSprite();
    m_TransparentSprite->Initialize(TbRectF32(0.0f,0.0f,CMN_WINDOW_WIDTH, CMN_WINDOW_HEIGHT));
    m_TransparentSprite->SetColor(TbColor::FromRgbaU8(0xFF, 0xFF, 0xFF, 0xAA));

    // サウンド
    {
        TbSoundInitParam soundInitParam; // デフォルト値使う
        soundInitParam.simplePlayCount = 30;
        soundInitParam.soundMax = 30;
        m_SoundManager = new TbSoundManager( soundInitParam );
        // BGM読みこみ
#if 1
        m_SoundManager->LoadSound("sound/bgm.ogg","title_bgm", TB_SOUND_TYPE_STREAM);
        m_SoundManager->LoadSound("sound/bounds.ogg","stage_select", TB_SOUND_TYPE_STREAM);
        m_SoundManager->LoadSound("sound/hands.ogg","bgm", TB_SOUND_TYPE_STREAM);
        m_SoundManager->LoadSound("sound/jump.ogg","jump",TB_SOUND_TYPE_STATIC);
        m_SoundManager->LoadSound("sound/coin.ogg","coin",TB_SOUND_TYPE_STATIC);
#else
        m_SoundManager->LoadSound("sound/bgm.wav","title_bgm",TB_SOUND_TYPE_STREAM);
        m_SoundManager->LoadSound("sound/bounds.wav","stage_select",TB_SOUND_TYPE_STREAM);
        m_SoundManager->LoadSound("sound/hands.wav","bgm",TB_SOUND_TYPE_STREAM);
#endif
        // SE再生
#if 0
        TbUiManager::GetInstance().LoadSE(TB_UI_SE_SELECT,"sound/select.ogg");
        TbUiManager::GetInstance().LoadSE(TB_UI_SE_OPEN,"sound/open.ogg");
        TbUiManager::GetInstance().LoadSE(TB_UI_SE_CANCEL,"sound/cancel.ogg");
#endif
//        m_SoundManager->LoadSound("sound/se.wav","se",TB_SOUND_TYPE_STATIC);
    }

    // 入力設定
#if NET_INPUT_ENABLE
    m_NetInputServer = new NetInputServer();
#endif
    TbInputSetting::SetRepeatMilliSecFirst(33.0f*10);
    TbInputSetting::SetRepeatMilliSec(66.0f);
    m_GiantInput.Initialize();
    {
        LoadUserData();
        SaveUserData();
    }

    m_UiManager.SetTexture(m_ResTexture.GetTexture());
    m_UiManager.SetFontBuffer(m_FontBuffer);

    SetSoundVolumeLevel(m_SoundVolumeLevel);

    m_PlayerMaterial = new CmnSpritePlayerMaterial();

#if 1 // TB_IS_DEBUG
    // デバッグメニュー設定
    TbDebugMenuManager* debugMenu = TbDebugMenuManager::GetInstance();
    if( debugMenu ) {
        debugMenu->SetFontBuffer(m_DebugFontBuffer);
        debugMenu->SetPos( TbVector2(10.0f,10.0f) );
        debugMenu->SetEnableInputPad(TB_FALSE);

        m_SeqManager.CreateDebugMenu();

        TbDebugMenu* inputMenu = debugMenu->CreateMenu("SysInput");

        // タイプ
        TbDebugMenuItemBool* dispItem = inputMenu->CreateItemBool("IsEnableDrawGuid");
        dispItem->Bind(&s_IsEnableDrawGuid);
        m_GiantInput.SetupDebugMenu(inputMenu);

        TbDebugMenu* settingMenu = debugMenu->CreateMenu("Setting");
        TbDebugMenuItemU32* playerCountMenu = settingMenu->CreateItemU32("PlayerCount");
        playerCountMenu->Bind(&m_PlayerCount);
        playerCountMenu->SetRange(0,CMN_PLAYER_MAX,TB_TRUE);

        TbDebugMenu* systemMenu = debugMenu->CreateMenu("System");
        TbDebugMenuItemButton* finishButton = systemMenu->CreateItemButton("Finish");
        finishButton->SetClickedCallback( finishSystem );
#if !(TB_IS_DEBUG || 0)
        debugMenu->SetEnable(TB_FALSE);
#endif
    }
#endif
#if 1 || !TB_PLATFORM_IS_WIN
    TbDraw2dTextureAllocVertexBuffer(1024 * 1024);
#endif
}

/*!
 * 終了処理
 * @author teco
 * @since 2013.09.02
 */
Root::~Root()
{
    TbDraw2dTextureFreeVertexBuffer();
    m_SeqManager.Reset();
    TbTaskManager::GetInstance().Kill();
    TB_SAFE_DELETE(m_TransparentSprite);
    TB_SAFE_DELETE(m_PlayerMaterial);
#if NET_INPUT_ENABLE
    TB_SAFE_RELEASE(m_NetInputServer);
#endif
    TB_SAFE_DELETE(m_Font);
    TB_SAFE_DELETE(m_FontBuffer);
    TB_SAFE_DELETE(m_LocalFontBuffer);
    TB_SAFE_DELETE(m_DebugFont);
    TB_SAFE_DELETE(m_DebugFontBuffer);
    TB_SAFE_DELETE(m_ScriptManager);
    TB_SAFE_DELETE(m_SoundManager);
}

/*!
 * 更新
 * @author teco
 * @since 2013.09.02
 */
void Root::Update()
{
    m_SoundManager->Update();

    if( !isInitialized() ){
        return;
    }

    m_GiantInput.Update();

    // 強制やり直し
    if( SysInput::GetBool(SYS_INPUT_ID_BOOL_RETURN_TITLE) ){
        m_SeqManager.RequestSequence(SEQ_TITLE,1);
    }

    if( SysInput::GetBool(SYS_INPUT_ID_BOOL_RETURN_STAGE_SELECT) ){
#if CMN_ENABLE_BUILD_OLD
        m_SeqManager.RequestSequence(SEQ_STAGE_SELECT,0);
#else
        m_SeqManager.RequestSequence(SEQ_WORLD_SELECT, 0);
#endif
    }

    if( SysInput::GetBool(SYS_INPUT_ID_BOOL_BREAK ) ){
        TbBreak();
    }

    m_UiManager.Update();
    m_SeqManager.Update();
    TbTaskManager::GetInstance().ExecuteTask( CMN_TASK_LINE_UPDATE );

    // キーボード操作対象切り替え
    for( TbSint32 key = TB_KEY_1; key <= TB_KEY_0; ++key ) {
        if( TbKeyboard::IsPress(static_cast<TbKeyType>(key)) ){
            SysInput::SetMainPlayerKeyboardPadIndexBit(TB_BIT((key-TB_KEY_1)));
        }
    }
#if 1 || (1 && TB_IS_DEBUG)
    if( TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_0) ){
        SysInput::SetMainPlayerKeyboardPadIndexBit(0xFFFFFFFF);
    }
#endif
#if (1 && TB_IS_DEBUG)
    if (TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_S)) {
        m_ShaderManager.Reload();
    }
#endif
#if (1 && TB_IS_DEBUG)
    if (TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_C)) {
        // 強制クリア(後で整理すべき。ワールド別ステージ数が管理されていない)
        m_GlobalData.SetWorldStageClearCount(0, 0, 1);
        m_GlobalData.SetWorldStageClearCount(0, 1, 1);
        m_GlobalData.SetWorldStageClearCount(0, 2, 1);
        m_GlobalData.SetWorldStageClearCount(0, 3, 1);
    }
#endif

#if CMN_ENABLE_BUILD_ARCADE
    // 二分間何も触ってないとタイトルに戻る
    {
        static const TbFloat32 RESET_SEC = 60.0f * 2.0f;
        if( SysInput::IsOnAny() ){
            m_IdleSec = 0.0f;
        }else{
            m_IdleSec += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds())/1000.0f;
            if( m_IdleSec > RESET_SEC ) {
                SeqManager::GetInstance().RequestSequence(SEQ_TITLE,0);
                m_IdleSec = 0.0f;
                return;
            }
        }
    }
#endif

}

/*!
 * 描画
 * @author teco
 * @since 2013.09.02
 */
void Root::Draw()
{
	TbTaskManager::GetInstance().ExecuteTask(CMN_TASK_LINE_DRAW);

    // 半透明フィルターをかけるか
    if( m_BitArray.Test(FLAG_ENABLE_TRANSLUCENT) ){
        m_TransparentSprite->Calculate();
        m_TransparentSprite->Draw(TbMatrix::IDENTITY);
    }

    m_UiManager.Draw();

    // パフォーマンス
#if 0
//   m_DebugFontBuffer->Printf( 10 , CMN_WINDOW_HEIGHT-30 , "TIME=%3.3f COUNT=%0.4f" , TbTick::GetFrameSpeedMilliSeconds() , TbTick::GetFrameSpeedCountF32() );
     TbVector4 cursor;
     TbMouse::GetPos(cursor,TB_MOUSE_POS_TYPE_WINDOW);
     m_DebugFontBuffer->Printf( 10 , CMN_WINDOW_HEIGHT-30 , "lx=%3.3f ly=%3.3f" , cursor.GetX() , cursor.GetY() );
     {
        TbRectS32 rect = TbRenderDevice::GetInstance().GetView(0)->GetPresentRegion();
        TbFloat32 x = static_cast<TbFloat32>(cursor.GetX()-rect.x) / static_cast<TbFloat32>(rect.width);
        TbFloat32 y = static_cast<TbFloat32>(cursor.GetY()-rect.y) / static_cast<TbFloat32>(rect.height);
        x *= static_cast<TbFloat32>(CMN_WINDOW_WIDTH);
        y *= static_cast<TbFloat32>(CMN_WINDOW_HEIGHT);
         m_DebugFontBuffer->Printf( x , y , "lx=%3.3f ly=%3.3f" , x , y );
     }
#endif

#if 1
    if( s_IsEnableDrawGuid )
#endif
    {
        TbUint32 count = TbPad::GetPadCount();
        for ( TbUint32 i = 0; i < count; ++i ) {
            TbUint64 id = TbPad::GetPadUniqId(i);
            m_FontBuffer->Printf( 30 , 30+i*40 , "id=%lu" , id ); 
        }
    }

    if( m_FontBuffer ) {
        m_FontBuffer->Flush();
    }
    if( m_DebugFontBuffer ){
        m_DebugFontBuffer->Flush();
    }

    CmnFade::GetInstance().Draw();
}

/*!
 * 初期化済みか
 * @author teco
 */
TbBool Root::isInitialized()
{
    if( m_BitArray.Test(FLAG_INITIALIZE) ){
        return TB_TRUE;
    }
    if( // TbUiManager::GetInstance().IsReadySE() &&
        m_SoundManager && 
        ( !m_SoundManager->IsInitialized() ||
           m_SoundManager->IsReadySoundAll() ) 
      )
    {
#if CMN_ENABLE_BUILD_SOWN
        // タイトル画面からスタート
        m_SeqManager.RequestSequence(SEQ_TITLE,1);
#elif CMN_ENABLE_BUILD_TETRIS
        m_SeqManager.RequestSequence(SEQ_STAGE_SELECT,0);
#else
        m_SeqManager.RequestSequence(SEQ_TITLE,0);
#endif
        m_BitArray.SetBit(FLAG_INITIALIZE);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ユーザーデータロード
 * @author teco
 */
void Root::LoadUserData()
{
    TbSaveFile file;
    file.Initialize("userdata.lua",TB_FILE_SYNC);
    if(TB_FAILED(file.Read())){
        return;
    }

    // シーケンス設定取得
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);
    
    TbScriptLuaNode::Result res = node->DoBuffer(file.GetReadBuffer(), file.GetReadBufferSize());
    if( res != TbScriptLuaNode::RESULT_OK ) {
#if !CMN_ENABLE_BUILD_ALPHA
        SetFullscreen(TB_TRUE);
#endif
        return;
    }

    // 音量
    res = node->GoToAbsolutely("userData");
    res = node->TryToSint32(m_SoundVolumeLevel,"soundVolume");
    TbSint32 isFullscreen = TB_FALSE;
    res = node->TryToSint32(isFullscreen,"isFullscreen");
    
    // パッド設定
    {
        node->GoToAbsolutely("userData.padConfig");
        TbUint32 count = 0;
        node->TryToTableLength(count);
        for( TbUint32 i = 0; i < count; ++i ) {
            TbUint32 id = 0;
            TbUint32 button = 0;
            node->GoToChild(i);
            if( TbScriptLuaNode::RESULT_OK !=  node->TryToUint32(id,"product") ){
                continue;
            }
            if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(button,"start") ){
                TbPad::SetPadButtonSettingByProductUniqId( id , TB_PAD_START , button );
            }
            if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(button,"a") ){
                TbPad::SetPadButtonSettingByProductUniqId( id , TB_PAD_A , button );
            }
            if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(button,"b") ){
                TbPad::SetPadButtonSettingByProductUniqId( id , TB_PAD_B , button );
            }
            node->GoToParent(1);
        }
    }

    // パッド設定(tecogamepad)
#if NET_INPUT_ENABLE
    {
        NetInputServer& netInput = NetInputServer::GetInstance();
        node->GoToAbsolutely("userData.tecogamepadConfig");
        TbUint32 id = 0;
        TbUint32 button = 0;
        if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(button,"start") ){
            netInput.SetPadButtonSetting( TB_PAD_START , button );
        }
        if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(button,"a") ){
            netInput.SetPadButtonSetting( TB_PAD_A , button );
        }
        if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(button,"b") ){
            netInput.SetPadButtonSetting( TB_PAD_B , button );
        }
    }
#endif

    // キーボード
    {
        node->GoToAbsolutely("userData.keyboardConfig");
        TbUint32 count = 0;
        node->TryToTableLength(count);
        for( TbUint32 p = 0; p < count; ++p ) {
            node->GoToChild(p);

            for( TbSint32 i = 0; i < SysInput::BUTTON_MAX; ++i ) {
                TbSint32 key = TB_KEY_INVALID;
                if( TbScriptLuaNode::RESULT_OK !=  node->TryToSint32ByIndex(key,i) ) {
                    break;
                }
                SysInput::BindButtonKeyboardKey(p,static_cast<SysInput::Button>(i),static_cast<TbKeyType>(key));
            }

            node->GoToParent(1);
        }
    }

    {
        node->GoToAbsolutely("userData.stageData");
        TbUint32 count = 0;
        node->TryToTableLength(count);
        for( TbUint32 i = 0; i < count; ++i ) {
            TbUint32 id = 0;
            TbUint32 value = 0;
            const char* valueStr = nullptr;

            node->GoToChild(i);
//            node->TryToTableLength(count);
            if( TbScriptLuaNode::RESULT_OK != node->TryToString(&valueStr,"name") ){
                continue;
            }
            TbSint32 stageIndex = m_SeqManager.GetStageIndex(valueStr);
            if( 0 <= stageIndex )
            {
                if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(value,"playCount") ){
                    m_GlobalData.SetPlayCount(stageIndex,value);
                }
                if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32(value,"clearCount") ){
                    m_GlobalData.SetClearCount(stageIndex,value);
                }
                node->GoToChild("bestTime");
                for( TbUint32 p = 0; p < CMN_PLAYER_MAX; ++p ) {
                    if( TbScriptLuaNode::RESULT_OK ==  node->TryToUint32ByIndex(value,p) ){
                        m_GlobalData.SetBestMillSec(stageIndex,p+1,value);
                    }
                }
                node->GoToParent(1);
            }
            node->GoToParent(1);
        }
    }

    if( isFullscreen ){
        SetFullscreen(TB_TRUE);
    }
}

/*!
 * ユーザーデータセーブ
 * @author teco
 */
void Root::SaveUserData()
{
    TbSaveFile file;
    file.Initialize("userdata.lua",TB_FILE_SYNC);
    {
        static const TbUint32 BUFSIZE = 1024*64;
        TbChar8* buf = new TbChar8[BUFSIZE]; // 最大64K
        TbMemoryStream stream(buf,BUFSIZE);
        TbStreamTextWriter writer(&stream);
        writer.WriteString("userData=\n");
        writer.WriteString("{\n");

        // 音量
        {
            TbStaticString128 str;
            str.SetFormatedString("    soundVolume=%u,\n",m_SoundVolumeLevel);
            writer.WriteString(str.GetBuffer());
        }

        // フルスクリーン
        {
            TbStaticString128 str;
            str.SetFormatedString("    isFullscreen=%d,\n",IsFullscreen());
            writer.WriteString(str.GetBuffer());
        }

        // パッドコンフィグ
        writer.WriteString("    --padconfig setting\n");
        writer.WriteString("    padConfig=\n");
        writer.WriteString("    {\n");
        {
            TbUint32 count = TbPad::GetPadProductCount();
            for( TbUint32 i = 0; i < count; ++i ) 
            {
                const TbPadButtonSetting* setting = TbPad::GetPadProductSetting(i);
                if( !setting ){
                    continue;
                }
                TbStaticString128 str;
                writer.WriteString("        {\n");
                str.SetFormatedString("            product=%u,\n",setting->productGuid);
                writer.WriteString(str.GetBuffer());
                str.SetFormatedString("            start=%u,\n",setting->table[TB_PAD_START]);
                writer.WriteString(str.GetBuffer());
                str.SetFormatedString("            a=%u,\n",setting->table[TB_PAD_A]);
                writer.WriteString(str.GetBuffer());
                str.SetFormatedString("            b=%u,\n",setting->table[TB_PAD_B]);
                writer.WriteString(str.GetBuffer());
                writer.WriteString("        },\n");
            }
        }
        writer.WriteString("    },\n");

#if NET_INPUT_ENABLE
        // パッドコンフィグ(TecoGamePad
        writer.WriteString("    --tecogamepadconfig setting\n");
        writer.WriteString("    tecogamepadConfig=\n");
        writer.WriteString("    {\n");
        {
            NetInputServer& netInput = NetInputServer::GetInstance();
            TbStaticString128 str;
            str.SetFormatedString("            start=%u,\n",netInput.GetPadButtonSetting(TB_PAD_START) );
            writer.WriteString(str.GetBuffer());
            str.SetFormatedString("            a=%u,\n",netInput.GetPadButtonSetting(TB_PAD_A) );
            writer.WriteString(str.GetBuffer());
            str.SetFormatedString("            b=%u,\n",netInput.GetPadButtonSetting(TB_PAD_B) );
            writer.WriteString(str.GetBuffer());
        }
        writer.WriteString("    },\n");
#endif

        // キーボードコンフィグ
        writer.WriteString("    --keyboardConfig setting\n");
        writer.WriteString("    keyboardConfig=\n");
        writer.WriteString("    {\n");
        {
            for( TbUint32 p = 0; p < CMN_PLAYER_MAX; ++p ) {
                writer.WriteString("        {\n");
                for( TbSint32 i = 0; i < SysInput::BUTTON_MAX; ++i ) {

                    TbKeyType keyType = SysInput::GetButtonKeyboardKey(p,static_cast<SysInput::Button>(i));
                    TbStaticString128 str;
                    str.SetFormatedString("            %d,\n",keyType);
                    writer.WriteString(str.GetBuffer());
                }
                writer.WriteString("        },\n");
            }
        }
        writer.WriteString("    },\n");

        // タイムレコード
        writer.WriteString("    --stage data\n");
        writer.WriteString("    stageData=\n");
        writer.WriteString("    {\n");
        {
            TbUint32 count = m_SeqManager.GetStageCount();;
            for( TbUint32 i = 0; i < count; ++i ) 
            {
                TbStaticString128 str;
                TbUint32 ms = 0;
                writer.WriteString("        {\n");
                str.SetFormatedString("            name=\"%s\",\n",m_SeqManager.GetStageName(i)); 
                writer.WriteString(str.GetBuffer());
                str.SetFormatedString("            playCount=%u,\n",m_GlobalData.GetPlayCount(i)); 
                writer.WriteString(str.GetBuffer());
                str.SetFormatedString("            clearCount=%u,\n",m_GlobalData.GetClearCount(i)); 
                writer.WriteString(str.GetBuffer());
                writer.WriteString("            bestTime=\n");
                writer.WriteString("            {\n");
                for( TbUint32 p = 0; p < CMN_PLAYER_MAX; ++p ) {
                    str.SetFormatedString("                %u,\n",m_GlobalData.GetBestMillSec(i,p+1)); 
                    writer.WriteString(str.GetBuffer());
                }
                writer.WriteString("            },\n");
                writer.WriteString("        },\n");
            }
        }
        writer.WriteString("    }\n");

        writer.WriteString("}\n\0");
        writer.WriteEOFChar8();

        TbSizeT fileSize = TbStrCharCount(buf);
        file.Write(buf,fileSize);
        TB_SAFE_DELETE_ARRAY(buf);
    }
}

/*!
 * サウンドボリュームレベル設定
 * @author teco
 */
void Root::SetSoundVolumeLevel( TbSint32 level )
{
    m_SoundVolumeLevel = TbClamp<TbSint32>(level,0,CMN_SOUND_VOLUME_LEVEL_MAX);
    TbFloat32 volume = CmnUtil::ConvertSoundLevelToVolume(m_SoundVolumeLevel);
    m_SoundManager->SetMasterVolume(volume);
}

/*!
 * フルスクリーン設定
 * @author teco
 */
void Root::SetFullscreen( TbBool isFullscreen )
{
    TbRenderDevice::GetInstance().SetFullscreen(isFullscreen);
    if( !isFullscreen ){
        TbWindow* window = TbWindowManager::GetInstance().GetFocusWindow();
        if( window ){
            window->SetSize(CMN_WINDOW_WIDTH,CMN_WINDOW_HEIGHT);
        }
    }
}

/*!
 * フルスクリーン取得
 * @author teco
 */
TbBool Root::IsFullscreen() const
{
    return TbRenderDevice::GetInstance().IsFullscreen();
}

/*!
 * ゲーム上の矩形を画面上の矩形サイズに変換
 * フルスクリーン取得
 * @author teco
 */
TbBool Root::ConvertGameRectToScreenRect( TbRectF32* dst , const TbRectF32& src )
{
    // 表示領域にマッピング
    TbRectS32 presentRect = TbRenderDevice::GetInstance().GetView(0)->GetPresentRegion();
    if( 0==presentRect.width || 0==presentRect.height ) {
        return TB_FALSE;
    }
    TbFloat32 widthScale = static_cast<TbFloat32>(presentRect.width)/static_cast<TbFloat32>(CMN_WINDOW_WIDTH);
    TbFloat32 heightScale = static_cast<TbFloat32>(presentRect.height)/static_cast<TbFloat32>(CMN_WINDOW_HEIGHT);
    dst->width  = (src.width*widthScale);
    dst->height = (src.height*heightScale);
    dst->x = presentRect.x + (src.x)*widthScale;
    dst->y = presentRect.y + (src.y)*heightScale;
    return TB_TRUE;
}
