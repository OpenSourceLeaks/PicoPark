/*!
 * ���C�����j���[�p�y�[�W
 * @author teco
 */

#ifndef _CMN_SLIDE_H_
#define _CMN_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/util/tb_dynamic_cast.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/cmn_simple_sprite.h"

class CmnSlide;

class CmnSlidePage
{
public:

    enum Result
    {
        RESULT_PENDING ,
        RESULT_NEXT , 
        RESULT_PREV ,
        RESULT_PARENT ,
        RESULT_RETURN 
    };

public:
    // �R���X�g���N�^
    CmnSlidePage()
        : m_IsActive(TB_FALSE)
        , m_IsEnableDrawArrowLeft(TB_FALSE)
        , m_IsEnableDrawArrowRight(TB_FALSE)
        , m_Depth(0)
        , m_Phase(0)
        , m_InitPhase(0)
        , m_PhaseSec(0.0f)
        , m_Slide(nullptr)
    {
        TbRectF32 rect(-64.0f, -32.0f, 64.0f, 64.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(0.0f, 640.0f, 48.0f, 48.0f));
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowSprite); ++i) {
            TbRectF32 uv = uvRect;
            uv.x += CMN_TEX_POS(48.0f)*i;
            m_ArrowSprite[i].Initialize(rect, uv);
            m_ArrowSprite[i].SetColor(TbColor::White());
        }
    }
    // �f�X�g���N�^
    virtual ~CmnSlidePage() {}
public:
    TB_DEFINE_BASE_CLASS(CmnSlidePage);
public:
    // �L��
    void SetActive(TbBool isActive) {
        if (TB_BOOL_NOT_EQUAL(m_IsActive, isActive)) {
            m_IsActive = isActive;
            m_Phase = m_InitPhase;
            m_PhaseSec = 0.0f;
            OnChangeActive(m_IsActive);
        }
    }
    // �L����
    TbBool IsActive() const {
        return m_IsActive;
    }
    // �����Ă���X���C�h�ݒ�
    void SetSlide(CmnSlide* slide) {
        TB_ASSERT(!slide || !m_Slide);
        m_Slide = slide;
        OnChangeInsert(slide!=nullptr);
    }
    // �J�����g�擾
    TbBool IsCurrent() const;

    // �[�x�擾
    TbSint32 GetDepth() const {
        return m_Depth;
    }

    // �[�x�ݒ�
    void SetDepth(TbSint32 layer) {
        m_Depth = layer;
    }

    // ���`��\��
    void SetEnableDrawArrowLeft(TbBool isEnable) {
        m_IsEnableDrawArrowLeft = isEnable;
    }

    void SetEnableDrawArrowRight(TbBool isEnable) {
        m_IsEnableDrawArrowRight = isEnable;
    }

public:
    // �X�V
    Result Update(TbFloat32 deltasec) {
        Result result = RESULT_PENDING;
        if (m_IsActive) {
            result = OnUpdate(deltasec);
            m_PhaseSec += deltasec;
        }
        return result;
    }

    // �`��
    void Draw(const TbVector2& pos);

protected:

    // �p���������
    virtual Result OnUpdate(TbFloat32 deltasec) { return RESULT_PENDING; }

    // �p���`�揈��
    virtual void   OnDraw(const TbVector2& pos) {}

    // �A�N�e�B�u��Ԃ��ω�����
    virtual void OnChangeActive( TbBool isActive ) {}

    // �y�[�W�o�^���ꂽ
    virtual void OnChangeInsert(TbBool isInserted) {}

    // �t�F�[�Y�擾
    TbSint32 GetPhase() const {
        return m_Phase;
    }

    // �t�F�[�Y���Ԏ擾
    TbFloat32 GetPhaseSec() const {
        return m_PhaseSec;
    }

    // �t�F�[�Y�i�s
    void AddPhase() {
        ++m_Phase;
        m_PhaseSec = 0.0f;
    }

    // �t�F�[�Y�ݒ�
    void SetPhase(TbSint32 phase) {
        m_Phase = phase;
        m_PhaseSec = 0.0f;
    }

    // �����t�F�[�Y�ݒ�
    void SetInitPhase(TbSint32 phase) {
        m_InitPhase = phase;
    }

private:

    enum
    {
        ARROW_DEFAULT,
        ARROW_SELECT,
        ARROW_MAX
    };

