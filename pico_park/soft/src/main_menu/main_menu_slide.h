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
#include "common/ui/ui_player_select_window.h"
#include "ui/ui_message_window.h"

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
    static const TbUint32 MODE_WIDTH = 2;
    static const TbUint32 MODE_HEIGHT = MODE_MAX/MODE_WIDTH;

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

    // ���Z�b�g
    void Reset();

    // �I���C���f�b�N�X�擾
    TbSint32 GetMode() const {
        return m_Index;
    }

public:

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& pos) TB_OVERRIDE;

private:

    enum State
    {
        STATE_MENU,
        STATE_PLAYER_SELECT = 10,
        STATE_OPTION = 20,
        STATE_CANCEL = 30,
    };

private:

    TbSint32                m_State;
    TbBool                  m_IsSelected;
    TbSint32		        m_Index;
    CmnCursorSprite	        m_CursorSprite;
    UiOptionWindow          m_OptionWindow;
    UiMessageWindow         m_ConfirmWindow;

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
		return m_ModeSelectPage.GetMode();
    }

    // ���X�^�[�g
    void Restart();

protected:

    // ���̃y�[�W�擾
    virtual CmnSlidePage* GetNextPage() TB_OVERRIDE;

private:

    static const TbSint32 PAGE_MAX = 4;

private:

    MainMenuModeSelectPage	m_ModeSelectPage;

};

#endif