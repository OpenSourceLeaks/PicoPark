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
#include "system/sys_input.h"
#include "system/sys_giant_input.h"
#include <input/tb_pad.h>
#include <input/tb_mouse.h>
#include <framework/tb_framework.h>

static const TbRectF32Pod SPRITE_RECT[] = 
{
    { -29 , -10 , 58 , 20},
};
static const TbRectF32Pod SPRITE_UV[] =
{
    { CMN_TEX_UV_RECT(320,320,29,10) },
};
static const TbVector2 SPRITE_POS[] =
{
    TbVector2(CMN_WINDOW_WIDTH/2,CMN_WINDOW_HEIGHT/2) ,
};
static const TbFloat32 SPRITE_SCALE[] =
{
    5.0f ,
    10.0f
};

static TbFloat32 ADVERTISE_FADE_IN   = 2.0f;
static TbFloat32 ADVERTISE_FADE_WAIT = 2.0f;
static TbFloat32 ADVERTISE_FADE_OUT  = 2.5f;
static TbFloat32 START_STAGE_WAIT = 1.0f;
static TbUint32  PLAYER_MIN = 2U;

#if CMN_ENABLE_BUILD_GIANT_INPUT
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_title_giant.lua";
#elif CMN_ENABLE_BUILD_ARCADE
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_title_arcade.lua";
#else
static const char SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_title.lua";
#endif

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
    , m_Stage(nullptr)
    , m_ConfirmWindow(UiMessageWindow::TYPE_YESNO)
    , m_PlayerCount(CMN_PLAYER_COUNT)
{
#if CMN_ENABLE_BUILD_SOWN
    m_BitArray.SetBit(FLAG_SKIP_SELECT_CHARA);
#else
    if( !isEnableSelectPlayer ) {
        m_BitArray.SetBit(FLAG_SKIP_SELECT_CHARA);
    }
#endif

    // 元に戻すキャラ入力を
    for( size_t i = 0; i < CMN_PLAYER_MAX; ++i ) {
        SysInput::SetPlayerPadIndex(i,i);
    }
    Root::GetInstance().SetPlayerCount(m_PlayerCount);

    // チェックポイントリセット
    Root::GetInstance().GetGlobalData().ResetCheckPoint();

    // クリア情報リセット
    Root::GetInstance().GetGlobalData().ResetWorldData();

    m_SpriteMaterial.SetTexture(Root::GetInstance().GetCommonTexture());

    // スプライト設定
    for( size_t i = 0; i < TB_ARRAY_COUNT_OF(m_Sprite); ++i )
    {
        TbSpriteNodeTree::InitNodeParam initParam;
        initParam.name = "root";
        initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
        initParam.param.rect = SPRITE_RECT[i]; //
        initParam.param.uvRect = SPRITE_UV[i]; //
        m_Sprite[i].node.Initialize(initParam);
        m_Sprite[i].sprite.Initialize(&m_Sprite[i].node);
        m_Sprite[i].sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
        m_Sprite[i].sprite.SetMaterial(&m_SpriteMaterial);
        m_Sprite[i].sprite.SetTranslate(SPRITE_POS[i]);
        m_Sprite[i].sprite.SetScale(SPRITE_SCALE[i]);
    }

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
                setPhase(m_Phase+1);
            }
        }
        break;
    case PHASE_ADVERTISE+4:
        {
            TbFloat32 alpha = m_PhaseTime/ADVERTISE_FADE_OUT;
            if( alpha >= 1.0 ){
                alpha = 1.0f;
#if CMN_ENABLE_BUILD_100P
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE,12);
#else
                setPhase(PHASE_TITLE);
#endif
            }
            m_Sprite[SPRITE_TYPE_ADVERTISE].sprite.SetAlpha(1.0f-alpha);
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
            if (!IsReadyBgm()) {
                break;
            }
            PlayBgm();
            FadeIn(0.0f);
            m_Stage->SetActive(TB_TRUE);
            m_Stage->SetPause(TB_FALSE);
            addPhase();
        }
        break;
    case PHASE_TITLE+2:
        {
#if CMN_ENABLE_BUILD_GIANT_INPUT
            if( SysGiantInput::IsPress(SysInput::BUTTON_DECIDE,0) ) {
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
            for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_COUNT; ++playerIndex ){
                if( SysInput::IsPress(SysInput::BUTTON_START,playerIndex) || isMousePress ) {
#if CMN_ENABLE_BUILD_OLD
                    if( m_BitArray.Test(FLAG_SKIP_SELECT_CHARA) ){
                        m_PlayerCount = CMN_PLAYER_COUNT;
                        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PadPlayerIndex); ++i ){
                            m_PadPlayerIndex.PushBack(i);
                        }
                        setPhase(PHASE_TITLE_NEXT_STAGE);
                    }else{
                        m_Stage->SetPause(TB_TRUE);
                        m_SelectWindow.SetPlayerBit(TB_BIT(playerIndex));
                        m_SelectWindow.Open();
                        m_SelectWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f+120.0f));
                        setPhase(PHASE_TITLE_CHARA_SELECT);
                    }
                    break;
