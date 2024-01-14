/*!
 * ���C�����j���[�p�y�[�W
 * @author teco
 */

#include "fwd.h"
#include "cmn_slide.h"
#include "root.h"
#include "common/cmn_util.h"

 /////////////////////
 //  CmnSlidePage
 ///////////////////// 

 // �J�����g�擾
TbBool CmnSlidePage::IsCurrent() const
{
    if (m_Slide) {
        return m_Slide->GetCurrentPage() == this;
    }
    return TB_FALSE;
}

// �`��
void CmnSlidePage::Draw(const TbVector2& pos)
{
    static TB_NDEBUG_CONST TbFloat32 ARROW_OFFSET_X = 540.0f;
    static TB_NDEBUG_CONST TbFloat32 ARROW_OFFSET_Y = CMN_WINDOW_HEIGHT/2.0f;

    OnDraw(pos);

    if(m_IsEnableDrawArrowLeft || m_IsEnableDrawArrowRight)
    {
        TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f) + pos.GetX();
        TbMatrix mtx;

        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowSprite); ++i) {
            m_ArrowSprite[i].Calculate();
        }
        if (m_IsEnableDrawArrowLeft) {
            mtx.SetTranslation(x - ARROW_OFFSET_X, ARROW_OFFSET_Y, 0.0f);
            m_ArrowSprite[IsChangingPrev()].Draw(mtx);
        }
        if (m_IsEnableDrawArrowRight) {
            mtx.SetTranslation(x + ARROW_OFFSET_X, ARROW_OFFSET_Y, 0.0f);
            mtx.Scale(-1.0f, 1.0f, 1.0f);
            m_ArrowSprite[IsChangingNext()].Draw(mtx);
        }
    }
}

// �O�y�[�W�������Ă���
TbBool CmnSlidePage::IsChangingPrev() const
{
    if (!m_Slide) {
        return TB_FALSE;
    }
    if (this != m_Slide->GetCurrentPage()) {
        return TB_FALSE;
    }
    return m_Slide->IsChangingPrev();
}

// ���y�[�W�������Ă���
TbBool CmnSlidePage::IsChangingNext() const
{
    if (!m_Slide) {
        return TB_FALSE;
    }
    if (this != m_Slide->GetCurrentPage()) {
        return TB_FALSE;
    }
    return m_Slide->IsChangingNext();
}

/////////////////////
//  CmnSlide
///////////////////// 

// �R���X�g���N�^
CmnSlide::CmnSlide()
    : m_CurrentPageIndex(0)
    , m_IsBusy(TB_FALSE)
    , m_IsCanceled(TB_FALSE)
    , m_IsPausePage(TB_FALSE)
    , m_IsVisible(TB_TRUE)
    , m_Phase(0)
    , m_Page(nullptr)
    , m_PageMax(0)
{
}

// �f�X�g���N�^
CmnSlide::~CmnSlide()
{
    TB_SAFE_DELETE_ARRAY(m_Page);
}

// ������
void CmnSlide::Initialize(TbUint32 pageMax)
{
    TB_RESQUE(!m_Page);
    m_Page = new CmnSlidePage*[pageMax];
    TbFill<CmnSlidePage**>(m_Page, m_Page+pageMax,nullptr);
    m_PageMax =  pageMax;
}

// �y�[�W�}��
void CmnSlide::InsertPage(TbUint32 index, CmnSlidePage* page)
{
    if (index < m_PageMax) {
        m_Page[index] = page;
        m_Page[index]->SetSlide(this);
    }
}

// �y�[�W����
void CmnSlide::ErasePage(TbUint32 index)
{
    if (index < m_PageMax) {
        m_Page[index]->SetSlide(nullptr);
        m_Page[index] = nullptr;
    }
}

// �y�[�W�������ݒ�
void CmnSlide::SetPage(TbUint32 pageIndex)
{
    resetPage();
    m_CurrentPageIndex = pageIndex;
    TbFloat32 x = -(m_CurrentPageIndex) * CMN_WINDOW_WIDTH;
    m_OffsetPos.SetX(x);
}

