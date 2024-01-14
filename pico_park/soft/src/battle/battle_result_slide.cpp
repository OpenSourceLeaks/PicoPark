/*!
* バトルステージ選択用ページ
* @author teco
*/

#include "fwd.h"
#include "battle/battle_result_slide.h"
#include "common/cmn_global_data.h"
#include "common/cmn_util.h"
#include "root.h"
#include "battle/battle_manager.h"
#include <base/algorithm/tb_algorithm_misc.h>

//////////////////////////////////////////
// BattleResultPage
/////////////////////////////////////////

static TB_NDEBUG_CONST TbFloat32 SPRITE_SCALE = 1.5f;
static TB_NDEBUG_CONST TbFloat32 RESULT_SEC = 2.0f;
static TB_NDEBUG_CONST TbFloat32 FINISH_SEC = 4.0f;

static const TbRectF32Pod WIN_ICON_UV[] =
{ 
    CMN_TEX_UV_RECT(416,160,16,16) ,
    CMN_TEX_UV_RECT(384,160,16,16) ,
};

// コンストラクタ
BattleResultPage::BattleResultPage()
    : m_Sec(0.0f)
    , m_IsNewWin(TB_FALSE)
{
    static const TbRectF32 ICON_RECT =
        { -22.0f , -24.0f , 44.0f , 48.0f };

    static const TbRectF32 ICON_UV =
        { CMN_TEX_UV_RECT(32 * 5 + 5,8,22,24) };

    static const TbRectF32 HELMENT_ICON_RECT =
    { -22.0f , -24.0f , 44.0f , 48.0f };


    static TbFloat32 HEAD_WIDTH = 25.0f;
    static TbFloat32 HEAD_HEIGHT = 13.0f;
    static const TbRectF32Pod HELMET_RECT = { -HEAD_WIDTH , -HEAD_HEIGHT-20.0f , HEAD_WIDTH*2.0f , HEAD_HEIGHT*2.0f };
    static const TbRectF32Pod HELMET_ICON_UV = { CMN_TEX_UV_RECT(256,464,HEAD_WIDTH - 1,HEAD_HEIGHT - 1) };

    static const TbRectF32 WIN_ICON_RECT =
    { -16.0f , -16.0f , 32.0f , 32.0f };

    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_PlayerInfo); ++i)
    {
        m_PlayerInfo[i].playerSprite.Initialize(ICON_RECT,ICON_UV);
        m_PlayerInfo[i].playerSprite.SetColor(CmnUtil::GetPlayerColor(i));
        m_PlayerInfo[i].playerSprite.SetMaterial(&m_PlayerMaterial);

        m_PlayerInfo[i].helmetSprite.Initialize(HELMET_RECT, HELMET_ICON_UV);
        m_PlayerInfo[i].helmetSprite.SetColor(CmnUtil::GetPlayerColor(i));
        m_PlayerInfo[i].helmetSprite.SetMaterial(&m_PlayerMaterial);

        for (TbSizeT j = 0; j < TB_ARRAY_COUNT_OF(m_PlayerInfo[i].winSpirte); ++j)
        {
            m_PlayerInfo[i].winSpirte[j].Initialize(WIN_ICON_RECT,WIN_ICON_UV[0]);
            m_PlayerInfo[i].winSpirte[j].SetColor(TbColor::White());
        }
    }

}

// 定期処理
CmnSlidePage::Result BattleResultPage::OnUpdate(TbFloat32 deltasec)
{
    Result result = CmnSlidePage::RESULT_PENDING;
    m_Sec += deltasec;
    if (FINISH_SEC <= m_Sec) 
    {
        result = CmnSlidePage::RESULT_NEXT;
    }
    return result;
}

// 現在のページになったか
void BattleResultPage::OnChangeActive(TbBool isActive)
{
}

