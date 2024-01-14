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
    : m_IsSelected(TB_FALSE)
    , m_Index(0)
    , m_PageIndex(0)
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
}

// 定期処理
CmnSlidePage::Result MainMenuModeSelectPage::OnUpdate(TbFloat32 deltasec)
{
    Result result = CmnSlidePage::RESULT_PENDING;
    if (CrInput::IsOnRepeat(CrInput::BUTTON_RIGHT, 0)) {
        if (MODE_MAX_IN_PAGE - 1 <= m_Index) {
            if ( m_PageIndex < MODE_PAGE_MAX-1 ) {
                result = CmnSlidePage::RESULT_NEXT;
            }
        }else {
            ++m_Index;
        }
    }
    if (CrInput::IsOnRepeat(CrInput::BUTTON_LEFT, 0)) {   
        if (m_Index <= 0) {
            if (0 < m_PageIndex) {
                result = CmnSlidePage::RESULT_PREV;
            }
        }else {
            --m_Index;
        }
    }
    if (CrInput::IsPress(CrInput::BUTTON_DECIDE, 0)) {
        m_IsSelected = TB_TRUE;
        result = CmnSlidePage::RESULT_NEXT;
    }
    else if (CrInput::IsPress(CrInput::BUTTON_CANCEL, 0)) {
        result = CmnSlidePage::RESULT_PARENT;
    }
    m_CursorSprite.Update(deltasec);
    return result;
}

// アクティブ状態が変化した
void MainMenuModeSelectPage::OnChangeActive(TbBool isActive)
{
    m_IsSelected = TB_FALSE;
}

static TbSint32 TEXT_Y = 332;
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

    static TbSint32 TEXT_INTERVAL = 260;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f) + pos.GetX() - TEXT_INTERVAL;
    for (TbUint32 i = 0; i < MODE_MAX_IN_PAGE; ++i)
    {
        TbUint32 mode = m_PageIndex*MODE_MAX_IN_PAGE+i;
        TbColor color = CmnUtil::GetMainColor();
        if (m_Index != i) {
            color.SetAlphaF32(0.4f);
        }
        font->SetColor(color);
        font->Printf(x + i*2*TEXT_INTERVAL, TEXT_Y, MODE_NAME[mode]);
    }
    font->PopSetting();

    if( IsCurrent() )
    {
        static TB_NDEBUG_CONST TbFloat32 CURSOR_OFFSET_Y = 0.0f;
        m_CursorSprite.Calculate();
        TbMatrix mtx;
        mtx.SetTranslation(x + m_Index*2*TEXT_INTERVAL, TEXT_Y+ CURSOR_OFFSET_Y, 0.0f);
        m_CursorSprite.Draw(mtx);
    }
}

//////////////////////////////////////////
// MainMenuPlayerSelectPage
/////////////////////////////////////////

// コンストラクタ
MainMenuPlayerSelectPage::MainMenuPlayerSelectPage()
    : m_PlayerCount(Root::GetInstance().GetPlayerCount())
    , m_ArrowSelect(0)
{
    TbRectF32 rect(-64.0f , -32.0f,64.0f,64.0f);
    TbRectF32 uvRect( CMN_TEX_UV_RECT(0.0f,640.0f,48.0f, 48.0f) );
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowSprite); ++i) {
        TbRectF32 uv = uvRect;
        uv.x += CMN_TEX_POS(48.0f)*i;
        m_ArrowSprite[i].Initialize(rect, uv);
        m_ArrowSprite[i].SetColor(TbColor::White());
    }
    SetDepth(1);
}

// 定期処理
CmnSlidePage::Result MainMenuPlayerSelectPage::OnUpdate(TbFloat32 deltasec)
{
    if (CrInput::IsOnRepeat(CrInput::BUTTON_LEFT, 0)) {
        if (CMN_PLAYER_MIN < m_PlayerCount) {
            --m_PlayerCount;
        }else {
            m_PlayerCount = CMN_PLAYER_COUNT;
        }
    }
    else if (CrInput::IsOnRepeat(CrInput::BUTTON_RIGHT, 0)) {
        if (m_PlayerCount <CMN_PLAYER_COUNT) {
            ++m_PlayerCount;
        }else {
            m_PlayerCount = CMN_PLAYER_MIN;
        }
    }
    m_ArrowSelect = 0;
    if (CrInput::IsOn(CrInput::BUTTON_LEFT, 0) ) {
        m_ArrowSelect = -1;
    }else if (CrInput::IsOn(CrInput::BUTTON_RIGHT, 0) ){
        m_ArrowSelect = 1;
    }
    Result result = CmnSlidePage::RESULT_PENDING;
    if (CrInput::IsPress(CrInput::BUTTON_DECIDE, 0)) {
        result = CmnSlidePage::RESULT_NEXT;
    }
    else if (CrInput::IsPress(CrInput::BUTTON_CANCEL, 0)) {
        result = CmnSlidePage::RESULT_PREV;
    }
    return result;
}