#else
                    FadeOut(1.0f);
                    setPhase(PHASE_TITLE_NEXT_STAGE);
#endif
                }
            }
#endif
        }
        break;
    case PHASE_TITLE_CHARA_SELECT:
        {   
            UiSelectWindow::Result res = m_SelectWindow.GetResult();
            if( res == UiSelectWindow::RESULT_NONE ) {
                break;
            }

            if( res == UiSelectWindow::RESULT_OK ){
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
            str.SetFormatedString("%s ?" , m_SelectWindow.GetSelectedString() );
            m_ConfirmWindow.SetMessage(str.GetBuffer());
            m_ConfirmWindow.SetPlayerBit(m_SelectWindow.GetPlayerBit());
            m_ConfirmWindow.Open();
        }
        addPhase();
        break;
    case PHASE_TITLE_CONFIRM+1:
        if( UiMessageWindow::RESULT_NONE != m_ConfirmWindow.GetResult() )
        {
            if( UiMessageWindow::RESULT_YES == m_ConfirmWindow.GetResult() ) {
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
            }else{
                m_SelectWindow.Open();
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
                setPhase(m_Phase+1);
            }else{
                for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_COUNT; ++playerIndex ){
                    if( SysInput::IsPress(SysInput::BUTTON_START,playerIndex) ) {
                        m_Stage->SetPause(TB_TRUE);
                        m_ConfirmWindow.SetMessage("        Reset ?       ");
                        m_ConfirmWindow.SetPlayerBit(TB_BIT(playerIndex));
                        m_ConfirmWindow.Open();
                        setPhase(PHASE_TITLE_RESET);
                        break;
                    }
                }
            }
        }
        break;
    case PHASE_TITLE_ENTER_DOOR+1:
        {
            if( m_PhaseTime >= START_STAGE_WAIT ){
                setPhase(PHASE_TITLE_NEXT_STAGE);
            }
        }
        break;

    case PHASE_TITLE_NEXT_STAGE:
        {
            if (IsFading()) {
                break;
            }
#if CMN_ENABLE_BUILD_OLD
            TB_ASSERT(m_PlayerCount==m_PadPlayerIndex.GetCount());
            // 反映
            for( size_t i = 0; i < m_PlayerCount; ++i ) {
                SysInput::SetPlayerPadIndex(i,m_PadPlayerIndex[i]);
            }
            Root::GetInstance().SetPlayerCount(m_PlayerCount);
#if CMN_ENABLE_BUILD_GIANT_INPUT
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE,0);
#else
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_SELECT,0);
#endif
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
                    SeqManager::GetInstance().RequestSequence(SEQ_TITLE,1);
                }
                m_Stage->SetPause(TB_FALSE);
                setPhase(PHASE_TITLE_ENTER_DOOR);
            }
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
            if( UiOptionWindow::RESULT_NONE != m_OptionWindow.GetResult() ){
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
    }else{
        m_Stage->Draw();

        TbFontBuffer2d* fontBuffer = Root::GetInstance().GetFont();
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
#if CMN_ENABLE_BUILD_GIANT_INPUT
                fontBuffer->Printf(m_PressTextRect,
                                   static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
                                   static_cast<TbSint32>(CMN_WINDOW_HEIGHT/1.5f),"PRESS A");
#elif TB_PLATFORM_IS_NX
                fontBuffer->Printf(m_PressTextRect,
                    static_cast<TbSint32>(CMN_WINDOW_WIDTH / 2),
                    static_cast<TbSint32>(CMN_WINDOW_HEIGHT / 1.5f), "PRESS -/+ BUTTON");
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
#if CMN_ENABLE_BUILD_OLD
            // ドアに入っている人数
            TbUint32 clearedPlayerCount = m_Stage->GetClearedPlayerCount();
            SideActionGoal* goal = m_Stage->GetGoal();
            if( goal ) {
                TbVector4 pos = goal->GetPos();
                static TB_NDEBUG_CONST TbFloat32 OFFSET_Y = 90.0f;
                fontBuffer->SetPoseMatrix(m_Stage->GetPoseMatrix());
                fontBuffer->Printf(static_cast<TbSint32>(pos.GetX()),
                                   static_cast<TbSint32>(pos.GetY()-OFFSET_Y),
                                   "%02u/%02u",
                                   clearedPlayerCount,
                                   m_PlayerCount);
            }
#endif
        }

        fontBuffer->PopSetting();
    }
}

