/*!
 * ���C�����j���[�p�y�[�W
 * @author teco
 */

#ifndef _CMN_SLIDE_H_
#define _CMN_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/cmn_simple_sprite.h"

class CmnSlidePage
{
public:
    // �R���X�g���N�^
    CmnSlidePage()
        : m_IsActive(TB_FALSE)
        , m_Phase(0)
        , m_PhaseSec(0.0f)
    {}
    // �f�X�g���N�^
    virtual ~CmnSlidePage() {}
public:
    // �L��
    void SetActive(TbBool isActive) {
        if (TB_BOOL_NOT_EQUAL(m_IsActive, isActive)) {
            m_IsActive = isActive;
            m_Phase = 0;
            m_PhaseSec = 0.0f;
        }
    }
    // �L����
    TbBool IsActive() const {
        return m_IsActive;
    }
public:
    // �X�V
    void Update(TbFloat32 deltasec) {
        if (m_IsActive) {
            OnUpdate(deltasec);
            m_PhaseSec += deltasec;
        }
    }
    // �`��
    virtual void Draw(const TbVector2& pos) {}
protected:

    // �p���������
    virtual void OnUpdate(TbFloat32 deltasec) {}

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

private:
    TbBool       m_IsActive;
    TbSint32    m_Phase;
    TbFloat32   m_PhaseSec;
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

public:

	// �J�n
    void Start() {
        m_IsBusy = TB_TRUE;
        m_IsCanceled = TB_FALSE;
        setCurrentPage(m_CurrentPageIndex);
    }

	// �r�W�[���ǂ���
    TbBool IsBusy() const {
		return m_IsBusy;
    }

    // �L�����Z��������
    TbBool IsCanceled() const {
        return  m_IsCanceled;
    }

    // �y�[�W�������ݒ�
    void SetPage( TbUint32 pageIndex );

public:

    // �������
    void Update(TbFloat32 deltasec);

    // �`�揈��
    void Draw();

protected:

    // �y�[�W�ύX�J�n
    virtual void OnChangingPage( TbUint32 page ) {}

private:

    enum
    {
		PHASE_UPDATE_PAGE ,
		PHASE_NEXT_PAGE   , 
		PHASE_PREV_PAGE   ,
    };

private:

    void setCurrentPage(TbSint32 index) {
        TB_RESQUE(m_Page[index]);
        resetPage();
        m_CurrentPageIndex = index;
        m_Page[m_CurrentPageIndex]->SetActive(TB_TRUE);
    }

    void resetPage() {
        for (TbSizeT i = 0; i < m_PageMax; ++i) {
            if (m_Page[i]) {
                m_Page[i]->SetActive(TB_FALSE);
            }
        }
    }

private:
    TbBool						m_IsBusy;
    TbBool                      m_IsCanceled;
    TbSint32					m_Phase;
    TbSint32					m_CurrentPageIndex;
    TbVector2			        m_OffsetPos;
    CmnSlidePage**              m_Page;
    TbUint32                    m_PageMax;

};

#endif