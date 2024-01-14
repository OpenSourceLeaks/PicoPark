/*!
 * ルート
 * @author teco
 */

#include "fwd.h"
#include "root.h"

#include <base/task/tb_task_manager.h>
#include <base/io/resource/tb_resource_all.h>
#include <base/io/tb_file_manager.h>
#include <base/time/tb_tick.h>
#include <base/util/tb_bind.h>
#include <base/time/tb_time.h>
#include <base/time/tb_tick.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <framework/debug/menu/tb_debug_menu_all.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/util/tb_screen_shot.h>
#include <input/tb_pad.h>
#include <input/tb_mouse.h>
#include <input/tb_keyboard.h>
#include <input/tb_input_setting.h>
#include <base/io/stream/tb_stream_text_writer.h>
#include <base/io/stream/tb_file_stream.h>
#include <base/io/stream/tb_memory_stream.h>
#include <base/system/tb_debug.h>

#define IS_ENABLE_MEMORY_ARCHIVE (1 && !TB_IS_DEBUG)
#if IS_ENABLE_MEMORY_ARCHIVE
#include "outbin.h"
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

/*!
 * 初期化
 * @author teco
 * @since 2013.09.02
 */
Root::Root()
    : m_SeqManager()
    , m_Font(nullptr)
    , m_FontBuffer(nullptr)
    , m_DebugFont(nullptr)
    , m_DebugFontBuffer(nullptr)
    , m_ScriptManager(nullptr)
    , m_SoundManager(nullptr)
{
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_SpriteMaterial); ++i ) {
        m_SpriteMaterial[i] = TB_NEW  TbSpriteFixedMaterial(TbFrameworkSimple::RENDER_PASS_2D);
    }

#if IS_ENABLE_MEMORY_ARCHIVE // アーカイブからの読みこみテスト
    m_Archive.InitializeFromMemory(RESOURCE_BIN,sizeof(RESOURCE_BIN));
    m_Archive.Wait();
    m_FileMapperArchive.Setup("",TB_FILE_PATH_ID_PRESET_ROOT,&m_Archive,1);
    TbFileManager::GetInstance().AddMapper(&m_FileMapperArchive);
#endif

    const char* TEX_TABLE[] =
    {
        "tex/tex_common.tga",
    };
    {
        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_ResTexture); ++i )
        {
            m_ResTexture[i].Create(TEX_TABLE[i],0,TB_FILE_SYNC);
            m_ResTexture[i].Activate();
            m_SpriteMaterial[i]->SetTexture(m_ResTexture[i].GetTexture());
        }
    }

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
        m_FontBuffer->SetFontSize(16);
        m_FontBuffer->SetColor(TbColor::White());

        m_LocalFontBuffer = new TbFontBuffer2d(1024*100,TbFrameworkSimple::RENDER_PASS_2D);
        m_LocalFontBuffer->SetFont(m_Font);
        m_LocalFontBuffer->SetFontSize(16);
        m_LocalFontBuffer->SetColor(TbColor::White());
        
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
        m_DebugFontBuffer->SetColor(TbColor::White());
    }

    // スクリプト初期化
    {
        static const TbUint32 SCRIPT_VM_DEFAULT = 1;
        static const TbUint32 SCRIPT_THREAD_DEFAULT = 2;
        static const TbUint32 SCRIPT_STACK_DEFAULT = 10;
        m_ScriptManager = new TbScriptLuaManager(SCRIPT_VM_DEFAULT);
        m_ScriptManager->CreateVM(SCRIPT_THREAD_DEFAULT,SCRIPT_STACK_DEFAULT);
    }
    m_SeqManager.Initialize();

    // サウンド
    {
        TbSoundInitParam soundInitParam; // デフォルト値使う
        soundInitParam.soundMax = 10;
        m_SoundManager = new TbSoundManager( soundInitParam );

        // BGM読みこみ
//        m_SoundManager->LoadSound("sound/title.wav","title_bgm",TB_SOUND_TYPE_STREAM);
//        m_SoundManager->LoadSound("sound/ymck.ogg","ymck",TB_SOUND_TYPE_STREAM);
    }

    // 入力設定
    TbInputSetting::SetRepeatMilliSecFirst(33.0f*10);
    TbInputSetting::SetRepeatMilliSec(66.0f);

    m_UiManager.SetTexture(m_ResTexture[TEX_COMMON].GetTexture());
    m_UiManager.SetFontBuffer(m_FontBuffer);