// 描画処理
void BattleResultPage::OnDraw(const TbVector2& offset)
{
    static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 64.0f;
    static TB_NDEBUG_CONST TbFloat32 HELMENT_X = -2.0f;
    static TB_NDEBUG_CONST TbFloat32 HELMENT_Y = -40.0f;
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    TbUint32 latestWinner = BattleManager::GetInstance().GetLatestWinner();
    TbFontBuffer2d* font = Root::GetInstance().GetLocalFont();
    font->PushSetting();
    font->SetFontSize(24);
    font->SetColor(TbColor::Black());
    font->SetPoseMatrix(TbMatrix::IDENTITY);

    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        BattleManager::PlayerResult result = BattleManager::GetInstance().GetResult(i);
        TbVector2 pos = getPlayerPos(i);
        TbMatrix mtx;
        mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
        mtx.Scale(SPRITE_SCALE, SPRITE_SCALE, 1.0f);
        m_PlayerInfo[i].playerSprite.Calculate();
        m_PlayerInfo[i].playerSprite.Draw(mtx);
        if ( SaveFile::GetInstance().IsVisiblePlayerNumber() )
        {
            m_PlayerInfo[i].helmetSprite.Calculate();
            m_PlayerInfo[i].helmetSprite.Draw(mtx);
            TbStaticString32 text = TbStaticString32::Format("%u",i+1);
            font->Printf(pos.GetX() + HELMENT_X,
                         pos.GetY() + HELMENT_Y, text.GetBuffer());
        }
        for (TbUint32 w = 0; w < result.winCount; ++w)
        {
            if (i == latestWinner && w == result.winCount - 1)
            {
                if (m_Sec < RESULT_SEC) {
                    continue;
                }
                if (!m_IsNewWin)
                {
                    TbPlaySound("win");
                    m_IsNewWin = TB_TRUE;
                }
            }
            m_PlayerInfo[i].winSpirte[w].SetUV(WIN_ICON_UV[1]);
        }
        for (TbUint32 w = 0; w < BattleManager::WIN_MAX; ++w)
        {
            mtx.Translate(OFFSET_X, 0.0f, 0.0f);
            m_PlayerInfo[i].winSpirte[w].Calculate();
            m_PlayerInfo[i].winSpirte[w].Draw(mtx);
        }
    }

    font->PopSetting();
    font->Flush();
}

// プレイヤー位置取得
TbVector2 BattleResultPage::getPlayerPos(TbUint32 playerIndex)
{
//    static TbFloat32 SPRITE_INTERVAL_X = 350.0f;
    static const TbUint32 PLAYER_HALF = 4;
    static TB_NDEBUG_CONST TbFloat32 CENTER_X = CMN_WINDOW_WIDTH/2.0f;
    static TB_NDEBUG_CONST TbFloat32 CENTER_Y = (CMN_WINDOW_HEIGHT/2.0f)+70.0f;
    static TB_NDEBUG_CONST TbFloat32 INTERVAL_Y = 100.0f;
    static TB_NDEBUG_CONST TbFloat32 CELL_WIDTH = 200.0f;

    TbVector2 pos;
    pos.SetY(CENTER_Y);
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();

    if (PLAYER_HALF < playerCount)
    {
        static TB_NDEBUG_CONST TbFloat32 ALL_OFFSET_X = 30.0f;
        static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 230.0f;
        // 2列
        pos.Add(0.0f, -(INTERVAL_Y / 2.0f) - INTERVAL_Y);
        if (playerIndex < PLAYER_HALF)
        {
            // 1列目
            pos.SetX(CENTER_X - OFFSET_X - CELL_WIDTH / 2.0f );
            pos.Add(-ALL_OFFSET_X, playerIndex*INTERVAL_Y);
        }
        else
        {
            // 2列目
            pos.SetX(CENTER_X + OFFSET_X - CELL_WIDTH/2.0f);
            pos.Add(-ALL_OFFSET_X,(playerIndex-PLAYER_HALF)*INTERVAL_Y);
        }
    }
    else 
    {
        static TB_NDEBUG_CONST TbFloat32 OFFSET_Y_TABLE[] =
        {
            0.0f,
            0.0f,
            -INTERVAL_Y/2.0f,
            -INTERVAL_Y/4.0f,
            0.0f,
        };
        static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 60.0f;
        // 中心
        pos.SetX(CENTER_X-CELL_WIDTH/2.0f);
        if ( 0 == playerCount % 2) {
            pos.Add(0.0f,-INTERVAL_Y/2.0f - INTERVAL_Y*(playerCount/2-1));
        }else {
            pos.Add(0.0f,-INTERVAL_Y*(playerCount/2));
        }
        pos.Add(-OFFSET_X, playerIndex*INTERVAL_Y + OFFSET_Y_TABLE[playerCount]);
    }

    return pos;
}

//////////////////////////////////////////
// MainMenuPageManager
/////////////////////////////////////////

// コンストラクタ
BattleResultSlide::BattleResultSlide()
    : Super()
{
    // 全体初期化
    Initialize(1);
    InsertPage(0, &m_BattleResultPage);
}

// デストラクタ
BattleResultSlide::~BattleResultSlide()
{
}

// 定期処理
void BattleResultSlide::OnUpdate()
{
}

// 描画
void BattleResultSlide::OnDraw()
{
    static TB_NDEBUG_CONST TbFloat32 TEXT_Y = 90.0f;
    static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 96.0f;
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    font->SetFontSize(FONT_SIZE);

    TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
    font->Printf(x, TEXT_Y, "RESULT");
    font->PopSetting();
}

// ページ変更
void BattleResultSlide::OnChangingPage(TbBool isNext)
{
}

// 次のページ取得
CmnSlidePage* BattleResultSlide::GetNextPage()
{
    return nullptr;
}
