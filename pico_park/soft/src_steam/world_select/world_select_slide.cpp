/*!
 * メインメニュー用ページ
 * @author teco
 */

#include "fwd.h"
#include "world_select_slide.h"
#include "common/cmn_global_data.h"
#include "root.h"
#include "common/cmn_util.h"

void drawCursor(CmnCursorSprite& sprite, const TbVector2& pos,TbFloat32 scale = 1.0f)
{
    static TbFloat32 SCALE = 2.0f;
    TbMatrix mtx;

    mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
    mtx.Scale(SCALE*scale, SCALE*scale, 1.0f);
    sprite.Calculate();
    sprite.Draw(mtx);
}

//////////////////////////////////////////
// WorldSelectPage
/////////////////////////////////////////

static TB_NDEBUG_CONST TbFloat32 SPRITE_SCALE = 2.6f;

// コンストラクタ
WorldSelectPage::WorldSelectPage()
    : m_World(0)
{
    // カーソル
    {
        TbDimensionF32 size(128.0f, 128.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(288.0f, 704.0f, size.width, size.height));
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        m_CursorSprite.Initialize(rect, uvRect);
        m_CursorSprite.SetColor(TbColor::White());
    }

    // グループ
    {
        TbDimensionF32 size(96.0f, 96.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(592.0f, 928.0f, size.width, size.height));
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
        {
            TbRectF32 uv = uvRect;
            if (i == 0) {
                uv.x -= CMN_TEX_POS(96.0f);
            }
            m_Group.sprite[i].Initialize(rect, uv);
            m_Group.sprite[i].SetColor(TbColor::White());
            if (i != 0) {
                m_Group.sprite[i].SetAlpha(0.4f);
            }
        }

    }
}

// 定期処理
void WorldSelectPage::OnUpdate(TbFloat32 deltasec)
{
    m_CursorSprite.Update(deltasec);

}

// 描画処理
void WorldSelectPage::Draw(const TbVector2& offset)
{
    static TbUint32 WORLD_WIDTH = 3;
    static TbFloat32 SPRITE_INTERVAL_X = 350.0f;
    static TbFloat32 SPRITE_INTERVAL_Y = 260.0f;
    static TbFloat32 BEGIN_Y = 300.0f;

    TbUint32  height = STAGE_MAX_IN_GROUP / WORLD_WIDTH;
    TbFloat32 beginX = (CMN_WINDOW_WIDTH / 2.0f) - SPRITE_INTERVAL_X;
    for (TbUint32 y = 0; y < height; ++y)
    {
        for (TbUint32 x = 0; x < WORLD_WIDTH; ++x)
        {
            TbVector2 pos;
            pos.SetX(beginX + (x * SPRITE_INTERVAL_X));
            pos.SetY(BEGIN_Y + (y * SPRITE_INTERVAL_Y));
            pos += offset;

            TbUint32 index = y*WORLD_WIDTH + x;
            TbMatrix mtx;
            mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
            mtx.Scale(SPRITE_SCALE, SPRITE_SCALE, 1.0f);
            m_Group.sprite[index].Calculate();
            m_Group.sprite[index].Draw(mtx);
            if (m_FocusPoint.x == x && m_FocusPoint.y == y)
            {
                drawCursor(m_CursorSprite,pos);
            }
        }
    }
}

//////////////////////////////////////////
// WorldStageSelectPage
/////////////////////////////////////////

// コンストラクタ
WorldStageSelectPage::WorldStageSelectPage()
    : m_SelectedStage(0)
    , m_World(0)
    , m_Stage(nullptr)
{
    SetWorld(m_World);

    // カーソル
    {
        TbDimensionF32 size(128.0f, 128.0f);
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(288.0f, 704.0f, size.width, size.height));
        m_CursorSprite.Initialize(rect, uvRect);
        m_CursorSprite.SetColor(TbColor::White());
        m_CursorSprite.SetScale(1.1f);
        m_CursorSprite.SetVisible(TB_FALSE);
    }
}

// ワールド設定
void WorldStageSelectPage::SetWorld(TbSint32 world)
{
    if( !m_Stage || m_World != world )
    {
        m_World = world;
        TbSint32 stageCount = getStageCount();
        TB_SAFE_DELETE_ARRAY(m_Stage);
        m_Stage = new Stage[stageCount];

        TbFloat32 s = 256.0f/ stageCount;
        TbDimensionF32 size(s, s);
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(112.0f, 800.0f, 64.0f, 64.0f));

        TbRectF32 lineRect(0.0f,-4.0f,1.0f,8.0f);
        CmnGlobalData& globalData = Root::GetInstance().GetGlobalData();
        for (TbSint32 i = 0; i < stageCount; ++i)
        {
            m_Stage[i].sprite.Initialize(rect,uvRect);
            m_Stage[i].line.Initialize(lineRect);
            m_Stage[i].line.SetColor(CmnUtil::GetMainColor());
            if ( i <= globalData.GetLatestStage(m_World) ) {
                m_Stage[i].ratio = 1.0f;
            }
        }
    }
}