private:

    TbBool IsChangingNext() const;

    TbBool IsChangingPrev() const;

private:

    TbBool          m_IsActive;
    TbBool          m_IsEnableDrawArrowLeft;
    TbBool          m_IsEnableDrawArrowRight;
    TbSint32        m_Depth;
    TbSint32        m_Phase;
    TbSint32        m_InitPhase;
    TbFloat32       m_PhaseSec;
    CmnSimpleSprite	m_ArrowSprite[ARROW_MAX];
    CmnSlide*       m_Slide;
};

class CmnSlide
{
public:

	// �R���X�g���N�^s
    CmnSlide();

	// �f�X�g���N�^
	virtual ~CmnSlide();

public:

    // ������
    void Initialize( TbUint32 pageMax );

    // �y�[�W�}��
    void InsertPage( TbUint32 index, CmnSlidePage* page );

    // �y�[�W����
    void ErasePage( TbUint32 index );

public:

	// �J�n
    void Start() {
        m_IsBusy = TB_TRUE;
        m_IsCanceled = TB_FALSE;
        setCurrentPage(m_CurrentPageIndex);
    }

    // ��~
    void Stop() {
        m_IsBusy = TB_FALSE;
        m_IsCanceled = TB_TRUE;
    }

	// �r�W�[���ǂ���
    TbBool IsBusy() const {
		return m_IsBusy;
    }

    // �L�����Z��������
    TbBool IsCanceled() const {
        return  m_IsCanceled;
    }

    // �y�[�W���ꎞ�|�[�Y
    void SetPausePage(TbBool isPause)
    {
        m_IsPausePage = isPause;
    }

    // �y�[�W����
    TbBool IsEmptyPage() const {
        return m_Page[0] == nullptr;
    }

    // �y�[�W�������ݒ�
    void SetPage( TbUint32 pageIndex );

    // �y�[�W�擾
    CmnSlidePage* GetPage(TbUint32 pageIndex) const {
        return m_Page[pageIndex];
    }

    // ���݂̃y�[�W�ԍ�
    TbUint32 GetCurentPageIndex() const {
        return m_CurrentPageIndex;
    }

    // ���݂̃y�[�W�擾
    const CmnSlidePage* GetCurrentPage() const {
        return m_Page[m_CurrentPageIndex];
    }

    // �O�̃y�[�W�������Ă���
    TbBool HasPrevPage() const;

    // ���̃y�[�W�������Ă���
    TbBool HasNextPage() const;

    // �O�̃y�[�W�Ɉڍs��
    TbBool IsChangingPrev() const {
        return m_Phase == PHASE_PREV_PAGE;
    }

    // ���̃y�[�W�Ɉڍs��
    TbBool IsChangingNext() const {
        return m_Phase == PHASE_NEXT_PAGE;
    }

    // 
    void SetVisible(TbBool isVisible) {
        m_IsVisible = isVisible;
    }

public:

    // �������
    void Update(TbFloat32 deltasec);

    // �`�揈��
    void Draw();

protected:

    // �������
    virtual void OnUpdate() {}

    // �`��
    virtual void OnDraw() {}

    // �y�[�W�ύX�J�n
    virtual void OnChangingPage(TbBool isNext) {}

    // ���̃y�[�W�擾
    virtual CmnSlidePage* GetNextPage() = 0;

private:

    enum
    {
		PHASE_UPDATE_PAGE ,
		PHASE_NEXT_PAGE   , 
		PHASE_PREV_PAGE   ,
    };

private:

    // ���݂̃y�[�W�ݒ�
    void setCurrentPage(TbSint32 index) {
        TB_RESQUE(m_Page[index]);
        resetPage();
        m_CurrentPageIndex = index;
        m_Page[m_CurrentPageIndex]->SetActive(TB_TRUE);
    }

    // �y�[�W���Z�b�g
    void resetPage() {
        for (TbSizeT i = 0; i < m_PageMax; ++i) {
            if (m_Page[i]) {
                m_Page[i]->SetActive(TB_FALSE);
            }
        }
    }

private:

    TbBool			  m_IsBusy;
    TbBool            m_IsCanceled;
    TbBool            m_IsPausePage;
    TbBool            m_IsVisible;
    TbSint32		  m_Phase;
    TbSint32		  m_CurrentPageIndex;
    TbVector2		  m_OffsetPos;
    CmnSlidePage**    m_Page;
    TbUint32          m_PageMax;

};

#endif