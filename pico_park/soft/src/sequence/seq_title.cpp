/*!
 * タイトルシーケンス
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_title.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include <crayon/input/cr_input.h>
#include <crayon/input/cr_giant_input.h>
#include <input/tb_pad.h>
#include <input/tb_mouse.h>
#include <framework/tb_framework.h>
#include "common/cmn_util.h"

static const TbRectF32Pod SPRITE_RECT[] = 
{
    { -29 , -10 , 58 , 20},
    { -1024 , -256 , 2048 , 256 },
    { -1024 , -256 , 2048 , 256 },
    { -8 , -8 , 16 , 16 },

    { -64 , -40 , 128 , 80 },
    { -64 , -40 , 128 , 80 },

    { -416 , -195 , 832 , 390 },
    { -416 , -195 , 832 , 390 },
};
static const TbRectF32Pod SPRITE_UV[] =
{
    { CMN_TEX_UV_RECT(320,320,29,10) },
    { 0.0f,0.5f,1.0f,0.5f },
    { 0.0f,0.0f,1.0f,0.5f },
    { CMN_TEX_UV_RECT(400,352,16,16) },
    { CMN_TEX_UV_RECT(656,368,128,80) },
    { CMN_TEX_UV_RECT(656,448,128,80) },

    { CMN_TEX_UV_RECT(0,0,832,390) },
    { CMN_TEX_UV_RECT(0,512,832,390) },
};
static const TbVector2 SPRITE_POS[] =
{
    TbVector2(CMN_WINDOW_WIDTH/2,CMN_WINDOW_HEIGHT/2) ,
    TbVector2(CMN_WINDOW_WIDTH/2,static_cast<TbFloat32>(CMN_WINDOW_HEIGHT)/1.8f) ,
    TbVector2(CMN_WINDOW_WIDTH/2,static_cast<TbFloat32>(CMN_WINDOW_HEIGHT)/1.8f) ,
    TbVector2(CMN_WINDOW_WIDTH/2,static_cast<TbFloat32>(CMN_WINDOW_HEIGHT)/1.6f) ,
    TbVector2(CMN_WINDOW_WIDTH / 2 + 244.0f , CMN_WINDOW_HEIGHT / 1.5f + 4.0f ) ,
    TbVector2(CMN_WINDOW_WIDTH / 2 + 8.0f ,CMN_WINDOW_HEIGHT / 1.5f + 4.0f ) ,

    TbVector2(CMN_WINDOW_WIDTH / 2 , CMN_WINDOW_HEIGHT / 1.5f ) ,
    TbVector2(CMN_WINDOW_WIDTH / 2 ,CMN_WINDOW_HEIGHT / 1.5f ) ,
};
static const TbFloat32 SPRITE_SCALE[] =
{
    5.0f ,
    0.5f ,
    0.5f ,
    5.0f ,
    1.0f ,
    1.0f ,

    0.5f ,
    0.5f ,
};

static TbFloat32 ADVERTISE_FADE_IN   = 0.5f;
static TbFloat32 ADVERTISE_FADE_WAIT = 2.0f;
static TbFloat32 ADVERTISE_FADE_OUT  = 0.5f;
static TbFloat32 AUTOSAVE_WAIT = 4.5f;

#if CMN_ENABLE_BUILD_GIANT_INPUT
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_title_giant.lua";
#elif CMN_ENABLE_BUILD_ARCADE
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_title_arcade.lua";
#else
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_title.lua";
#endif

#if CMN_ENABLE_BUILD_OLD
    typedef UiSelectWindow MainSelectWindow;
    #define m_MainSelectWindow m_SelectWindow
#else
    typedef UiPlayerSelectWindow MainSelectWindow;
    #define m_MainSelectWindow m_PlayerSelectWindow
#endif

#define ENABLE_PLAYSTYLE_IMAGE (0)

static const char* WARNING_AUTOSAVE_EN = "THE GAME SAVES DATA AUTOMATICALLY AT CERTAIN POINTS.\nDO NOT SWITCH OFF THE POWER WHEN THIS ICON IS DISPLAYED.";
static const char* WARNING_AUTOSAVE_JP = "\n";

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqTitle::SeqTitle( TbBool isEnableSkipAdvertise , TbBool isEnableSelectPlayer )
    : Super(SEQ_TITLE,"SeqTitle")
    , m_Phase(PHASE_ADVERTISE)
    , m_PhaseTime(0.0f)
    , m_SpriteMaterial(TbFrameworkSimple::RENDER_PASS_2D)
    , m_SpriteMaterialAutoSave(TbFrameworkSimple::RENDER_PASS_2D)
    , m_SpriteMaterialPlayStyle(TbFrameworkSimple::RENDER_PASS_2D)
    , m_Stage(nullptr)
    , m_ConfirmWindow(UiMessageWindow::TYPE_YESNO)
    , m_PlayerCount(CMN_PLAYER_COUNT)
{
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_SCALE) == SeqTitle::SPRITE_TYPE_MAX);
    m_Language = TbSystemSetting::GetLanguage();
#if CMN_ENABLE_BUILD_SOWN
    m_BitArray.SetBit(FLAG_SKIP_SELECT_CHARA);
#else
    if( !isEnableSelectPlayer ) {
        m_BitArray.SetBit(FLAG_SKIP_SELECT_CHARA);
    }
#endif
    SeqManager::GetInstance().ClearCacheSeq();

    // 元に戻すキャラ入力を
    for( size_t i = 0; i < CMN_PLAYER_MAX; ++i ) {
        CrInput::SetPlayerPadIndex(i,i);
    }
    Root::GetInstance().SetPlayerCount(m_PlayerCount);

    // チェックポイントリセット
    Root::GetInstance().GetGlobalData().ResetCheckPoint();

    m_SpriteMaterial.SetTexture(Root::GetInstance().GetCommonTexture());
    m_SpriteMaterialAutoSave.SetTexture(Root::GetInstance().GetAutoSaveTexture());
#if ENABLE_PLAYSTYLE_IMAGE
    m_SpriteMaterialPlayStyle.SetTexture(Root::GetInstance().GetPlayStyleTexture());
#endif

    // スプライト設定
    for( size_t i = 0; i < TB_ARRAY_COUNT_OF(m_Sprite); ++i )
    {
        m_Sprite[i].sprite.Initialize(SPRITE_RECT[i], SPRITE_UV[i]);
        m_Sprite[i].sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
        m_Sprite[i].sprite.SetMaterial(&m_SpriteMaterial);
        m_Sprite[i].sprite.SetTranslate(SPRITE_POS[i]);
        m_Sprite[i].sprite.SetScale(SPRITE_SCALE[i]);
    }
    m_Sprite[SPRITE_TYPE_AUTOSAVE_EN].sprite.SetMaterial(&m_SpriteMaterialAutoSave);
    m_Sprite[SPRITE_TYPE_AUTOSAVE_JP].sprite.SetMaterial(&m_SpriteMaterialAutoSave);
#if ENABLE_PLAYSTYLE_IMAGE
    m_Sprite[SPRITE_TYPE_PLAYSTYLE_JOYCON].sprite.SetMaterial(&m_SpriteMaterialPlayStyle);
    m_Sprite[SPRITE_TYPE_PLAYSTYLE_PROCON].sprite.SetMaterial(&m_SpriteMaterialPlayStyle);
#endif
    m_Sprite[SPRITE_TYPE_ADVERTISE].sprite.SetAlpha(0.0f);
    if( isEnableSkipAdvertise ){
        setPhase(PHASE_TITLE);
    }

    // 確認ウィンドウ
    m_ConfirmWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f+120.0f));

    // 
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);
    TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");
    res = node->DoFileSync(SCRIPT_FILE);
    node->GoToAbsolutely("stage_title");


    m_Stage = new SideActionStage(node);
    node->GoToAbsolutely("");
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqTitle::~SeqTitle()
{
    TB_SAFE_DELETE(m_Stage);
}

/*!
 * 更新
 * @author teco
 * @since 2013.09.07
 */
