/*!
 * メインメニュー用ページ
 * @author teco
 */

#include "fwd.h"
#include "world_select_slide.h"
#include "common/cmn_global_data.h"
#include "common/cmn_util.h"
#include "root.h"
#include "world_manager.h"
#include <base/algorithm/tb_algorithm_misc.h>

static const TbFloat32 CROWN_TEX_WIDTH = 33.0f;
static const TbFloat32 CROWN_TEX_HEIGHT = 33.0f;
static const TbRectF32Pod CROWN_UV_RECT = {CMN_TEX_UV_RECT(928.0f, 0.0f, CROWN_TEX_WIDTH, CROWN_TEX_HEIGHT)};

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
static TB_NDEBUG_CONST TbFloat32 CROWN_SPRITE_SCALE = 1.5f;

// コンストラクタ
WorldSelectPage::WorldSelectPage()
    : m_HasPrev(TB_FALSE)
    , m_HasNext(TB_FALSE)
    , m_IsSelected(TB_FALSE)
{
    // カーソル
    {
        TbDimensionF32 size(128.0f, 128.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(288.0f, 704.0f, size.width, size.height));
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        m_CursorSprite.Initialize(rect, uvRect);
        m_CursorSprite.SetColor(TbColor::White());
    }
}

// デストラクタ
WorldSelectPage::~WorldSelectPage()
{
}

// 初期化
void WorldSelectPage::Initialize( TbSint32 begin , TbBool hasPrev , TbBool hasNext )
{
    m_HasPrev = hasPrev;
    m_HasNext = hasNext;

    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    TbSint32 worldCount = WorldManager::GetInstance().GetWorldCount();
    m_WorldRange.begin = begin;
    m_WorldRange.end = TbMin<TbSint32>(begin+WORLD_MAX_IN_GROUP,worldCount);

    // ワールド
    {
//        TbBool isClear = 0 < SaveFile::GetInstance().GetClearCount(worldInfo->stage[i].id,playerCount);

        TbDimensionF32 size(64.0f, 64.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512.0f, 640.0f, size.width, size.height));
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        TbRectF32 crownRect(-CROWN_TEX_WIDTH/2.0f, -CROWN_TEX_HEIGHT / 2.0f, CROWN_TEX_WIDTH, CROWN_TEX_HEIGHT);
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
        {
            const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(i+m_WorldRange.begin);
            TbBool isEnable = TB_FALSE;
            if ( (info && info->isEnable) || begin != 0) {
                isEnable = TB_TRUE;
            }
            TbRectF32 uv = uvRect;
            if (isEnable) {
                uv.x += info->uvOffset.GetX();
                uv.y += info->uvOffset.GetY();
            }
            m_Group.sprite[i].Initialize(rect, uv);
            m_Group.sprite[i].SetColor(TbColor::White());
            if (!isEnable) {
                m_Group.sprite[i].SetAlpha(0.4f);
            }
            if (!info) {
                m_Group.sprite[i].SetVisible(TB_FALSE);
            }
            m_Group.crownSprie[i].Initialize(crownRect, CROWN_UV_RECT);
            UpdateCrown();
        }

    }
}

// ワールド全解放モード
void WorldSelectPage::SetOpenAllWorldMode()
{
    SetPhase(PHASE_OPEN_WORLD_CLEAR);
    SetInitPhase(PHASE_OPEN_WORLD_CLEAR);
    m_CursorSprite.SetVisible(TB_FALSE);
}

// 王冠更新
void WorldSelectPage::UpdateCrown()
{
    // クラウン更新
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
    {
        const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(i + m_WorldRange.begin);
        if (info) {
            m_Group.crownSprie[i].SetVisible(info->isWorldClear[playerCount - 1]);
        }else {
            m_Group.crownSprie[i].SetVisible(TB_FALSE);
        }
    }
}

