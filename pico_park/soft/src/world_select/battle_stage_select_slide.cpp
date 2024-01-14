/*!
 * バトルステージ選択用ページ
 * @author teco
 */

#include "fwd.h"
#include "battle_stage_select_slide.h"
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
// BattleStageSelectPage
/////////////////////////////////////////

static TB_NDEBUG_CONST TbFloat32 SPRITE_SCALE = 2.6f;

// コンストラクタ
BattleStageSelectPage::BattleStageSelectPage()
    : m_HasPrev(TB_FALSE)
    , m_HasNext(TB_FALSE)
    , m_StageWidth(1)
    , m_StageHeight(1)
    , m_IsSelected(TB_FALSE)
    , m_IsConfirming(TB_FALSE)
    , m_ConfirmWindow(nullptr)
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
void BattleStageSelectPage::Initialize( TbSint32 begin , TbBool hasPrev , TbBool hasNext )
{
    m_HasPrev = hasPrev;
    m_HasNext = hasNext;

    TbSint32 stageCount = WorldManager::GetInstance().GetBattleStageCount();
    m_StageRange.begin = begin;
    m_StageRange.end = TbMin<TbSint32>(begin+STAGE_MAX_IN_GROUP,stageCount);
    TbSint32 dispStageCount = m_StageRange.end - m_StageRange.begin;
    m_StageHeight = 1; // TbMin<TbUint32>(dispStageCount,2);
    m_StageWidth  = dispStageCount; // static_cast<TbUint32>(TbCeil(static_cast<TbFloat32>(dispStageCount)/ 2.0f));

    // ワールド
    {
        TbDimensionF32 size(64.0f, 64.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512.0f, 640.0f, size.width, size.height));
        TbRectF32 rect(-size.width / 2.0f, -size.height / 2.0f, size.width, size.height);
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Group.sprite); ++i)
        {
            const BattleStageInfo* info = WorldManager::GetInstance().GetBattleStageInfo(i+m_StageRange.begin);
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
CmnSlidePage::Result BattleStageSelectPage::OnUpdate(TbFloat32 deltasec)
{
    m_CursorSprite.Update(deltasec);

    // 確認中
    if (m_IsConfirming)
    {
        if (!m_ConfirmWindow->IsOpened())
        {
            if (UiMessageWindow::RESULT_YES == m_ConfirmWindow->GetResult())
            {
                m_IsSelected = TB_TRUE;
                return CmnSlidePage::RESULT_NEXT;
            }
            Root::GetInstance().SetEnableTranslucent(TB_FALSE);
            m_IsConfirming = TB_FALSE;
        }
        return CmnSlidePage::RESULT_PENDING;
    }

    TbUint32 playerIndex = 0;
    // セレクト
    Result result = CmnSlidePage::RESULT_PENDING;
    if (CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_LEFT,CR_INPUT_PLAYER_ALL_BIT)) {
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
    else if (CrInput::IsAnyonePress(CrInput::BUTTON_DECIDE, CR_INPUT_PLAYER_ALL_BIT,CrInput::MODE_BASE,&playerIndex)) {
        if (m_ConfirmWindow) {
            const BattleStageInfo* info = WorldManager::GetInstance().GetBattleStageInfo(GetStageIndex());
            if (info) {
                TbStaticString64 msg = TbStaticString64::Format("%s?", info->name.GetBuffer());
                m_ConfirmWindow->SetMessage(msg.GetBuffer());
                m_ConfirmWindow->SetPlayerBit(TB_BIT(playerIndex));
                m_ConfirmWindow->Open();
                Root::GetInstance().SetEnableTranslucent(TB_TRUE);
                m_IsConfirming = TB_TRUE;
            }else {
                m_IsSelected = TB_TRUE;
            }
        }else {
            m_IsSelected = TB_TRUE;
        }
        if (m_IsSelected) {
            result = CmnSlidePage::RESULT_NEXT;
        }
    }
    return result;
}

// 現在のページになったか
void BattleStageSelectPage::OnChangeActive(TbBool isActive) 
{
    m_IsSelected = TB_FALSE;
}

// 描画処理
void BattleStageSelectPage::OnDraw(const TbVector2& offset)
{
    static TB_NDEBUG_CONST TbFloat32 SPRITE_INTERVAL_X = 300.0f;
    static TB_NDEBUG_CONST TbFloat32 SPRITE_INTERVAL_Y = 260.0f;

    static TB_NDEBUG_CONST TbFloat32 CENTER_X = (CMN_WINDOW_WIDTH / 2.0f);
    TbFloat32 beginX = CENTER_X;
    if ( m_StageWidth % 2 == 0 ) {
        beginX -= (SPRITE_INTERVAL_X / 2);
        beginX -= (SPRITE_INTERVAL_X)*((m_StageWidth/2)-1);
    } else {
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
TbBool BattleStageSelectPage::moveCursor(TbSint32 movX, TbSint32 movY)
{
    TbPointS32 point = m_FocusPoint;
    point.x = TbClamp<TbSint32>(m_FocusPoint.x+movX,0, m_StageWidth-1);
    point.y = TbClamp<TbSint32>(m_FocusPoint.y+movY,0, m_StageHeight-1);
    m_FocusPoint = point;
    return TB_TRUE;
}

// 位置からステージインデックス取得
TbSint32 BattleStageSelectPage::getStageIndex(TbSint32 x, TbSint32 y) const
{
    TbSint32 stageIndex = x*m_StageHeight+y;
    return stageIndex;
}

// ワールドインデックスから座標取得
TbPointS32 BattleStageSelectPage::getPoint(TbSint32 stageIndex) const
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
BattleStageSelectSlide::BattleStageSelectSlide()
    : Super()
    , m_StageIndex(0)
    , m_BackConfirmWindow(UiMessageWindow::TYPE_YESNO)
    , m_BackConfirmSleep(0)
    , m_ConfirmWindow(UiMessageWindow::TYPE_YESNO)
{
    TbUint32 battleStageCount = WorldManager::GetInstance().GetBattleStageCount();
    TbUint32 pageCount = ((battleStageCount -1)/BattleStageSelectPage::STAGE_MAX_IN_GROUP)+1;

    // 各ページの初期化
    m_BattleStageSelectPage.PushBackDefault(pageCount);
    for (TbSint32 i = 0; i < m_BattleStageSelectPage.GetCount(); ++i) {
        if( 2 <= m_BattleStageSelectPage.GetCount() ){
            if (i != 0) {
                m_BattleStageSelectPage[i].SetEnableDrawArrowLeft(TB_TRUE);
            }
            if (i < m_BattleStageSelectPage.GetCount()-1) {
                m_BattleStageSelectPage[i].SetEnableDrawArrowRight(TB_TRUE);
            }
        }
        m_BattleStageSelectPage[i].Initialize(i*BattleStageSelectPage::STAGE_MAX_IN_GROUP,i!=0, i < m_BattleStageSelectPage.GetCount()-1);
        m_BattleStageSelectPage[i].SetConfirmWindow(&m_ConfirmWindow);
    }

    // 全体初期化
    Initialize(pageCount+1);
    InsertPage(0,&m_BattleStageSelectPage[0]);

    m_BackConfirmWindow.SetDecidedCallback(TbCreateMemFunc(this, &Self::onDecidedBackConfirm));
    m_BackConfirmWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f));
    m_BackConfirmWindow.SetAlign(TB_UI_ALIGN_X_CENTER, TB_UI_ALIGN_Y_CENTER);
    m_BackConfirmWindow.SetMessage("FINISH BATTLE?");

    m_ConfirmWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f));
    m_ConfirmWindow.SetAlign(TB_UI_ALIGN_X_CENTER, TB_UI_ALIGN_Y_CENTER);
}

