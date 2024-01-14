/*!
 * バトルステージ選択用ページ
 * @author teco
 */

#include "fwd.h"
#include "endless_stage_select_slide.h"
#include "common/cmn_global_data.h"
#include "common/cmn_util.h"
#include "root.h"
#include "world_manager.h"
#include <base/algorithm/tb_algorithm_misc.h>

static void drawCursor(CmnCursorSprite& sprite, const TbVector2& pos,TbFloat32 scale = 1.0f)
{
    static TbFloat32 SCALE = 2.0f;
    TbMatrix mtx;

    mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
    mtx.Scale(SCALE*scale, SCALE*scale, 1.0f);
    sprite.Calculate();
    sprite.Draw(mtx);
}

//////////////////////////////////////////
// EndlessStageSelectPage
/////////////////////////////////////////

static TB_NDEBUG_CONST TbFloat32 SPRITE_SCALE = 2.6f;

// コンストラクタ
EndlessStageSelectPage::EndlessStageSelectPage()
    : m_HasPrev(TB_FALSE)
    , m_HasNext(TB_FALSE)
    , m_StageWidth(1)
    , m_StageHeight(1)
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

// 初期化
void EndlessStageSelectPage::Initialize( TbSint32 begin , TbBool hasPrev , TbBool hasNext )
{
    m_HasPrev = hasPrev;
    m_HasNext = hasNext;

    TbSint32 stageCount = WorldManager::GetInstance().GetEndlessStageCount();
    m_StageRange.begin = begin;
    m_StageRange.end = TbMin<TbSint32>(begin+STAGE_MAX_IN_GROUP,stageCount);
    TbSint32 dispStageCount = m_StageRange.end - m_StageRange.begin;
    m_StageHeight = 1; // TbMin<TbUint32>(dispStageCount,2);
    m_StageWidth  = dispStageCount;

    // ステージ
    {
        TbDimensionF32 size(64.0f, 64.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512.0f, 640.0f, size.width, size.height));
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
        {
            const EndlessStageInfo* info = WorldManager::GetInstance().GetEndlessStageInfo(i+m_StageRange.begin);
            TbRectF32 uv = uvRect;
            if (info ) {
                uv.x += info->uvOffset.GetX();
                uv.y += info->uvOffset.GetY();
            }
            m_Group.sprite[i].Initialize(rect, uv);
            m_Group.sprite[i].SetColor(TbColor::White());
            if (!info) {
                m_Group.sprite[i].SetVisible(TB_FALSE);
            }
        }

    }
}

// 定期処理
CmnSlidePage::Result EndlessStageSelectPage::OnUpdate(TbFloat32 deltasec)
{
    if (GetPhase()) {
        m_IsSelected = TB_FALSE;
        AddPhase();
    }
    m_CursorSprite.Update(deltasec);

    Result result = CmnSlidePage::RESULT_PENDING;
    if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_LEFT, CR_INPUT_PLAYER_ALL_BIT)) {
        if (m_FocusPoint.x == 0 && m_HasPrev) {
            result = CmnSlidePage::RESULT_PREV;
        }else{
            moveCursor(-1, 0);
        }
    }
    else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_RIGHT, CR_INPUT_PLAYER_ALL_BIT)) {
        if (m_FocusPoint.x == m_StageWidth-1 && m_HasNext) {
            result = CmnSlidePage::RESULT_NEXT;
        }else{
            moveCursor(1,0);
        }
    }
    else if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_UP, CR_INPUT_PLAYER_ALL_BIT)) {
        moveCursor(0,-1);
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
    return result;
}

// 現在のページになったか
void EndlessStageSelectPage::OnChangeActive(TbBool isActive) 
{
    m_IsSelected = TB_FALSE;
}