// 定期処理
CmnSlidePage::Result WorldSelectPage::OnUpdate(TbFloat32 deltasec)
{
    m_CursorSprite.Update(deltasec);
    Result result = CmnSlidePage::RESULT_PENDING;
    switch (GetPhase())
    {
    case PHASE_DEFAULT:
        if (m_CursorSprite.IsVisible())
        {
            AddPhase();
        }
        else
        {
            SetPhase(PHASE_OPEN_WORLD_CLEAR);
        }
        break;
    case PHASE_DEFAULT+1:
        {
            if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_LEFT, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_FocusPoint.x == 0 && m_HasPrev) {
                    result = CmnSlidePage::RESULT_PREV;
                }
                else {
                    moveCursor(-1, 0);
                }
            }
            else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_RIGHT, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_FocusPoint.x == WORLD_WIDTH - 1 && m_HasNext) {
                    result = CmnSlidePage::RESULT_NEXT;
                }
                else {
                    moveCursor(1, 0);
                }
            }
            else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_UP, CR_INPUT_PLAYER_ALL_BIT)) {
                moveCursor(0, -1);
            }
            else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_DOWN, CR_INPUT_PLAYER_ALL_BIT)) {
                moveCursor(0, 1);
            }
            else if (CrInput::IsAnyonePress(CrInput::BUTTON_DECIDE, CR_INPUT_PLAYER_ALL_BIT)) {
                result = CmnSlidePage::RESULT_NEXT;
                m_IsSelected = TB_TRUE;
            }
            else if (CrInput::IsAnyonePress(CrInput::BUTTON_CANCEL, CR_INPUT_PLAYER_ALL_BIT)) {
                result = CmnSlidePage::RESULT_RETURN;
            }
        }
        break;
    case PHASE_OPEN_WORLD_CLEAR:
        {
            if (GetPhaseSec() < 1.0f) {
                break;
            }
            TbPlaySound("powerdown");
            AddPhase();
        }
        break;
    case PHASE_OPEN_WORLD_CLEAR+1:
        {
            TbBool changed = TB_FALSE;
            for (TbSizeT i = 1; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
            {
                const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(i + m_WorldRange.begin);
                TbFloat32 scale = m_Group.sprite[i].GetScale().GetX();
                scale = TbMax(0.0f, scale-0.02f);
                m_Group.sprite[i].SetScale(scale);
                if (m_Group.sprite[i].GetScale().GetX() < 0.01f)
                {
                    changed = TB_TRUE;
                    break;
                }
            }
            if (changed)
            {
                for (TbSizeT i = 1; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
                {
                    const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(i + m_WorldRange.begin);
                    TbRectF32 uv = m_Group.sprite[i].GetUV(0);
                    if (info) {
                        uv.x += info->uvOffset.GetX();
                        uv.y += info->uvOffset.GetY();
                    }
                    m_Group.sprite[i].SetUV(uv);
                    m_Group.sprite[i].SetAlpha(1.0f);
                }
                AddPhase();
            }
        }
        break;
    case PHASE_OPEN_WORLD_CLEAR + 2:
        {
            if (GetPhaseSec() < 0.5f) {
                break;
            }
            TbPlaySound("powerup");
            AddPhase();
        }
        break;
    case PHASE_OPEN_WORLD_CLEAR+3:
        {
            TbBool finish = TB_FALSE;
            for (TbSizeT i = 1; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
            {
                const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(i + m_WorldRange.begin);
                TbFloat32 scale = m_Group.sprite[i].GetScale().GetX();
                scale = scale + 0.02f;
                if (1.0f <= scale) {
                    finish = TB_TRUE;
                }
                m_Group.sprite[i].SetScale(scale);
            }
            if (finish)
            {
                TbPlaySound("win");
                AddPhase();
                break;
            }
        }
        break;
    case PHASE_OPEN_WORLD_CLEAR+4:
        {
            if (GetPhaseSec() < 1.0f) {
                break;
            }
            m_CursorSprite.SetVisible(TB_TRUE);
            SetEnableDrawArrowRight(TB_TRUE);
            WorldManager::GetInstance().UpdateOpenAllWorld();
            SetPhase(PHASE_DEFAULT);
            SetInitPhase(PHASE_DEFAULT);
        }
        break;
    }

    return result;
}

// 現在のページになったか
void WorldSelectPage::OnChangeActive(TbBool isActive) 
{
    m_IsSelected = TB_FALSE;
}

// 描画処理
void WorldSelectPage::OnDraw(const TbVector2& offset)
{
    static TbUint32 WORLD_WIDTH = 3;
    static TbFloat32 SPRITE_INTERVAL_X = 350.0f;
    static TbFloat32 SPRITE_INTERVAL_Y = 260.0f;
    static TbFloat32 BEGIN_Y = 300.0f;
    static TbFloat32 CROWN_OFS_X = -80.0f;
    static TbFloat32 CROWN_OFS_Y = -80.0f;

    TbFloat32 beginX = (CMN_WINDOW_WIDTH / 2.0f) - SPRITE_INTERVAL_X;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
    {
        TbPointS32 point = getPoint(i);
        TbVector2 pos;
        pos.SetX(beginX + (point.x * SPRITE_INTERVAL_X));
        pos.SetY(BEGIN_Y + (point.y * SPRITE_INTERVAL_Y));
        pos += offset;

        TbMatrix mtx;
        mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
        mtx.Scale(SPRITE_SCALE, SPRITE_SCALE, 1.0f);
        // サムネイル
        m_Group.sprite[i].Calculate();
        m_Group.sprite[i].Draw(mtx);
        // 王冠
        TbMatrix crownMtx;
        crownMtx.Translate(pos.GetX()+CROWN_OFS_X, pos.GetY()+ CROWN_OFS_Y, 0.0f);
        crownMtx.Scale(CROWN_SPRITE_SCALE, CROWN_SPRITE_SCALE, 1.0f);
        m_Group.crownSprie[i].Calculate();
        m_Group.crownSprie[i].Draw(crownMtx);
        // カーソル
        if (m_FocusPoint.x == point.x && m_FocusPoint.y == point.y)
        {
            if(IsCurrent()){
                drawCursor(m_CursorSprite,pos);
            }
        }
    }
}

// カーソル移動
TbBool WorldSelectPage::moveCursor(TbSint32 movX, TbSint32 movY)
{
    TbPointS32 point = m_FocusPoint;
    point.x = TbClamp<TbSint32>(m_FocusPoint.x+movX,0,WORLD_WIDTH-1);
    point.y = TbClamp<TbSint32>(m_FocusPoint.y+movY,0,WORLD_HEIGHT-1);
    if (isEnableWorld(point.x, point.y)) {
        m_FocusPoint = point;
        return TB_TRUE;
    }
    return TB_FALSE;
}

// 位置からワールドインデックス取得
TbSint32 WorldSelectPage::getWorldIndex(TbSint32 x, TbSint32 y) const
{
    TbSint32 worldIndex = y*WORLD_WIDTH +x;
    return worldIndex;
}

// ワールドインデックスから座標取得
TbPointS32 WorldSelectPage::getPoint(TbSint32 worldIndex) const
{
    TbPointS32 result;
    result.x = worldIndex%WORLD_WIDTH;
    result.y = worldIndex/WORLD_WIDTH;
    TB_ASSERT(result.x < WORLD_WIDTH);
    return result;
}

// ワールド利用可能か
TbBool   WorldSelectPage::isEnableWorld(TbSint32 x, TbSint32 y) const
{
    TbSint32 world = getWorldIndex(x,y) + m_WorldRange.begin;
    const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(world);
    if (info && info->isEnable) {
        return TB_TRUE;
    }
    return TB_FALSE;
}

//////////////////////////////////////////
// WorldStageSelectPage
/////////////////////////////////////////

static const char WORLD_CLEAR_STR[] = "WORLD CLEAR";
static TB_NDEBUG_CONST TbFloat32 WORLD_CLEAR_FONT_SIZE = 80.0f;

// コンストラクタ
WorldStageSelectPage::WorldStageSelectPage()
    : m_SelectedStage(0)
    , m_World(0)
    , m_LatestStage(0)
    , m_WorldClearPosX(0.0f)
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

// デストラクタ
WorldStageSelectPage::~WorldStageSelectPage()
{
    TB_SAFE_DELETE_ARRAY(m_Stage);
}

// ワールド設定
void WorldStageSelectPage::SetWorld(TbSint32 world)
{
    if( !m_Stage || m_World != world )
    {
        m_SelectedStage = 0;
        m_World = world;
        const WorldInfo* worldInfo = WorldManager::GetInstance().GetWorldInfo(world);
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
        m_LatestStage = worldInfo->latestStage;
        TbSint32 stageCount = getStageCount();
        TB_SAFE_DELETE_ARRAY(m_Stage);
        m_Stage = new Stage[stageCount];

        TbFloat32 s = 256.0f/ TbMax<TbSint32>(4,stageCount);
        TbDimensionF32 size(s,s);
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(112.0f, 800.0f, 64.0f, 64.0f));

        TbRectF32 crownRect(-CROWN_TEX_WIDTH, -CROWN_TEX_HEIGHT, CROWN_TEX_WIDTH*2.0f, CROWN_TEX_HEIGHT*2.0f);
        TbRectF32 lineRect(0.0f,-4.0f,1.0f,8.0f);
        for (TbSint32 i = 0; i < stageCount; ++i)
        {
            TbBool isClear = 0 < SaveFile::GetInstance().GetClearCount(worldInfo->stage[i].id, playerCount);
            m_Stage[i].sprite.Initialize(rect,uvRect);
            m_Stage[i].crownSprie.Initialize(crownRect, CROWN_UV_RECT);
            m_Stage[i].crownSprie.SetVisible(isClear);
            m_Stage[i].line.Initialize(lineRect);
            m_Stage[i].line.SetColor(CmnUtil::GetMainColor());
            if ( i <= m_LatestStage) {
                m_Stage[i].ratio = 1.0f;
            }
        }
    }
}