// デストラクタ
BattleStageSelectSlide::~BattleStageSelectSlide()
{
}

// ステージID取得
TbUint32 BattleStageSelectSlide::GetStageId() const
{
    const BattleStageInfo* info = WorldManager::GetInstance().GetBattleStageInfo(m_StageIndex);
    if (info) {
        return info->stage;
    }
    return 0;
}

// 定期処理
void BattleStageSelectSlide::OnUpdate()
{
    if (!m_BackConfirmWindow.IsOpened() && !m_ConfirmWindow.IsOpened() )
    {
        if (0 < m_BackConfirmSleep) {
            --m_BackConfirmSleep;
        }
        if (0 == m_BackConfirmSleep)
        {
            TbUint32 playerIndex = 0;
            if (CrInput::IsAnyonePress(CrInput::BUTTON_CANCEL, CR_INPUT_PLAYER_ALL_BIT, CrInput::MODE_BASE, &playerIndex)) {
                m_BackConfirmWindow.SetPlayerBit(TB_BIT(playerIndex));
                m_BackConfirmWindow.Open();
                Root::GetInstance().SetEnableTranslucent(TB_TRUE);
                SetPausePage(TB_TRUE);
            }
            else {
                const CmnSlidePage* currentPage = GetCurrentPage();
                TbForIterator(it, m_BattleStageSelectPage) {
                    if (&(*it) == currentPage) {
                        m_StageIndex = it->GetStageIndex();
                    }
                }
            }
        }
    }
    else {
        m_BackConfirmSleep = 5;
    }
}

// 描画
void BattleStageSelectSlide::OnDraw()
{
    static TB_NDEBUG_CONST TbFloat32 TEXT_Y = 90.0f;
    static TB_NDEBUG_CONST TbFloat32 ROUND_TEXT_Y = CMN_WINDOW_HEIGHT-90.0f;
    static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 96.0f;
    TbFontBuffer2d* font     = Root::GetInstance().GetLocalFont();

    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetPoseMatrix(TbMatrix::IDENTITY);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
    const BattleStageInfo* info = WorldManager::GetInstance().GetBattleStageInfo(m_StageIndex);
    if( info ){
        font->Printf(x, TEXT_Y, info->name.GetBuffer());
    }

    TbUint32 round = BattleManager::GetInstance().GetRound()+1;
    font->Printf(x, ROUND_TEXT_Y, "ROUND %u" , round);
    font->PopSetting();
    font->Flush();
}

// ページ変更
void BattleStageSelectSlide::OnChangingPage(TbBool isNext)
{
}

// 次のページ取得
CmnSlidePage* BattleStageSelectSlide::GetNextPage()
{
    const CmnSlidePage* currentPage = GetCurrentPage();
    TbForIterator(it, m_BattleStageSelectPage) {
        if (&(*it) == currentPage) {
            if (it->IsSelected()) {
                return nullptr;
            }else{
                ++it;
                if (it != m_BattleStageSelectPage.End()) {
                    return &(*it);
                }
            }
        }
    }
    return nullptr;
}

// 確認ウィンドウのコールバック
void BattleStageSelectSlide::onDecidedBackConfirm(UiMessageWindow::Result result)
{
    if (result == UiMessageWindow::RESULT_YES)
    {
        Stop();
    }
    else 
    {
        Root::GetInstance().SetEnableTranslucent(TB_FALSE);
    }
    SetPausePage(TB_FALSE);
}