// 描画処理
void EndlessStageSelectPage::OnDraw(const TbVector2& offset)
{
    static TB_NDEBUG_CONST TbFloat32 SPRITE_INTERVAL_X = 300.0f;
    static TB_NDEBUG_CONST TbFloat32 SPRITE_INTERVAL_Y = 260.0f;

    static TB_NDEBUG_CONST TbFloat32 CENTER_X = (CMN_WINDOW_WIDTH / 2.0f);
    TbFloat32 beginX = CENTER_X;
    if ( m_StageWidth % 2 == 0 ) {
        beginX -= (SPRITE_INTERVAL_X / 2);
        beginX -= (SPRITE_INTERVAL_X)*((m_StageWidth / 2) - 1);
    }else {
        beginX = CENTER_X - (SPRITE_INTERVAL_X)*(m_StageWidth/2);
    }
    static TB_NDEBUG_CONST TbFloat32 SPRITE_Y = 360.0f;
    TbFloat32 beginY = SPRITE_Y;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
    {
        TbPointS32 point = getPoint(i);
        TbVector2 pos;
        pos.SetX(beginX + (point.x * SPRITE_INTERVAL_X));
        pos.SetY(beginY + (point.y * SPRITE_INTERVAL_Y));
        pos += offset;

        TbMatrix mtx;
        mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
        mtx.Scale(SPRITE_SCALE, SPRITE_SCALE, 1.0f);
        m_Group.sprite[i].Calculate();
        m_Group.sprite[i].Draw(mtx);
        if (m_FocusPoint.x == point.x && m_FocusPoint.y == point.y)
        {
            if(IsCurrent()){
                drawCursor(m_CursorSprite,pos);
            }
        }
    }
}

// カーソル移動
TbBool EndlessStageSelectPage::moveCursor(TbSint32 movX, TbSint32 movY)
{
    TbPointS32 point = m_FocusPoint;
    point.x = TbClamp<TbSint32>(m_FocusPoint.x+movX,0, m_StageWidth-1);
    point.y = TbClamp<TbSint32>(m_FocusPoint.y+movY,0, m_StageHeight-1);
    m_FocusPoint = point;
    return TB_TRUE;
}

// ステージ順番設定
TbBool EndlessStageSelectPage::SetStageIndex(TbUint32 stageIndex)
{
    if (m_StageRange.begin <= stageIndex && stageIndex < m_StageRange.end)
    {
        TbUint32 index = stageIndex - m_StageRange.begin;
        TbSint32 x = index;
        TbSint32 y = 0;
        if (2 <= m_StageHeight) {
            x = index /m_StageHeight;
            y = index%m_StageHeight;
        }
        moveCursor(x,y);
        return TB_TRUE;
    }
    return TB_FALSE;
}

// 位置からステージインデックス取得
TbSint32 EndlessStageSelectPage::getStageIndex(TbSint32 x, TbSint32 y) const
{
    TbSint32 stageIndex = x*m_StageHeight+y;
    return stageIndex;
}

// ワールドインデックスから座標取得
TbPointS32 EndlessStageSelectPage::getPoint(TbSint32 stageIndex) const
{
    TbPointS32 result;
    if( 1 == m_StageHeight ){
        result.x = stageIndex;
        result.y = 0;
    } else {
        result.x = stageIndex / m_StageHeight;
        result.y = stageIndex%m_StageHeight;
    }
    return result;
}