// ステージ取得
TbUint32 WorldStageSelectPage::GetSelectedStageId() const
{
    const WorldInfo* worldInfo = WorldManager::GetInstance().GetWorldInfo(m_World);
    return worldInfo->stage[m_SelectedStage].id;
}

// 定期処理
CmnSlidePage::Result WorldStageSelectPage::OnUpdate(TbFloat32 deltasec)
{
    Result result = CmnSlidePage::RESULT_PENDING;
    CmnGlobalData& globalData = Root::GetInstance().GetGlobalData();
    TbUint32 lastStage = getStageCount() - 1;
     switch (GetPhase())
    {
    case PHASE_CHECK_CLEAR:
        {
            WorldManager::UpdateInfoResult updated = WorldManager::GetInstance().UpdateInfo(m_World);
            if ( updated.isUpdatedLatestStage )
            {
                // 次ステージがある
                SetPhase(PHASE_NEW_STAGE);
            }
            else if (updated.isUpdatedWorldClear)
            {
                SetPhase(PHASE_WORLD_CLEAR);
            }
            else 
            {
                SetPhase(PHASE_SELECT_STAGE);
            }
        }
        break;
    case PHASE_NEW_STAGE:
        {
            static TbFloat32 WAIT_SEC = 0.75f;
            if ( GetPhaseSec() < WAIT_SEC) {
                break;
            }
            if (m_LatestStage < lastStage ) {
                m_Stage[m_LatestStage +1].ratio += 0.02f;
                if (1.0f <= m_Stage[m_LatestStage + 1].ratio) {
                    m_Stage[m_LatestStage + 1].ratio = 1.0f;
                    m_SelectedStage = m_LatestStage + 1;
                    TbPlaySound("win");
                    AddPhase();
                }
            }
            else {
                SetPhase(PHASE_SELECT_STAGE);
            }
        }
        break;

    case PHASE_NEW_STAGE + 1:
        {
            static TbFloat32 WAIT_SEC = 1.0f;
            if (GetPhaseSec() < WAIT_SEC) {
                break;
            }
            ++m_LatestStage;
            SetPhase(PHASE_SELECT_STAGE);
        }
        break;

    case PHASE_WORLD_CLEAR:
        {
            TbPlaySound("clear");
            TbFontBuffer2d* font = Root::GetInstance().GetFont();
            font->PushSetting();
            font->SetFontSize(WORLD_CLEAR_FONT_SIZE);
            TbRectF32 region;
            font->GetRegion(region,0,0,WORLD_CLEAR_STR);
            font->PopSetting();
            m_WorldClearPosX = -region.width;
            AddPhase();
        }
        break;

    case PHASE_WORLD_CLEAR+1:
        {
            static TB_NDEBUG_CONST TbFloat32 SPEED_X = 11.0f;
            if (m_WorldClearPosX < CMN_WINDOW_WIDTH + 200.0f ) {
                m_WorldClearPosX += SPEED_X;
                break;
            }
            if ( m_World == 0 && !WorldManager::GetInstance().IsOpenedAllWorld() )
            {
                // 最初のワールドクリアしたら
                // 全ワールドをオープン
                SetPhase(PHASE_OPEN_ALL_WORLD);
                result = CmnSlidePage::RESULT_RETURN;
            }
            else
            {
                SetPhase(PHASE_SELECT_STAGE);
            }
        }
        break;

    case PHASE_OPEN_ALL_WORLD:
        break;

    case PHASE_SELECT_STAGE:
        {
            m_CursorSprite.SetVisible(TB_TRUE);
            AddPhase();
        }
        // break through
    case PHASE_SELECT_STAGE+1:
        {
            if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_RIGHT, CR_INPUT_PLAYER_ALL_BIT)) {
                if (m_SelectedStage < m_LatestStage) {
                    ++m_SelectedStage;
                }
            }
            if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_LEFT, CR_INPUT_PLAYER_ALL_BIT)) {
                if (0 < m_SelectedStage) {
                    --m_SelectedStage;
                }
            }
            if (CrInput::IsAnyonePress(CrInput::BUTTON_DECIDE, CR_INPUT_PLAYER_ALL_BIT)) {
                result = CmnSlidePage::RESULT_NEXT;
            }
            else if (CrInput::IsAnyonePress(CrInput::BUTTON_CANCEL, CR_INPUT_PLAYER_ALL_BIT)) {
                result = CmnSlidePage::RESULT_PREV;
            }
        }
        break;
    }
    m_CursorSprite.Update(deltasec);
    return result;
}