void SeqTitle::OnUpdate()
{
    m_PhaseTime += 1/60.0f;
    switch( m_Phase )
    {
    case PHASE_ADVERTISE:
        {
            FadeIn(0.0f);
            CreateBgm("title_bgm");
            addPhase();
        }
        // break htrough;
    case PHASE_ADVERTISE+1:
        {
            if( !IsReadyBgm() ){
                break;
            }
#if 1 // 起動画面を待ちたい場合
            if( m_PhaseTime < 0.0f ){
                break;
            }
#endif
            PlayBgm();
            setPhase(m_Phase+1);
        }
    case PHASE_ADVERTISE+2:
        {
            TbFloat32 alpha = m_PhaseTime/ADVERTISE_FADE_IN;
            if( alpha >= 1.0 ){
                alpha = 1.0f;
                setPhase(m_Phase+1);
            }
            m_Sprite[SPRITE_TYPE_ADVERTISE].sprite.SetAlpha(alpha);
        }
        break;
    case PHASE_ADVERTISE+3:
        {
            if( m_PhaseTime >= ADVERTISE_FADE_WAIT ){
                FadeOut(ADVERTISE_FADE_OUT);
                setPhase(m_Phase+1);
            }
        }
        break;
    case PHASE_ADVERTISE+4:
        {
            if( !IsFading() ){
#if CMN_ENABLE_BUILD_100P
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE,12);
#else
                setPhase(PHASE_AUTOSAVE);
#endif
            }
        }
        break;
    case PHASE_AUTOSAVE:
        {
            FadeIn(0.25f);
            addPhase();
        }
        break;
    case PHASE_AUTOSAVE+1:
        {
            if (AUTOSAVE_WAIT <= m_PhaseTime) {
                addPhase();
                FadeOut(0.25f);
            }
        }
        break;
    case PHASE_AUTOSAVE + 2:
        {
            if (IsFading()) {
                break;
            }
            if (m_PhaseTime <= 0.4f) {
                break;
            }
            setPhase(PHASE_TITLE);
        }
        break;
    case PHASE_TITLE:
        {
            CreateBgm("title_bgm");
            addPhase();
        }
        // break htrough;
    case PHASE_TITLE+1:
        {
            if (IsFading()) {
                break;
            }
            if (!IsReadyBgm()) {
                break;
            }
            PlayBgm();
            FadeIn(0.0f);
            m_Stage->SetActive(TB_TRUE);
            m_Stage->SetPause(TB_FALSE);
            SaveFile::GetInstance().SetBgColorType(SaveFile::GetInstance().GetBgColorType());
            setPhase(PHASE_TITLE_MAIN);
        }
        break;
    case PHASE_TITLE_MAIN:
        {
#if CMN_ENABLE_BUILD_GIANT_INPUT
            if( SysGiantInput::IsPress(CrInput::BUTTON_DECIDE,0) ) {
                m_PlayerCount = 1;
                m_Stage->RequestOpanGoal();
                setPhase(PHASE_TITLE_ENTER_DOOR);
            }
#else
            TbBool isMousePress = TB_FALSE;
            {
                TbRectF32 screenRect;
                if( Root::GetInstance().ConvertGameRectToScreenRect(&screenRect,m_PressTextRect) ){
                    if( TbMouse::IsOver(screenRect,TB_MOUSE_POS_TYPE_WINDOW) ) {
                        if( TbMouse::IsPress(TB_MOUSE_LEFT_BIT) ){
                           isMousePress = TB_TRUE; 
                        }
                    }
                }
            }

#if CMN_ENABLE_BUILD_OLD && TB_IS_DEBUG
            if (TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_K)) {
                m_KeyboardConfigWindow.SetPlayerBit(1);
                m_KeyboardConfigWindow.Open();
                m_KeyboardConfigWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f));
                setPhase(PHASE_TITLE_KEYBOARD_CONFIG);
                break;
            }
