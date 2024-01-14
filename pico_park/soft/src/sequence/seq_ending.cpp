/*!
 * エンディングシーケンス
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_ending.h"
#include "seq_manager.h"
#include "root.h"
#include "common/cmn_util.h"
#include <graphics/font/tb_font_buffer_2d.h>

static const char   SCRIPT_FILE[] = "stage/" CMN_STAGE_DIR "stage_ending.lua";
static TbBool       s_IsForceStart = TB_FALSE;
static TB_NDEBUG_CONST TbFloat32 FADEOUT_SCROLL_BORDER = -3400;

enum
{
    PHASE_GAME , 
    PHASE_STAFF_ROLL = 10 ,
    PHASE_THANK_YOU  = 20
};

/*!
 * コンストラクタ
 * @author teco
 */
SeqEnding::SeqEnding()
    : Super(SEQ_ENDING,"SeqEnding")
    , m_PhaseSec(0.0f)
    , m_Stage(nullptr)
{
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);
    TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");
    res = node->DoFileSync(SCRIPT_FILE);
    node->GoToAbsolutely("stage_ending");
    m_Stage = new SideActionStage(node);
    node->GoToAbsolutely("");

    m_BgSprite.Initialize(TbRectF32(0.0f, 
                                    m_Stage->GetWindowHeight(),
                                    m_Stage->GetWindowWidth(), 
                                    m_Stage->GetWindowHeight()*100));
    m_BgSprite.SetColor(CmnUtil::GetMainColor());
}

/*!
 * デストラクタ
 * @author teco
 */
SeqEnding::~SeqEnding()
{
    TB_SAFE_DELETE(m_Stage);
    if (m_DebugMenuForceStart) {
        m_DebugMenuForceStart->Suicide();
    }
}

/*!
 * 更新
 * @author teco
 */
void SeqEnding::OnUpdate()
{
    switch (GetPhase())
    {
    case PHASE_GAME:
        CreateBgm("title_bgm");
        AddPhase(1);
        break;
    case PHASE_GAME+1:
        m_PhaseSec += 1 / 60.0f;
        if (m_PhaseSec < 1.0f) {
            break;
        }
        if (!IsReadyBgm()) {
            break;
        }
        FadeIn(4.0f);
        m_Stage->SetActive(TB_TRUE);
        m_Stage->SetPause(TB_FALSE);
        AddPhase(1);
        m_PhaseSec = 0.0f;
        break;
    case PHASE_GAME+2:
        if (IsFading()) {
            break;
        }
        if (m_Stage->IsRequestedClear() || s_IsForceStart)
        {
            m_Stage->SetPause(TB_TRUE);
            PlayBgm();
            // スクロール開始
            SetPhase(PHASE_STAFF_ROLL);
        }
        break;

    case PHASE_STAFF_ROLL:
        {
            TbMatrix pose = m_Stage->GetSpriteScene(0).GetPoseMatrix();
            pose.AddTranslation(TbVector3(0.0f,-1.0f,0.0f));
            m_Stage->GetSpriteScene(0).SetPoseMatrix(pose);

            TbVector3 t;
            pose.GetTranslation(t);
            if (t.GetY() < FADEOUT_SCROLL_BORDER)
            {
                FadeOut(2.0f);
                AddPhase(1);
            }
        }
        break;
    case PHASE_STAFF_ROLL+1:
        {
            if (IsFading()) {
                break;
            }
            m_Stage->SetActive(TB_FALSE);
            TbMatrix pose = m_Stage->GetSpriteScene(0).GetPoseMatrix();
            pose.SetTranslation(TbVector3());
            m_Stage->GetSpriteScene(0).SetPoseMatrix(pose);
            SetPhase(PHASE_THANK_YOU);
        }
        break;
    case PHASE_THANK_YOU:
        {
            FadeIn(2.0f);
            AddPhase(1);
        }
        break;
    case PHASE_THANK_YOU+1:
        {
            if (IsFading()) {
                break;
            }
            TbBool isNext = TB_FALSE;
            for (TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_COUNT; ++playerIndex) {
                if (CrInput::IsPress(CrInput::BUTTON_START, playerIndex)) {
                    TbPlaySound("win");
                    isNext = TB_TRUE;
                    break;
                }
            }
            m_PhaseSec += 1 / 60.0f;
            if (30.0f < m_PhaseSec) {
                isNext = TB_TRUE;
            }
            if (isNext) 
            {
                FadeOut(2.0f);
                AddPhase(1);
            }
        }
        break;
    case PHASE_THANK_YOU + 2:
        {
            if (IsFading()) {
                break;
            }
            SeqManager::GetInstance().RequestSequence(SEQ_TITLE,0);
        }
        break;
    }
    m_Stage->Update();
}

