/*!
 * メインメニュー用ページ
 * @author teco
 */

#include "fwd.h"
#include "main_menu_slide.h"
#include "root.h"
#include "common/cmn_util.h"

//////////////////////////////////////////
// MainMenuModeSelectPage
/////////////////////////////////////////

// コンストラクタ
MainMenuModeSelectPage::MainMenuModeSelectPage()
    : m_State(STATE_MENU)
    , m_Index(0)
    , m_ConfirmWindow(UiMessageWindow::TYPE_YESNO)
{
    TbDimensionF32 size(272.0f, 96.0f);
    TbRectF32 uvRect(CMN_TEX_UV_RECT(0.0f, 704.0f, size.width, size.height));

    TbFloat32 SCALE_X = 1.7f;
    TbFloat32 SCALE_Y = 1.5f;
    size.width  *= SCALE_X;
    size.height *= SCALE_Y;
    TbRectF32 rect(-size.width/2.0f, -size.height/2.0f, size.width, size.height);
    m_CursorSprite.Initialize(rect, uvRect);
    m_CursorSprite.SetColor(TbColor::White());
    SetDepth(0);

    TbVector2 winPos = TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f);
    m_OptionWindow.SetPos(winPos);
    m_ConfirmWindow.SetPos(winPos);
}

// リセット
void MainMenuModeSelectPage::Reset()
{
    m_State = STATE_MENU;
    m_Index = 0;
}

// 定期処理
CmnSlidePage::Result MainMenuModeSelectPage::OnUpdate(TbFloat32 deltasec)
{
    Result result = CmnSlidePage::RESULT_PENDING;
    switch (m_State)
    {
    case STATE_MENU:
        {
            if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_RIGHT, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_Index%MODE_WIDTH != MODE_WIDTH - 1) {
                    ++m_Index;
                }
            } else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_LEFT, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_Index%MODE_WIDTH != 0) {
                    --m_Index;
                }
            } else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_DOWN, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_Index/MODE_WIDTH != MODE_HEIGHT - 1) {
                    m_Index += MODE_WIDTH;
                }
            } else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_UP, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_Index/MODE_WIDTH != 0) {
                    m_Index -= MODE_WIDTH;
                }
            }
            TbUint32 playerIndex = 0;
            if (CrInput::IsAnyonePress(CrInput::BUTTON_DECIDE,CR_INPUT_PLAYER_ALL_BIT,CrInput::MODE_BASE,&playerIndex)) {
                if (MODE_OPTION == GetMode())
                {
                    m_OptionWindow.SetPlayerBit(CR_INPUT_PLAYER_ALL_BIT);
                    m_OptionWindow.Open();
                    m_State = STATE_OPTION;
                }
                else
                {
                    result = CmnSlidePage::RESULT_NEXT;
                }
            }
            else if (CrInput::IsAnyonePress(CrInput::BUTTON_CANCEL, CR_INPUT_PLAYER_ALL_BIT, CrInput::MODE_BASE, &playerIndex)) {
                m_State = STATE_CANCEL;
                m_ConfirmWindow.SetMessage("RETURN TITLE?");
                m_ConfirmWindow.SetPlayerBit(TB_BIT(playerIndex));
                m_ConfirmWindow.Open();
            }
        }
        break;
    case STATE_OPTION:
        if (!m_OptionWindow.IsOpened())
        {
            m_State = STATE_MENU;
        }
        break;
    case STATE_CANCEL:
        {
            if (m_ConfirmWindow.IsOpened()){
                break;
            }
            if (UiMessageWindow::RESULT_YES==m_ConfirmWindow.GetResult()) {
                result = CmnSlidePage::RESULT_PARENT;
            } else {
                m_State = STATE_MENU;
            }
        }
        break;
    }
    m_CursorSprite.Update(deltasec);
    return result;
}

// アクティブ状態が変化した
void MainMenuModeSelectPage::OnChangeActive(TbBool isActive)
{
}

static TbSint32 TEXT_Y = 290;
static TbSint32 FONT_SIZE = 80;

// 描画処理
void MainMenuModeSelectPage::OnDraw(const TbVector2& pos)
{
    static const char* MODE_NAME[] =
    {
        "WORLD" ,
        "ENDLESS" , 
        "BATTLE" ,
        "OPTION" ,
    };

    static TbSint32 TEXT_INTERVAL_X = 260;
    static TbSint32 TEXT_INTERVAL_Y = 140;
    TbFontBuffer2d* font = Root::GetInstance().GetLocalFont();
    TbFontBuffer2d* srcFont = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetSetting(srcFont->GetSetting());
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f) + pos.GetX() - TEXT_INTERVAL_X;
    for (TbUint32 i = 0; i < MODE_MAX; ++i)
    {
        TbUint32 mode = i;
        TbColor color = CmnUtil::GetMainColor();
        if (m_Index != i) {
            color.SetAlphaF32(0.4f);
        }
        font->SetColor(color);
        font->Printf(x + (i%MODE_WIDTH)*2* TEXT_INTERVAL_X,
                     TEXT_Y + (i/ MODE_WIDTH)*TEXT_INTERVAL_Y,
                     MODE_NAME[mode]);
    }
    font->PopSetting();

    if( IsCurrent() )
    {
        static TB_NDEBUG_CONST TbFloat32 CURSOR_OFFSET_Y = 0.0f;
        m_CursorSprite.Calculate();
        TbMatrix mtx;
        mtx.SetTranslation(x + (m_Index%MODE_WIDTH)*2*TEXT_INTERVAL_X,
                           TEXT_Y+ (m_Index/ MODE_WIDTH)*TEXT_INTERVAL_Y+CURSOR_OFFSET_Y, 0.0f);
        m_CursorSprite.Draw(mtx);
    }
    font->Flush();
}

//////////////////////////////////////////
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
MainMenuSlide::MainMenuSlide()
    : Super()
{
    Initialize(PAGE_MAX);
    InsertPage(0,&m_ModeSelectPage);
}

// デストラクタ
MainMenuSlide::~MainMenuSlide()
{
}

// リスタート
void MainMenuSlide::Restart()
{
    m_ModeSelectPage.Reset();
    if (IsEmptyPage()) {
        InsertPage(0, &m_ModeSelectPage);
    }
    Start();
}

// 次のページ取得
CmnSlidePage* MainMenuSlide::GetNextPage()
{
    return nullptr;
}