#endif

            for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_COUNT; ++playerIndex ){
                if( CrInput::IsPress(CrInput::BUTTON_START,playerIndex) || isMousePress ) {
                    if( m_BitArray.Test(FLAG_SKIP_SELECT_CHARA) ){
                        m_PlayerCount = CMN_PLAYER_COUNT;
                        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PadPlayerIndex); ++i ){
                            m_PadPlayerIndex.PushBack(i);
                        }
                        setPhase(PHASE_TITLE_NEXT_STAGE);
                    }else{
                        m_Stage->SetPause(TB_TRUE);
                        m_MainSelectWindow.SetPlayerBit(TB_BIT(playerIndex));
                        m_MainSelectWindow.Open();
                        m_MainSelectWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f+120.0f));
                        setPhase(PHASE_TITLE_CHARA_SELECT);
                    }
                    break;
                }
            }
#endif
        }
        break;
    case PHASE_TITLE_CHARA_SELECT:
        {
            MainSelectWindow::Result res = m_MainSelectWindow.GetResult();
            if( res == MainSelectWindow::RESULT_NONE ) {
                break;
            }
            if( res == MainSelectWindow::RESULT_OK ){
                setPhase(PHASE_TITLE_CONFIRM);
            }else{
                m_Stage->SetPause(TB_FALSE);
                setPhase(PHASE_TITLE);
            }
        }
        break;
    case PHASE_TITLE_CONFIRM: // 確認
        {
            TbStaticString128 str;
            str.SetFormatedString("%s ?" , m_MainSelectWindow.GetSelectedString() );
            m_ConfirmWindow.SetMessage(str.GetBuffer());
            m_ConfirmWindow.SetPlayerBit(m_MainSelectWindow.GetPlayerBit());
            m_ConfirmWindow.Open();
        }
        addPhase();
        break;
    case PHASE_TITLE_CONFIRM+1:
        if( UiMessageWindow::RESULT_NONE != m_ConfirmWindow.GetResult() )
        {
            if( UiMessageWindow::RESULT_YES == m_ConfirmWindow.GetResult() ) {
#if CMN_ENABLE_BUILD_OLD
                TbSint32 index = m_SelectWindow.GetSelectedIndex();
                if( UiTitleSelectWindow::ITEM_GAME_BEGIN <= index &&
                    index < UiTitleSelectWindow::ITEM_GAME_END )
                {
                    m_PlayerCount = m_SelectWindow.GetSelectedIndex()+2;
                    m_Stage->RequestOpanGoal();
                    m_Stage->SetPause(TB_FALSE);
                    setPhase(PHASE_TITLE_ENTER_DOOR);
                }
                else{
#if UI_TITLE_SELECT_ENABLE_OPTION
                    switch( index )
                    {
                    case UiTitleSelectWindow::ITEM_GAMEPAD_CONFIG:
                        m_GamePadConfigWindow.SetPlayerBit(m_ConfirmWindow.GetPlayerBit());
                        m_GamePadConfigWindow.Open();
                        m_GamePadConfigWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
                        setPhase(PHASE_TITLE_GAMEPAD_CONFIG);
                        break;
#if TB_PLATFORM_IS_WIN
                    case UiTitleSelectWindow::ITEM_KEYBOARD_CONFIG:
                        m_KeyboardConfigWindow.SetPlayerBit(m_ConfirmWindow.GetPlayerBit());
                        m_KeyboardConfigWindow.Open();
                        m_KeyboardConfigWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
                        setPhase(PHASE_TITLE_KEYBOARD_CONFIG);
                        break;
#endif
                    case UiTitleSelectWindow::ITEM_OPTION:
                        m_OptionWindow.SetPlayerBit(m_ConfirmWindow.GetPlayerBit());
                        m_OptionWindow.Open();
                        m_OptionWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
                        setPhase(PHASE_TITLE_OPTION);
                        break;
#if TB_PLATFORM_IS_WIN
                    case UiTitleSelectWindow::ITEM_EXIT_GAME:
                        TbFramework::FinishMainLoop();
                        break;
#endif
                    }
#endif
                }
#else
                m_PlayerCount = m_MainSelectWindow.GetPlayerCount();
                m_Stage->RequestOpanGoal();
                m_Stage->SetPause(TB_FALSE);
                setPhase(PHASE_TITLE_ENTER_DOOR);
#endif
            }else{
                m_MainSelectWindow.Open();
                setPhase(PHASE_TITLE_CHARA_SELECT);
            }
        }
        break;

    case PHASE_TITLE_ENTER_DOOR:
        {
            TbUint32 clearedPlayerCount = m_Stage->GetClearedPlayerCount();
            if( clearedPlayerCount >= m_PlayerCount ){
                m_Stage->SetPause(TB_TRUE);
                // 登録
                for( TbUint32 i = 0; i < CMN_PLAYER_COUNT; ++i ) {
                    SideActionPlayer* player = m_Stage->GetPlayer(i);
                    if( player->IsCleared() ){
                        m_PadPlayerIndex.PushBack(i);
                        if( m_PlayerCount==m_PadPlayerIndex.GetCount() ) {
                            break;
                        }
                    }
                }
                setPhase(PHASE_TITLE_NEXT_STAGE);
            }else{
                for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_COUNT; ++playerIndex ){
                    if( CrInput::IsPress(CrInput::BUTTON_START,playerIndex) ) {
                        m_Stage->SetPause(TB_TRUE);
                        m_ConfirmWindow.SetMessage("    RESET?    ");
                        m_ConfirmWindow.SetPlayerBit(TB_BIT(playerIndex));
                        m_ConfirmWindow.Open();
                        setPhase(PHASE_TITLE_RESET);
                        break;
                    }
                }
            }
        }
        break;
    case PHASE_TITLE_NEXT_STAGE:
        {
            FadeOut(1.0f);
            FadeOutBgm(1.0f);
            addPhase();
        }
        break;

    case PHASE_TITLE_NEXT_STAGE+1:
        {
            if (IsFading()) {
                break;
            }
            if(Root::GetInstance().IsBusyUserData()){
                break;
            }
            TB_ASSERT(m_PlayerCount==m_PadPlayerIndex.GetCount());
            // 反映
            for( size_t i = 0; i < m_PlayerCount; ++i ) {
                CrInput::SetPlayerPadIndex(i,m_PadPlayerIndex[i]);
            }
            Root::GetInstance().SetPlayerCount(m_PlayerCount);
#if CMN_ENABLE_BUILD_GIANT_INPUT
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE,0);
#else
            SeqManager::GetInstance().RequestSequence(SEQ_MAIN_MENU, 0);