// �O�̃y�[�W�������Ă���
TbBool CmnSlide::HasPrevPage() const
{    
    TbSint32 index = m_CurrentPageIndex-1;
    if (index < 0) {
        return TB_FALSE;
    }
    const CmnSlidePage* page = GetCurrentPage();
    if (!page) {
        return TB_FALSE;
    }
    TbSint32 depth = page->GetDepth();
    return depth == m_Page[index]->GetDepth();
}

// ���̃y�[�W�������Ă���
TbBool CmnSlide::HasNextPage() const
{
    TbSint32 index = m_CurrentPageIndex + 1;
    if (!m_Page[index] || TB_ARRAY_COUNT_OF(m_Page) <= index ) {
        return TB_FALSE;
    }
    const CmnSlidePage* page = GetCurrentPage();
    if (!page) {
        return TB_FALSE;
    }
    TbSint32 depth = page->GetDepth();
    return depth == m_Page[index]->GetDepth();
}

// �������
void CmnSlide::Update( TbFloat32 deltasec )
{
    static TB_NDEBUG_CONST TbFloat32 PAGE_SPEED = 15.0f;
    if (!m_IsBusy) {
        return;
    }
    OnUpdate();

    CmnSlidePage::Result result = CmnSlidePage::RESULT_PENDING;
    if( !m_IsPausePage )
    {
        for (TbSizeT i = 0; i < m_PageMax; ++i)
        {
            if (m_Page[i]) {
                CmnSlidePage::Result ret = m_Page[i]->Update(deltasec);
                if (i == m_CurrentPageIndex) {
                    result = ret;
                }
            }
        }
    }

    switch (m_Phase)
    {
    case PHASE_UPDATE_PAGE:
        {
            if (CmnSlidePage::RESULT_NEXT== result) {
                CmnSlidePage* page = GetNextPage();
                if (page) {
                    InsertPage(m_CurrentPageIndex + 1, page);
                }
                if( page ){
                    // ���̃y�[�W��
                    OnChangingPage(TB_TRUE);
                    resetPage();
                    m_Phase = PHASE_NEXT_PAGE;
                } else {
                    // �I��
                    m_IsBusy = TB_FALSE;
                }
            }
            else if (CmnSlidePage::RESULT_PREV == result) {
                // �O�̃y�[�W��
                if( 0 < m_CurrentPageIndex ){
                    OnChangingPage(TB_FALSE);
                    resetPage();
                    m_Phase = PHASE_PREV_PAGE;
                }else {
                    m_IsBusy = TB_FALSE;
                    m_IsCanceled = TB_TRUE;
                }
            }
            else if (CmnSlidePage::RESULT_PARENT == result) 
            {
                // �e�̃y�[�W��
                TbSint32 depth = GetCurrentPage()->GetDepth();
                while (0 < m_CurrentPageIndex) {
                    TbSint32 prevDepth = m_Page[m_CurrentPageIndex-1]->GetDepth();
                    if (prevDepth != depth) {
                        break;
                    }
                    ErasePage(m_CurrentPageIndex -1);
                    --m_CurrentPageIndex;
                }
                if (0 < m_CurrentPageIndex) {
                    OnChangingPage(TB_FALSE);
                    resetPage();
                    m_Phase = PHASE_PREV_PAGE;
                }
                else {
                    m_IsBusy = TB_FALSE;
                    m_IsCanceled = TB_TRUE;
                }
            }
            else if (CmnSlidePage::RESULT_RETURN == result) 
            {
                m_IsBusy = TB_FALSE;
                m_IsCanceled = TB_TRUE;
            }
        }
        break;
    case PHASE_NEXT_PAGE:
        {
            TbFloat32 x = -(m_CurrentPageIndex+1) * CMN_WINDOW_WIDTH;
            m_OffsetPos.Add(-PAGE_SPEED,0.0f);
            if( m_OffsetPos.GetX() <= x ) {
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
                ErasePage(m_CurrentPageIndex);
                setCurrentPage(m_CurrentPageIndex-1);
                m_Phase = PHASE_UPDATE_PAGE;
            }
        }
        break;
    }
}

// �`��
void CmnSlide::Draw()
{
    if (!m_IsVisible){
        return;
    }

    OnDraw();
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