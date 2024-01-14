/*!
 * �o�g���X�e�[�W�I��p�y�[�W
 * @author teco
 */

#ifndef _INCLUDED_BATTLE_STAGE_SELECT_SLIDE_H_
#define _INCLUDED_BATTLE_STAGE_SELECT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"
#include "ui/ui_message_window.h"

class BattleStageSelectPage : public CmnSlidePage
{
public:
    static const TbUint32 STAGE_MAX_IN_GROUP = 6;
    typedef BattleStageSelectPage Self;
public:

    // �R���X�g���N�^
    BattleStageSelectPage();

    // �f�X�g���N�^
    virtual ~BattleStageSelectPage() {}

public:

    // ������
    void Initialize(TbSint32 begin , TbBool hasPrev, TbBool hasNext);

    // �m�F�E�B������
    void SetConfirmWindow(UiMessageWindow* confirmWindow) {
        m_ConfirmWindow = confirmWindow;
    }

public:

    // �I�����[���h�擾
    TbSint32 GetStageIndex() const {
        return getStageIndex(m_FocusPoint.x, m_FocusPoint.y) + m_StageRange.begin;
    }

    // �I��������
    TbBool IsSelected() const {
        return  m_IsSelected;
    }

public:

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive( TbBool isActive ) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& offset) TB_OVERRIDE;

private:

    struct Group
    {
        CmnSimpleSprite sprite[STAGE_MAX_IN_GROUP];
    };

private:

    // �J�[�\���ړ�
    TbBool moveCursor( TbSint32 x , TbSint32 y );

    // �ʒu����X�e�[�W�C���f�b�N�X�擾
    TbSint32 getStageIndex( TbSint32 x, TbSint32 y ) const;

    // �X�e�[�W�C���f�b�N�X������W�擾
    TbPointS32 getPoint( TbSint32 stageIndex ) const;

private:
    TbRangeS32                      m_StageRange;
    TbBool                          m_HasPrev;
    TbBool                          m_HasNext;
    TbUint32                        m_StageWidth;
    TbUint32                        m_StageHeight;
    TbBool                          m_IsSelected;
    TbBool                          m_IsConfirming;
    Group                           m_Group;
    CmnCursorSprite                 m_CursorSprite;
    TbPointS32                      m_FocusPoint;
    UiMessageWindow*                m_ConfirmWindow;
};

class BattleStageSelectSlide : public CmnSlide
{
	typedef CmnSlide Super;
    typedef BattleStageSelectSlide Self;
public:

	// �R���X�g���N�^s
    BattleStageSelectSlide();

	// �f�X�g���N�^
	virtual ~BattleStageSelectSlide();

public:

    // �X�e�[�WID�擾
    TbUint32 GetStageId() const;

protected:

    // �������
    virtual void OnUpdate();

    // �`��
    virtual void OnDraw();

    // �y�[�W�ύX
    virtual void OnChangingPage(TbBool isNext) TB_OVERRIDE;

    // ���̃y�[�W�擾
    virtual CmnSlidePage* GetNextPage() TB_OVERRIDE;

private:

    // �m�F�E�B���h�E�̃R�[���o�b�N
    void onDecidedBackConfirm( UiMessageWindow::Result result );

private:
    TbUint32                                m_StageIndex;
    TbStaticArray<BattleStageSelectPage,10>	m_BattleStageSelectPage;
    TbUint32                                m_BackConfirmSleep;
    UiMessageWindow                         m_BackConfirmWindow;
    UiMessageWindow                         m_ConfirmWindow;
};

#endif