/*!
* 描画関数
* @author teco
*/
void SeqEnding::OnDraw()
{
    if (GetPhase() < PHASE_THANK_YOU)
    {
        m_BgSprite.Calculate();
        TbMatrix mtx = m_Stage->GetSpriteScene(0).GetPoseMatrix();
        m_BgSprite.Draw(mtx);
        m_Stage->Draw();
    }
    else
    {
        drawThankYouForPlaying();
        if (1.0f <= m_PhaseSec) {
            static TbFloat32 FONT_SIZE = 32.0f;
            static TbFloat32 FONT_MARGIN = 32.0f;
#if TB_PLATFORM_IS_WIN
            static const char* TEXT = "PRESS START BUTTON";
#else
            static const char* TEXT = "PRESS -/+ BUTTON";
#endif
            TbFontBuffer2d* font = Root::GetInstance().GetFont();
            font->PushSetting();
            font->SetColor(CmnUtil::GetMainColor());
            font->SetFontSize(FONT_SIZE);
            font->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
            font->SetAlignY(TbFontBuffer2d::ALIGN_Y_DOWN);
            font->Printf(CMN_WINDOW_WIDTH - FONT_MARGIN, CMN_WINDOW_HEIGHT- FONT_MARGIN, TEXT);
            font->PopSetting();
        }
    }
}

/*!
* デバッグメニュー生成
* @author teco
*/
void SeqEnding::OnCreateDebugMenu(TbDebugMenu* parent)
{
    m_DebugMenuForceStart = parent->CreateItemBool("StartStaffroll");
    m_DebugMenuForceStart->Bind(&s_IsForceStart);
}

/*!
* 描画
* @author teco
*/
void SeqEnding::drawThankYouForPlaying()
{
    static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 64.0f;
    static TB_NDEBUG_CONST TbFloat32 FONT_X = CMN_WINDOW_WIDTH / 2.0f;
    static TB_NDEBUG_CONST TbFloat32 FONT_Y = 200.0f;
    static TB_NDEBUG_CONST TbFloat32 OFFSET_CENTER_Y = 20.0f;
    struct PlayerPose
    {
        TbFloat32 x;
        TbFloat32 y;
        TbBool    isLeft;
    };
    static TB_NDEBUG_CONST PlayerPose PLAYER_POSE[] =
    {
        { 560.0f , 360.0f , TB_FALSE } ,
        { 740.0f , 400.0f , TB_TRUE } ,
        { 640.0f , 490.0f , TB_TRUE } ,
        { 440.0f , 420.0f , TB_FALSE } ,
        { 540.0f , 560.0f , TB_FALSE } ,
        { 800.0f , 560.0f , TB_TRUE } ,
        { 880.0f , 420.0f , TB_TRUE } ,
        { 400.0f , 540.0f , TB_FALSE } ,
        { 0.0f , 0.0f , TB_FALSE } ,
        { 0.0f , 0.0f , TB_FALSE } ,
    };

    TbFloat32 offsetX = 0.0f;
    TbFloat32 offsetY = FONT_Y;
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        offsetX += PLAYER_POSE[i].x;
        offsetY += PLAYER_POSE[i].y;
    }
    offsetX /= static_cast<TbFloat32>(playerCount);
    offsetY /= static_cast<TbFloat32>(playerCount+1);
    offsetX -= CMN_WINDOW_WIDTH / 2.0f;
    offsetY -= (CMN_WINDOW_HEIGHT / 2.0f) + OFFSET_CENTER_Y;

    // テキスト
    {
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        font->PushSetting();
        font->SetColor(CmnUtil::GetMainColor());
        font->SetFontSize(FONT_SIZE);
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        font->Printf(FONT_X, FONT_Y- offsetY, "THANK YOU FOR PLAYING");
        font->PopSetting();
    }

    // 位置
    {

        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(PLAYER_POSE)==CMN_PLAYER_MAX);
        static TB_NDEBUG_CONST TbFloat32 ICON_W = 22.0f;
        static TB_NDEBUG_CONST TbFloat32 ICON_H = 24.0f;
        static TB_NDEBUG_CONST TbFloat32 ICON_SCALE = 4.0f;
        
        static TB_NDEBUG_CONST TbRectF32Pod ICON_RECT =
        {
            -ICON_W*ICON_SCALE*0.5f ,
            -ICON_H*ICON_SCALE*0.5f ,
            ICON_W*ICON_SCALE , 
            ICON_H*ICON_SCALE
        };
        static TB_NDEBUG_CONST TbRectF32Pod ICON_UV =
        {
            CMN_TEX_UV_RECT(32 * 5 + 5,8,ICON_W,ICON_H)
        };
        m_PlayerMaterial.BeginDraw();
        for (TbUint32 i = 0; i < playerCount; ++i)
        {
            TbMatrix pose = TbMatrix::IDENTITY;
            pose.Translate(PLAYER_POSE[i].x-offsetX, PLAYER_POSE[i].y- offsetY,0.0f);
            if (PLAYER_POSE[i].isLeft) {
                pose.Scale(-1.0f,1.0f,1.0f);
            }
            m_PlayerMaterial.Draw(ICON_RECT, ICON_UV, CmnUtil::GetPlayerColor(i), pose);
        }
        m_PlayerMaterial.EndDraw();
    }
}
