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

class MainMenuModeSelectPage : public CmnSlidePage
{
public:

    // �R���X�g���N�^
    MainMenuModeSelectPage();

    // �f�X�g���N�^
    virtual ~MainMenuModeSelectPage() {}

public:

    // �I���C���f�b�N�X�擾
    TbSint32 GetSelectedIndex() const {
        return m_Index;
    }

public:

    // �������
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �`�揈��
    void Draw(const TbVector2& pos) TB_OVERRIDE;

private:

	static const TbSint32 MODE_MAX = 2;

private:
    TbSint32		m_Index;
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
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �`�揈��
    void Draw(const TbVector2& pos) TB_OVERRIDE;

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
		return m_ModeSelectPage.GetSelectedIndex();
    }

	// �v���C�l���擾
    TbSint32 GetPlayerCount() {
		return m_PlayerSelectPage.GetPlayerCount();
    }

private:

    enum
    {
		PAGE_MODE_SELECT ,
		PAGE_PLAYER_SELECT ,
		PAGE_MAX
    };

private:
    MainMenuModeSelectPage		m_ModeSelectPage;
    MainMenuPlayerSelectPage	m_PlayerSelectPage;
};

#endif