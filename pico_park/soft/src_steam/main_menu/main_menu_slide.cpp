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
    : m_Index(0)
{
    TbDimensionF32 size(272.0f, 96.0f);
    TbRectF32 uvRect(CMN_TEX_UV_RECT(0.0f, 704.0f, size.width, size.height));

    TbFloat32 SCALE = 1.4f;
    size.width  *= SCALE;
    size.height *= SCALE;
    TbRectF32 rect(-size.width/2.0f, -size.height/2.0f, size.width, size.height);
    m_CursorSprite.Initialize(rect, uvRect);
    m_CursorSprite.SetColor(TbColor::White());
}

// 定期処理
void MainMenuModeSelectPage::OnUpdate(TbFloat32 deltasec)
{
    if (SysInput::IsOnRepeat(SysInput::BUTTON_RIGHT, 0)) {
        TbIncrementLoop<TbSint32>(m_Index,MODE_MAX);
    }
    if (SysInput::IsOnRepeat(SysInput::BUTTON_LEFT, 0)) {        
        TbDecrementLoop<TbSint32>(m_Index, MODE_MAX);
    }
    m_CursorSprite.Update(deltasec);
}

static TbSint32 TEXT_Y = 332;
static TbSint32 FONT_SIZE = 80;

// 描画処理
void MainMenuModeSelectPage::Draw(const TbVector2& pos)
{
    static TbSint32 TEXT_INTERVAL = 300;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f) + pos.GetX();
    font->Printf(x - TEXT_INTERVAL, TEXT_Y, "CO*OP");

    TbColor color = CmnUtil::GetMainColor();
    color.SetAlphaF32(0.4f);
    font->SetColor(color);
    font->Printf(x + TEXT_INTERVAL, TEXT_Y, "BATTLE");
    font->PopSetting();

    static TB_NDEBUG_CONST TbFloat32 CURSOR_OFFSET_Y = 0.0f;
    m_CursorSprite.Calculate();
    TbMatrix mtx;
    mtx.SetTranslation(x - TEXT_INTERVAL, TEXT_Y+ CURSOR_OFFSET_Y, 0.0f);
    m_CursorSprite.Draw(mtx);
}

//////////////////////////////////////////
// MainMenuPlayerSelectPage
/////////////////////////////////////////

// コンストラクタ
MainMenuPlayerSelectPage::MainMenuPlayerSelectPage()
    : m_PlayerCount(CMN_PLAYER_MIN)
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
}

// 定期処理
void MainMenuPlayerSelectPage::OnUpdate(TbFloat32 deltasec)
{
    if (SysInput::IsOnRepeat(SysInput::BUTTON_LEFT, 0)) {
        if (CMN_PLAYER_MIN < m_PlayerCount) {
            --m_PlayerCount;
        }
    }
    else if (SysInput::IsOnRepeat(SysInput::BUTTON_RIGHT, 0)) {
        if (m_PlayerCount <CMN_PLAYER_COUNT) {
            ++m_PlayerCount;
        }
    }
    m_ArrowSelect = 0;
    if (SysInput::IsOn(SysInput::BUTTON_LEFT, 0) ) {
        m_ArrowSelect = -1;
    }else if (SysInput::IsOn(SysInput::BUTTON_RIGHT, 0) ){
        m_ArrowSelect = 1;
    }
}

// 描画処理
void MainMenuPlayerSelectPage::Draw(const TbVector2& pos)
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
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
MainMenuSlide::MainMenuSlide()
    : Super()
{
    Initialize(PAGE_MAX);
    InsertPage(PAGE_MODE_SELECT,&m_ModeSelectPage);
    InsertPage(PAGE_PLAYER_SELECT, &m_PlayerSelectPage);
}

// デストラクタ
MainMenuSlide::~MainMenuSlide()
{
}