// 描画処理
void MainMenuPlayerSelectPage::OnDraw(const TbVector2& pos)
{
    static TB_NDEBUG_CONST TbFloat32 ARROW_OFFSET_X = 280.0f;
    static TB_NDEBUG_CONST TbFloat32 ARROW_OFFSET_Y = 0.0f;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);
    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f) + pos.GetX();
    TbStaticString128 str = TbStaticString128::Format("%dPLAYERS",m_PlayerCount);
    font->Printf(x , TEXT_Y, str.GetBuffer());
    font->PopSetting();

    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowSprite); ++i) {
        m_ArrowSprite[i].Calculate();
    }

    TbMatrix mtx;
    // 左矢印
    mtx.SetTranslation(x- ARROW_OFFSET_X, TEXT_Y + ARROW_OFFSET_Y,0.0f);
    TbBool selected = m_ArrowSelect < 0;
    m_ArrowSprite[selected].Draw(mtx);

    // 右矢印
    mtx.SetTranslation(x + ARROW_OFFSET_X, TEXT_Y + ARROW_OFFSET_Y, 0.0f);
    mtx.Scale(-1.0f,1.0f,1.0f);
    selected = 0 < m_ArrowSelect;
    m_ArrowSprite[selected].Draw(mtx);
}

//////////////////////////////////////////
// MainMenuOptionPage
/////////////////////////////////////////

// コンストラクタ
MainMenuOptionPage::MainMenuOptionPage()
{
    SetDepth(1);
}

// アクティブ状態が変化した
void MainMenuOptionPage::OnChangeActive(TbBool isActive)
{
    if (isActive)
    {
        m_OptionWindow.SetFocus(TB_TRUE);
    }
}

// ページ登録された
void MainMenuOptionPage::OnChangeInsert(TbBool isInserted)
{
    if (isInserted)
    {
        m_OptionWindow.SetPlayerBit(1);
        m_OptionWindow.Open();
        m_OptionWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH,0.0f));
        m_OptionWindow.SetFocus(TB_FALSE);
    }
    else
    {
        m_OptionWindow.Close();
    }
}

// 定期処理
CmnSlidePage::Result MainMenuOptionPage::OnUpdate(TbFloat32 deltasec)
{
    CmnSlidePage::Result result = CmnSlidePage::RESULT_PENDING;
    if (IsActive())
    {
        if (!m_OptionWindow.IsFocus() || CrInput::IsPress(CrInput::BUTTON_CANCEL, 0))
        {
            result = CmnSlidePage::RESULT_PARENT;
        }
    }
    return result;
}

// 描画処理
void MainMenuOptionPage::OnDraw(const TbVector2& pos)
{
    TbVector2 winPos = TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f);
    winPos += pos;
    m_OptionWindow.SetPos(winPos);
}

//////////////////////////////////////////
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
MainMenuSlide::MainMenuSlide()
    : Super()
    , m_CurrentModePageIndex(0)
{
    Initialize(PAGE_MAX);
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ModeSelectPage); ++i) {
        m_ModeSelectPage[i].SetPageIndex(i);
    }
    m_ModeSelectPage[1].SetEnableDrawArrowLeft(TB_TRUE);
    m_ModeSelectPage[0].SetEnableDrawArrowRight(TB_TRUE);
    InsertPage(0,&m_ModeSelectPage[0]);
}

// デストラクタ
MainMenuSlide::~MainMenuSlide()
{
}

// 次のページ取得
CmnSlidePage* MainMenuSlide::GetNextPage()
{
    const CmnSlidePage* page = GetCurrentPage();
    if (const MainMenuModeSelectPage* modePage = TbDynamicCast<const MainMenuModeSelectPage*>(page))
    {
        if (modePage->IsSelected())
        {
            m_CurrentModePageIndex = modePage->GetPageIndex();
            if( MainMenuModeSelectPage::MODE_OPTION == modePage->GetMode())
            {
                return &m_OptionPage;
            }
            else
            {
                return &m_PlayerSelectPage;
            }
        }
        else
        {
            TbSint32 page = (modePage->GetPageIndex()+1)% MODE_PAGE_MAX;
            return &m_ModeSelectPage[page];
        }
    }
    return nullptr;
}