#endif
        }
        break;
    case PHASE_TITLE_RESET:
        {
            if( UiMessageWindow::RESULT_NONE != m_ConfirmWindow.GetResult() ) 
            {
                if( UiMessageWindow::RESULT_YES == m_ConfirmWindow.GetResult() ) {
                    FadeOut(0.5f);
                    FadeOutBgm(0.5f);
                    addPhase();
                }else{
                    m_Stage->SetPause(TB_FALSE);
                    setPhase(PHASE_TITLE_ENTER_DOOR);
                }
            }
        }
        break;
    case PHASE_TITLE_RESET+1:
        {
            if (IsFading()) {
                break;
            }        
            SeqManager::GetInstance().RequestSequence(SEQ_TITLE, 1);
        }
        break;

    case PHASE_TITLE_GAMEPAD_CONFIG: // ゲームパッドコンフィグ
        {
            if( UiGamePadConfigWindow::RESULT_NONE != m_GamePadConfigWindow.GetResult() ){
                setPhase(m_Phase+1);
            }
        }
        break;
    case PHASE_TITLE_GAMEPAD_CONFIG+1: // キーコンフィグ
        {
#if UI_TITLE_SELECT_ENABLE_OPTION
            m_SelectWindow.SetSelectedIndex( UiTitleSelectWindow::ITEM_GAMEPAD_CONFIG );
            m_SelectWindow.Open();
            setPhase(PHASE_TITLE_CHARA_SELECT);
#endif
        }
        break;

    case PHASE_TITLE_KEYBOARD_CONFIG: // キーボードコンフィグ
        {
            if( UiKeyboardConfigWindow::RESULT_NONE != m_KeyboardConfigWindow.GetResult() ){
                setPhase(m_Phase+1);
            }
        }
        break;
    case PHASE_TITLE_KEYBOARD_CONFIG+1: // キーボードコンフィグ
        {
#if UI_TITLE_SELECT_ENABLE_OPTION && TB_PLATFORM_IS_WIN
            m_SelectWindow.SetSelectedIndex( UiTitleSelectWindow::ITEM_KEYBOARD_CONFIG );
            m_SelectWindow.Open();
            setPhase(PHASE_TITLE_CHARA_SELECT);
#endif
        }
        break;

    case PHASE_TITLE_OPTION:
        {
#if UI_TITLE_SELECT_ENABLE_OPTION
            if( UiOptionWindowOld::RESULT_NONE != m_OptionWindow.GetResult() ){
                setPhase(m_Phase+1);
            }
#endif
        }
        break;
    case PHASE_TITLE_OPTION+1:
#if UI_TITLE_SELECT_ENABLE_OPTION
        m_SelectWindow.SetSelectedIndex( UiTitleSelectWindow::ITEM_OPTION );
        m_SelectWindow.Open();
        setPhase(PHASE_TITLE_CHARA_SELECT);
#endif
        break;
    }

    m_Stage->Update();

    if( PHASE_ADVERTISE <= m_Phase && m_Phase < PHASE_TITLE ){
        m_Sprite[SPRITE_TYPE_ADVERTISE].sprite.Calculate();
    }else if(PHASE_AUTOSAVE <= m_Phase && m_Phase < PHASE_AUTOSAVE+10){
        m_Sprite[SPRITE_TYPE_AUTOSAVE_EN].sprite.Calculate();
        m_Sprite[SPRITE_TYPE_AUTOSAVE_JP].sprite.Calculate();
        m_Sprite[SPRITE_TYPE_SAVEICON].sprite.Calculate();
    }else{
        m_Sprite[SPRITE_TYPE_PAD_JOYCON].sprite.Calculate();
        m_Sprite[SPRITE_TYPE_PAD_PROCON].sprite.Calculate();
#if ENABLE_PLAYSTYLE_IMAGE
        m_Sprite[SPRITE_TYPE_PLAYSTYLE_JOYCON].sprite.Calculate();
        m_Sprite[SPRITE_TYPE_PLAYSTYLE_PROCON].sprite.Calculate();
#endif
    }
}