// 描画処理
void WorldStageSelectPage::OnDraw(const TbVector2& offset)
{
    static TB_NDEBUG_CONST TbFloat32 STAGE_Y = 420.0f;

    TbSint32 stageCount = getStageCount();
    TbFloat32 stageBeginX = 0.0f;
    const TbFloat32 STAGE_INTERVAL_X = 1120.0f/stageCount;
    TbFloat32 scale = (4.0f / static_cast<TbFloat32>(TbMax<TbSint32>(4, stageCount)));
    static TB_NDEBUG_CONST TbFloat32 LINE_OFFSET_X = -(STAGE_INTERVAL_X-(80.0f*scale));
    static TB_NDEBUG_CONST TbFloat32 LINE_OFFSET_WIDTH = (-LINE_OFFSET_X- (80.0f*scale));
    static TB_NDEBUG_CONST TbFloat32 CROWN_OFFSET_Y = -175.0f;
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
        // スプライト
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
        if( isEnableDrawLine )
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

        // 王冠
        {
            TbMatrix crownMtx;
            crownMtx.Translate(pos.GetX(), pos.GetY()+ CROWN_OFFSET_Y, 0.0f);
            m_Stage[i].crownSprie.Calculate();
            m_Stage[i].crownSprie.Draw(crownMtx);
        }

        // フォーカスカーソル
        if ( m_SelectedStage == i )
        {
            drawCursor(m_CursorSprite, pos , scale );
        }
        pos.Add(STAGE_INTERVAL_X, 0.0f);
    }
    font->PopSetting();

    // ワールドクリアー
    if (PHASE_WORLD_CLEAR+1 <= GetPhase() && GetPhase() < PHASE_SELECT_STAGE)
    {
        static TB_NDEBUG_CONST TbFloat32 POS_Y = CMN_WINDOW_HEIGHT * 0.85f;
        font->PushSetting();
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_LEFT);
        font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        font->SetFontSize(WORLD_CLEAR_FONT_SIZE);
        font->SetEnableBorder(TB_TRUE);
        font->Printf(m_WorldClearPosX, POS_Y, WORLD_CLEAR_STR);
        font->PopSetting();
    }
}

