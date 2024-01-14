/*!
 * メインメニュー用ページ
 * @author teco
 */

#include "fwd.h"
#include "cmn_slide.h"
#include "root.h"
#include "common/cmn_util.h"

// コンストラクタ
CmnSlide::CmnSlide()
    : m_CurrentPageIndex(0)
    , m_IsBusy(TB_FALSE)
    , m_IsCanceled(TB_FALSE)
    , m_Phase(0)
    , m_Page(nullptr)
    , m_PageMax(0)
{
}

// デストラクタ
CmnSlide::~CmnSlide()
{
    TB_SAFE_DELETE_ARRAY(m_Page);
}

// 初期化
void CmnSlide::Initialize(TbUint32 pageMax)
{
    TB_RESQUE(!m_Page);
    m_Page = new CmnSlidePage*[pageMax];
    TbFill<CmnSlidePage**>(m_Page, m_Page+pageMax,nullptr);
    m_PageMax =  pageMax;
}

// ページ挿入
void CmnSlide::InsertPage(TbUint32 index, CmnSlidePage* page)
{
    if (index < m_PageMax) {
        m_Page[index] = page;
    }
}

// ページを強制設定
void CmnSlide::SetPage(TbUint32 pageIndex)
{
    resetPage();
    m_CurrentPageIndex = pageIndex;
    TbFloat32 x = -(m_CurrentPageIndex) * CMN_WINDOW_WIDTH;
    m_OffsetPos.SetX(x);
}

// 定期処理
void CmnSlide::Update( TbFloat32 deltasec )
{
    static TB_NDEBUG_CONST TbFloat32 PAGE_SPEED = 10.0f;
    if (!m_IsBusy) {
        return;
    }

    switch (m_Phase)
    {
    case PHASE_UPDATE_PAGE:
        {
            if (SysInput::IsPress(SysInput::BUTTON_DECIDE, 0)) {
                if( m_CurrentPageIndex+1 < m_PageMax ){
                    // 次のページへ
                    resetPage();
                    m_Phase = PHASE_NEXT_PAGE;
                } else {
                    // 終了
                    m_IsBusy = TB_FALSE;
                }
            }
            else if (SysInput::IsPress(SysInput::BUTTON_CANCEL, 0)) {
                // 前のページへ
                if( 0 < m_CurrentPageIndex ){
                    resetPage();
                    m_Phase = PHASE_PREV_PAGE;
                }else {
                    m_IsBusy = TB_FALSE;
                    m_IsCanceled = TB_TRUE;
                }
            }
        }
        break;
    case PHASE_NEXT_PAGE:
        {
            TbFloat32 x = -(m_CurrentPageIndex+1) * CMN_WINDOW_WIDTH;
            m_OffsetPos.Add(-PAGE_SPEED,0.0f);
            if (m_OffsetPos.GetX() <= x) {
                setCurrentPage(m_CurrentPageIndex + 1);
                m_Phase = PHASE_UPDATE_PAGE;
            }

        }
        break;
    case PHASE_PREV_PAGE:
        {
            TbFloat32 x = -(m_CurrentPageIndex -1) * CMN_WINDOW_WIDTH;
            m_OffsetPos.Add(PAGE_SPEED, 0.0f);
            if (x <= m_OffsetPos.GetX()) {
                setCurrentPage(m_CurrentPageIndex-1);
                m_Phase = PHASE_UPDATE_PAGE;
            }
        }
        break;
    }
    
    for (TbSizeT i = 0; i < m_PageMax; ++i)
    {
        if (m_Page[i]) {
            m_Page[i]->Update(deltasec);
        }
    }
}

// 描画
void CmnSlide::Draw()
{
    TbVector2 pos;
    for (TbSizeT i = 0; i < m_PageMax; ++i)
    {
        pos = m_OffsetPos;
        pos.Add(i*CMN_WINDOW_WIDTH,0);
        if (m_Page[i]) {
            m_Page[i]->Draw(pos);
        }
    }
}