#if 1 // TB_IS_DEBUG
    // デバッグメニュー設定
    TbDebugMenuManager* debugMenu = TbDebugMenuManager::GetInstance();
    if( debugMenu ) {
        debugMenu->SetFontBuffer(m_DebugFontBuffer);
        debugMenu->SetPos( TbVector2(10.0f,10.0f) );
        debugMenu->SetEnableInputPad(TB_FALSE);

        m_SeqManager.CreateDebugMenu();

        TbDebugMenu* inputMenu = debugMenu->CreateMenu("CrInput");
        TbDebugMenuItemBool* dispItem = inputMenu->CreateItemBool("IsEnableDrawGuid");
        dispItem->Bind(&s_IsEnableDrawGuid);

        TbDebugMenu* systemMenu = debugMenu->CreateMenu("System");
        TbDebugMenuItemButton* finishButton = systemMenu->CreateItemButton("Finish");
        finishButton->SetClickedCallback( finishSystem );


#if !(TB_IS_DEBUG || 0)
        debugMenu->SetEnable(TB_FALSE);
#endif
    }
#endif
}

/*!
 * 終了処理
 * @author teco
 * @since 2013.09.02
 */
Root::~Root()
{
    m_SeqManager.Reset();
    TbTaskManager::GetInstance().Kill();
    TB_SAFE_DELETE(m_Font);
    TB_SAFE_DELETE(m_FontBuffer);
    TB_SAFE_DELETE(m_LocalFontBuffer);
    TB_SAFE_DELETE(m_DebugFont);
    TB_SAFE_DELETE(m_DebugFontBuffer);
    TB_SAFE_DELETE(m_ScriptManager);
    TB_SAFE_DELETE(m_SoundManager);
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_SpriteMaterial); ++i) {
        TB_SAFE_DELETE(m_SpriteMaterial[i]);
    }
}

/*!
 * 更新
 * @author teco
 * @since 2013.09.02
 */
void Root::Update()
{
    m_SoundManager->Update();

    if (!m_BitArray.Test(FLAG_INITIALIZE)) 
    {
        if (m_SoundManager &&
            (!m_SoundManager->IsInitialized() || m_SoundManager->IsReadySoundAll()))
        {
            m_SeqManager.RequestSequence(SEQ_MAIN, CMN_STAGE_ID_DEFAULT);
            m_BitArray.SetBit(FLAG_INITIALIZE);
        }
        else
        {
            return;
        }
    }

    // 強制やり直し
    if( CrInput::GetBool(CR_INPUT_ID_BOOL_RETURN_TITLE) ){
        m_SeqManager.RequestSequence(SEQ_MAIN, 0);
    }

    if( CrInput::GetBool(CR_INPUT_ID_BOOL_CAPTURE ) ){
        const TbDate& date = TbTime::GetDate();
        const TbStaticString64& filePath = TbStaticString64::Format("capture%d%d%d%d%d.tga",date.month,date.day,date.hour,date.minute,date.second);
        TbScreenShot::CaptureAndExportSync( filePath.GetBuffer() , TbScreenShot::FILE_TGA );
    }

    if( CrInput::GetBool(CR_INPUT_ID_BOOL_BREAK ) ){
        TbFloat32 masterVolume = TbSoundManager::GetInstance().GetMasterVolume();
        TbSoundManager::GetInstance().SetMasterVolume( 0.0f < masterVolume ? 0.0f : 1.0f );
//        TbBreak();
    }

    m_UiManager.Update();
    m_SeqManager.Update();
    TbTaskManager::GetInstance().ExecuteTask( CMN_TASK_LINE_UPDATE );

    // キーボード操作対象切り替え
    for( TbSint32 key = TB_KEY_1; key <= TB_KEY_0; ++key ) {
        if( TbKeyboard::IsPress(static_cast<TbKeyType>(key)) ){
            CrInput::SetKeyboardIndexBit(TB_BIT((key-TB_KEY_1)));
        }
    }
    if( TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_0) ){
        CrInput::SetKeyboardIndexBit(0xFFFFFFFF);
    }
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
        TbDraw2dBegin();
        TbDrawRectParam param;
        param.SetRect(0.0,0.0f,CMN_WINDOW_WIDTH,CMN_WINDOW_HEIGHT);
        param.SetColor( TB_COLOR_A8R8G8B8(0xAA,0xFF,0xFF,0xFF) );
        TbDrawRect(param);
        TbDraw2dEnd();
    }

    m_UiManager.Draw();


    if( m_FontBuffer ) {
        m_FontBuffer->Flush();
    }
    if( m_DebugFontBuffer ){
        m_DebugFontBuffer->Flush();
    }
}