// 定期処理
void WorldStageSelectPage::OnUpdate(TbFloat32 deltasec)
{
    enum
    {
        PHASE_CHECK_CLEAR,
        PHASE_SELECT_STAGE = 10
    };

    CmnGlobalData& globalData = Root::GetInstance().GetGlobalData();
    TbUint32 lastStage = getStageCount() - 1;
    TbUint32 latestStage = TbMin(lastStage,globalData.GetLatestStage(m_World));
    switch (GetPhase())
    {
    case PHASE_CHECK_CLEAR:
        {
            if ( globalData.IsWorldNewStageCleared(m_World)) 
            {
                AddPhase();
            }
            else 
            {
                SetPhase(PHASE_SELECT_STAGE);
            }
        }
        break;
    case PHASE_CHECK_CLEAR+1:
        {
            static TbFloat32 WAIT_SEC = 1.0f;
            if ( GetPhaseSec() < WAIT_SEC) {
                break;
            }
            if ( latestStage < lastStage ) {
                m_Stage[latestStage+1].ratio += 0.01f;
                if (1.0f <= m_Stage[latestStage + 1].ratio) {
                    m_Stage[latestStage + 1].ratio = 1.0f;
                    m_SelectedStage = latestStage + 1;
                    AddPhase();
                }
            }
            else {
                SetPhase(PHASE_SELECT_STAGE);
            }
        }
        break;

    case PHASE_CHECK_CLEAR + 2:
        {
            static TbFloat32 WAIT_SEC = 1.5f;
            if (GetPhaseSec() < WAIT_SEC) {
                break;
            }
            SetPhase(PHASE_SELECT_STAGE);
        }
        break;
    case PHASE_SELECT_STAGE:
        {
            globalData.ClearWorldStageClearFlag(m_World);
            m_CursorSprite.SetVisible(TB_TRUE);
            AddPhase();
        }
        // break through
    case PHASE_SELECT_STAGE+1:
        {
            if (SysInput::IsOnRepeat(SysInput::BUTTON_RIGHT, 0)) {
                if (m_SelectedStage < latestStage) {
                    ++m_SelectedStage;
                }
            }
            if (SysInput::IsOnRepeat(SysInput::BUTTON_LEFT, 0)) {
                if (0 < m_SelectedStage) {
                    --m_SelectedStage;
                }
            }
        }
        break;
    }
    m_CursorSprite.Update(deltasec);
}

// 描画処理
void WorldStageSelectPage::Draw(const TbVector2& offset)
{
    static TB_NDEBUG_CONST TbFloat32 STAGE_Y = 380.0f;

    TbSint32 stageCount = getStageCount();
    TbFloat32 stageBeginX = 0.0f;
    const TbFloat32 STAGE_INTERVAL_X = 1120.0f/stageCount;
    TB_NDEBUG_CONST TbFloat32 LINE_OFFSET_X = -800.0f/ stageCount;
    TB_NDEBUG_CONST TbFloat32 LINE_OFFSET_WIDTH = 480.0f/ stageCount;
    if (stageCount % 2)
    {
        stageBeginX = (CMN_WINDOW_WIDTH/2.0f) - (stageCount/2)*STAGE_INTERVAL_X;
    }
    else
    {
        stageBeginX = (CMN_WINDOW_WIDTH / 2.0f);
        stageBeginX -= ((stageCount/2)-1)*STAGE_INTERVAL_X;
        stageBeginX -= STAGE_INTERVAL_X/2.0f;
    }

    static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 64.0f;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
    TbVector2 pos(stageBeginX, STAGE_Y);
    pos += offset;
    for (TbSint32 i = 0; i < stageCount; ++i)
    {
        TbFloat32 alpha = m_Stage[i].IsActive() ? 1.0f : 0.4f;
        TbMatrix mtx;
        mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
        mtx.Scale(SPRITE_SCALE, SPRITE_SCALE, 1.0f);
        m_Stage[i].sprite.SetAlpha(alpha);
        m_Stage[i].sprite.Calculate();
        m_Stage[i].sprite.Draw(mtx);

        // ライン
        TbBool isEnableDrawLine = TB_FALSE;
        isEnableDrawLine = i != 0 && 0.0f < m_Stage[i].ratio;
        if(isEnableDrawLine )
        {
            TbMatrix lineMtx;
            lineMtx.Translate(pos.GetX()+ LINE_OFFSET_X, pos.GetY(), 0.0f);
            lineMtx.Scale(m_Stage[i].ratio * LINE_OFFSET_WIDTH,1.0f,1.0f);
            m_Stage[i].line.Calculate();
            m_Stage[i].line.Draw(lineMtx);
        }

        // 数字
        TbColor color = CmnUtil::GetMainColor();
        color.SetAlphaF32(alpha);
        font->SetColor(color);
        font->Printf(pos.GetX(), pos.GetY(), "%d", i + 1);

        // フォーカスカーソル
        if ( m_SelectedStage == i )
        {
            drawCursor(m_CursorSprite, pos , 4.0f/static_cast<TbFloat32>(stageCount));
        }
        pos.Add(STAGE_INTERVAL_X, 0.0f);
    }
    font->PopSetting();
}

// ステージ数取得
TbSint32 WorldStageSelectPage::getStageCount() const
{
    return 5;
}

//////////////////////////////////////////
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
WorldSelectSlide::WorldSelectSlide()
    : Super()
{
    Initialize(PAGE_MAX);
    InsertPage(PAGE_WORLD_SELECT,&m_WorldSelectPage);
    InsertPage(PAGE_STAGE_SELECT, &m_WorldStageSelectPage);
}

// デストラクタ
WorldSelectSlide::~WorldSelectSlide()
{
}

// ページ変更
void WorldSelectSlide::OnChangingPage(TbUint32 page)
{
    switch (page)
    {
    case PAGE_WORLD_SELECT:
        break;
    case PAGE_STAGE_SELECT: // ステージ変更
        {
            TbSint32 world = m_WorldSelectPage.GetWorld();
            m_WorldStageSelectPage.SetWorld(world);
        }
        break;
    }
}