// ステージ数取得
TbSint32 WorldStageSelectPage::getStageCount() const
{
    const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(m_World);
    if (TB_VERIFY(info))
    {
        return info->stageCount;
    }
    return 1;
}

//////////////////////////////////////////
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
WorldSelectSlide::WorldSelectSlide()
    : Super()
    , m_World(0)
    , m_IsReadyBgm(TB_TRUE)
{
    TbUint32 worldCount = WorldManager::GetInstance().GetWorldCount();
    TbBool   isOpenedAllWorld = WorldManager::GetInstance().IsOpenedAllWorld();
    TbUint32 pageCount = ((worldCount-1)/WorldSelectPage::WORLD_MAX_IN_GROUP)+1;
    // 各ページの初期化
    m_WorldSelectPage.PushBackDefault(pageCount);
    for (TbSint32 i = 0; i < m_WorldSelectPage.GetCount(); ++i) {
        if (2 <= m_WorldSelectPage.GetCount()) {
            if (i != 0) {
                m_WorldSelectPage[i].SetEnableDrawArrowLeft(TB_TRUE);
            }
            if (i < m_WorldSelectPage.GetCount() - 1) {
                if (isOpenedAllWorld)
                {
                    m_WorldSelectPage[i].SetEnableDrawArrowRight(TB_TRUE);
                }
            }
        }
        m_WorldSelectPage[i].Initialize(i*WorldSelectPage::WORLD_MAX_IN_GROUP,i!=0, i < m_WorldSelectPage.GetCount()-1);
    }
    // 全体初期化
    Initialize(pageCount+1);
    InsertPage(0,&m_WorldSelectPage[0]);
}