//////////////////////////////////////////
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
EndlessStageSelectSlide::EndlessStageSelectSlide()
    : Super()
    , m_StageIndex(0)
{
    TbUint32 endlessStageCount = WorldManager::GetInstance().GetEndlessStageCount();
    TbUint32 pageCount = ((endlessStageCount -1)/EndlessStageSelectPage::STAGE_MAX_IN_GROUP)+1;

    // 各ページの初期化
    m_EndlessStageSelectPage.PushBackDefault(pageCount);
    for (TbSint32 i = 0; i < m_EndlessStageSelectPage.GetCount(); ++i) {
        if( 2 <= m_EndlessStageSelectPage.GetCount() ){
            if (i != 0) {
                m_EndlessStageSelectPage[i].SetEnableDrawArrowLeft(TB_TRUE);
            }
            if (i < m_EndlessStageSelectPage.GetCount()-1) {
                m_EndlessStageSelectPage[i].SetEnableDrawArrowRight(TB_TRUE);
            }
        }
        m_EndlessStageSelectPage[i].Initialize(i*EndlessStageSelectPage::STAGE_MAX_IN_GROUP,i!=0, i < m_EndlessStageSelectPage.GetCount()-1);
    }
    // 全体初期化
    Initialize(pageCount+1);
    InsertPage(0,&m_EndlessStageSelectPage[0]);
}

// デストラクタ
EndlessStageSelectSlide::~EndlessStageSelectSlide()
{
}

// 復帰
void EndlessStageSelectSlide::Resume(TbUint32 stageId)
{
    WorldManager& worldManager = WorldManager::GetInstance();
    TbSint32 stageIndex = 0;
    if (worldManager.IsEndlessStage(stageId, &stageIndex)) {
        m_StageIndex = stageIndex;
        TbForIterator(it, m_EndlessStageSelectPage) {
            if (it->SetStageIndex(stageIndex))
            {
                break;
            }
        }
    }
}

// ステージID取得
TbUint32 EndlessStageSelectSlide::GetStageId() const
{
    const EndlessStageInfo* info = WorldManager::GetInstance().GetEndlessStageInfo(m_StageIndex);
    if (info) {
        return info->stage;
    }
    return 0;
}

// 定期処理
void EndlessStageSelectSlide::OnUpdate()
{
    const CmnSlidePage* currentPage = GetCurrentPage();
    TbForIterator(it, m_EndlessStageSelectPage) {
        if (&(*it) == currentPage) {
            m_StageIndex = it->GetStageIndex();
        }
    }
}

// 描画
void EndlessStageSelectSlide::OnDraw()
{
    static TB_NDEBUG_CONST TbFloat32 TEXT_Y = 90.0f;
    static TB_NDEBUG_CONST TbFloat32 HISCORE_TEXT_Y = CMN_WINDOW_HEIGHT-90.0f;
    static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 96.0f;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
    const EndlessStageInfo* info = WorldManager::GetInstance().GetEndlessStageInfo(m_StageIndex);
    if(info ){
        font->Printf(x, TEXT_Y, info->name.GetBuffer());
    }
    if( 0 <= TbStrFind(info->name.GetBuffer(),"WATCH") ) {
        TbUint32 msec = SaveFile::GetInstance().GetBestMillSec(info->stage, Root::GetInstance().GetPlayerCount());
        if( TB_UINT32_MAX == msec )
        {
            font->Printf(x, HISCORE_TEXT_Y, "BEST: --:--");
        }
        else
        {
            TbUint32 sec = (msec / 100) % 100;
            msec %= 100;
            font->Printf(x, HISCORE_TEXT_Y, "BEST: %u:%u", sec, msec);
        }
    }else {
        TbUint32 hiscore = SaveFile::GetInstance().GetHiscore(info->stage, Root::GetInstance().GetPlayerCount());
        font->Printf(x, HISCORE_TEXT_Y, "BEST: %u", hiscore);
    }
    font->PopSetting();
}

// ページ変更
void EndlessStageSelectSlide::OnChangingPage(TbBool isNext)
{
}

// 次のページ取得
CmnSlidePage* EndlessStageSelectSlide::GetNextPage()
{
    const CmnSlidePage* currentPage = GetCurrentPage();
    TbForIterator(it, m_EndlessStageSelectPage) {
        if (&(*it) == currentPage) {
            if (it->IsSelected()) {
                return nullptr;
            }else{
                ++it;
                if (it != m_EndlessStageSelectPage.End()) {
                    return &(*it);
                }
            }
        }
    }
    return nullptr;
}