/*!
 * 描画関数
 * @author teco
 * @since 2013.10.27
 */
void SeqTitle::OnDraw()
{
    if( PHASE_ADVERTISE <= m_Phase && m_Phase < PHASE_TITLE ){
        m_Sprite[SPRITE_TYPE_ADVERTISE].sprite.Draw(TbMatrix::IDENTITY);
    }else if(PHASE_AUTOSAVE <= m_Phase && m_Phase < PHASE_AUTOSAVE+10){
        if(m_Language == TB_LANG_JP){
            m_Sprite[SPRITE_TYPE_AUTOSAVE_JP].sprite.Draw(TbMatrix::IDENTITY);
        }else{
            m_Sprite[SPRITE_TYPE_AUTOSAVE_EN].sprite.Draw(TbMatrix::IDENTITY);
        }
        m_Sprite[SPRITE_TYPE_SAVEICON].sprite.Draw(TbMatrix::IDENTITY);
    }else{
        TbFontBuffer2d* fontBuffer = Root::GetInstance().GetLocalFont();
        fontBuffer->PushSetting();
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        fontBuffer->SetColor(CmnUtil::GetMainColor());
#if CMN_ENABLE_BUILD_GIANT_INPUT
        fontBuffer->Printf(static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
                            static_cast<TbSint32>(CMN_WINDOW_HEIGHT/1.9f),"- BIG CONTROLLER EDITION -");
#endif

        if( PHASE_TITLE <= m_Phase && m_Phase < PHASE_TITLE_CHARA_SELECT ){
            // PUSH START
            TbUint32 time = static_cast<TbUint32>(m_PhaseTime * 1000.0f);
            if( (time/1000)%2 == 0 ){
                fontBuffer->SetPoseMatrix(TbMatrix::IDENTITY);
#if CMN_ENABLE_BUILD_GIANT_INPUT
                fontBuffer->Printf(m_PressTextRect,
                                   static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
                                   static_cast<TbSint32>(CMN_WINDOW_HEIGHT/1.5f),"PRESS A");
#elif TB_PLATFORM_IS_NX || 0
                if (0 == TbPad::GetPadCount())
                {
#if !ENABLE_PLAYSTYLE_IMAGE
                    fontBuffer->Printf(m_PressTextRect,
                        static_cast<TbSint32>(CMN_WINDOW_WIDTH / 2),
                        static_cast<TbSint32>(CMN_WINDOW_HEIGHT / 1.5f), "CONNECT        OR        ");
                    m_Sprite[SPRITE_TYPE_PAD_JOYCON].sprite.Draw(TbMatrix::IDENTITY);
                    m_Sprite[SPRITE_TYPE_PAD_PROCON].sprite.Draw(TbMatrix::IDENTITY);
#else
                    m_Sprite[SPRITE_TYPE_PLAYSTYLE_JOYCON].sprite.Draw(TbMatrix::IDENTITY);
#endif
                }
                else
                {
                    fontBuffer->Printf(m_PressTextRect,
                        static_cast<TbSint32>(CMN_WINDOW_WIDTH / 2),
                        static_cast<TbSint32>(CMN_WINDOW_HEIGHT / 1.5f), "PRESS -/+ BUTTON");
                }
#else
                fontBuffer->Printf(m_PressTextRect,
                                   static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
                                   static_cast<TbSint32>(CMN_WINDOW_HEIGHT/1.5f),"PRESS START");
#endif
            }
        }else if( PHASE_TITLE_CHARA_SELECT == m_Phase ) {
            // 参加人数決定
//            fontBuffer->Printf(static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
//                               static_cast<TbSint32>(CMN_WINDOW_HEIGHT/1.5f),"< %02u PLAYERS GAME >",m_PlayerCount);
        }else if( m_Phase >= PHASE_TITLE_ENTER_DOOR && m_Phase < PHASE_TITLE_OPTION ){
            // ドアに入っている人数
            TbUint32 clearedPlayerCount = m_Stage->GetClearedPlayerCount();
            SideActionGoal* goal = m_Stage->GetGoal();
            if( goal ) {
                TbVector4 pos = goal->GetPos();
                static TB_NDEBUG_CONST TbFloat32 OFFSET_Y = 100.0f;
                fontBuffer->SetPoseMatrix(m_Stage->GetPoseMatrix());
                fontBuffer->Printf(static_cast<TbSint32>(pos.GetX()),
                                   static_cast<TbSint32>(pos.GetY()-OFFSET_Y),
                                   "%02u/%02u",
                                   clearedPlayerCount,
                                   m_PlayerCount);
            }
        }

        fontBuffer->PopSetting();
        fontBuffer->Flush();

        m_Stage->Draw();
    }
}