// デストラクタ
WorldSelectSlide::~WorldSelectSlide()
{
}

// 全ワールド解放モード設定
void WorldSelectSlide::SetOpenAllWorldMode()
{
    m_WorldSelectPage[0].SetOpenAllWorldMode();
    m_IsReadyBgm = TB_FALSE;
    SetPage(0);
}

// 復帰
void WorldSelectSlide::Resume( TbSint32 stageId , TbBool isNextStage)
{
    TbSint32 stageIndex = 0;
    TbSint32 world = WorldManager::GetInstance().GetWorldFromStageId(stageId, &stageIndex);
    TbUint32 pageIndex = (world/WorldSelectPage::WORLD_MAX_IN_GROUP);
    for (TbUint32 i = 1; i <= pageIndex; ++i) {
        InsertPage(i,&m_WorldSelectPage[i]);
    }
    TbBool isStageSelect = TB_TRUE;
    if (isNextStage) {
        isStageSelect = (stageIndex != 0);
    }
    if( !isNextStage){
        m_IsReadyBgm = TB_FALSE;
        InsertPage(pageIndex+1,&m_WorldStageSelectPage);
        SetPage(pageIndex + 1);
        SetWorld(world);
        SetStageIndex(stageIndex);
    }else {
        SetPage(pageIndex);
        SetWorld(world);
    }
}

// 定期処理
void WorldSelectSlide::OnUpdate()
{
    const CmnSlidePage* currentPage = GetCurrentPage();
    if (!m_IsReadyBgm) {
        if (currentPage == &m_WorldStageSelectPage)
        {
            m_IsReadyBgm = m_WorldStageSelectPage.IsReadySelectStage();
        }
        else
        {
            m_IsReadyBgm = m_WorldSelectPage[0].IsReadySelect();
        }
    }
    TbForIterator(it, m_WorldSelectPage) {
        if (&(*it) == currentPage) {
            m_World = it->GetWorld();
        }
        it->UpdateCrown();
    }
}

// 描画
void WorldSelectSlide::OnDraw()
{
    static TB_NDEBUG_CONST TbFloat32 TEXT_Y = 90.0f;
    static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 96.0f;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
    const WorldInfo* info = WorldManager::GetInstance().GetWorldInfo(m_World);
    if(info ){
        font->Printf(x, TEXT_Y, "%d.%s" , m_World+1 , info->name.GetBuffer());
    }
    font->PopSetting();
}

// ページ変更
void WorldSelectSlide::OnChangingPage(TbBool isNext)
{
    const CmnSlidePage* currentPage = GetCurrentPage();
    TbForIterator(it, m_WorldSelectPage) {
        if (&(*it) == currentPage ) {
            if( it->IsSelected() ){
                TbSint32 world = it->GetWorld();
                m_WorldStageSelectPage.SetWorld(world);
            }else{
                TbPointS32 srcPoint = it->GetFocusPoint();
                WorldSelectPage* target = nullptr;
                TbSint32 x = 0;
                if (isNext)
                {
                    target = TbDynamicCast<WorldSelectPage*>(GetPage(GetCurentPageIndex()+1));
                }
                else
                {
                    target = TbDynamicCast<WorldSelectPage*>(GetPage(GetCurentPageIndex()-1));
                    x = WorldSelectPage::WORLD_WIDTH-1;
                }
                if (target)
                {
                    target->SetWorld(x,srcPoint.y);
                }
            }
        }
    }
}

// 次のページ取得
CmnSlidePage* WorldSelectSlide::GetNextPage()
{
    const CmnSlidePage* currentPage = GetCurrentPage();
    TbForIterator(it, m_WorldSelectPage) {
        if (&(*it) == currentPage) {
            if (it->IsSelected()) {
                return &m_WorldStageSelectPage;
            }else {
                ++it;
                if (it != m_WorldSelectPage.End()) {
                    return &(*it);
                }
            }
        }
    }
    return nullptr;
}
