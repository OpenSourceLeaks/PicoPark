/*!
 * ���C�����j���[�p�y�[�W
 * @author teco
 */

#ifndef _INCLUDED_MAIN_MENU_PAGE_H_
#define _INCLUDED_MAIN_MENU_PAGE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"
#include "common/ui/ui_option_window.h"

class MainMenuModeSelectPage : public CmnSlidePage
{
public:

    enum
    {
        MODE_COOP ,
        MODE_CHALLENGE,
        MODE_BATTLE ,
        MODE_OPTION ,
        MODE_MAX
    };
    static const TbUint32 MODE_MAX_IN_PAGE = 2;
    static const TbUint32 MODE_PAGE_MAX = MODE_MAX/MODE_MAX_IN_PAGE;

    typedef MainMenuModeSelectPage Self;
    typedef CmnSlidePage Super;

public:

    // �R���X�g���N�^
    MainMenuModeSelectPage();

    // �f�X�g���N�^
    virtual ~MainMenuModeSelectPage() {}

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public:

    // �I���C���f�b�N�X�擾
    TbSint32 GetMode() const {
        return m_Index + (m_PageIndex*MODE_MAX_IN_PAGE);
    }

    // �I��������
    TbBool  IsSelected() const {
        return m_IsSelected;
    }

    // �y�[�W�ԍ��ݒ�
    void SetPageIndex(TbSint32 index) {
        m_PageIndex = index;
    }

    // �y�[�W�ԍ��擾
    TbSint32 GetPageIndex() const {
        return m_PageIndex;
    }

public:

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& pos) TB_OVERRIDE;

private:
    TbBool          m_IsSelected;
    TbSint32		m_Index;
    TbSint32        m_PageIndex;
    CmnCursorSprite	m_CursorSprite;
};

class MainMenuPlayerSelectPage : public CmnSlidePage
{
public:

    // �R���X�g���N�^
    MainMenuPlayerSelectPage();

    // �f�X�g���N�^
    virtual ~MainMenuPlayerSelectPage() {}

public:

    // �v���C���[�l���擾
    TbSint32 GetPlayerCount() const {
        return m_PlayerCount;
    }

public:

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& pos) TB_OVERRIDE;

private:

    enum
    {
		ARROW_DEFAULT , 
		ARROW_SELECT ,
		ARROW_MAX
    };

private:

    TbSint32		m_PlayerCount;
	TbSint32		m_ArrowSelect;
	CmnSimpleSprite	m_ArrowSprite[ARROW_MAX];

};

class MainMenuOptionPage : public CmnSlidePage
{
public:

    // �R���X�g���N�^
    MainMenuOptionPage();

    // �f�X�g���N�^
    virtual ~MainMenuOptionPage() {}

public:

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // �y�[�W�o�^���ꂽ
    void OnChangeInsert(TbBool isInserted) TB_OVERRIDE;

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& pos) TB_OVERRIDE;

private:

    enum
    {
        MENU_,
        MENU_INPUT_LAYOUT,
        MENU_MAX
    };

private:

    UiOptionWindow m_OptionWindow;

};


class MainMenuSlide : public CmnSlide
{
	typedef CmnSlide Super;
public:

	// �R���X�g���N�^s
    MainMenuSlide();

	// �f�X�g���N�^
	virtual ~MainMenuSlide();

public:

	// �I�����[�h�擾
    TbSint32 GetMode() {
		return m_ModeSelectPage[m_CurrentModePageIndex].GetMode();
    }

	// �v���C�l���擾
    TbSint32 GetPlayerCount() {
		return m_PlayerSelectPage.GetPlayerCount();
    }

protected:

    // ���̃y�[�W�擾
    virtual CmnSlidePage* GetNextPage() TB_OVERRIDE;

private:

    static const TbSint32 PAGE_MAX = 4;
    static const TbUint32 MODE_PAGE_MAX = MainMenuModeSelectPage::MODE_PAGE_MAX;

private:
    TbSint32                    m_CurrentModePageIndex;
    MainMenuModeSelectPage		m_ModeSelectPage[MODE_PAGE_MAX];
    MainMenuPlayerSelectPage	m_PlayerSelectPage;
    MainMenuOptionPage          m_OptionPage;
    
};

#endif