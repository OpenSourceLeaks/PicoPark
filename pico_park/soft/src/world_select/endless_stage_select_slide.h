/*!
 * �o�g���X�e�[�W�I��p�y�[�W
 * @author teco
 */

#ifndef _INCLUDED_ENDLESS_STAGE_SELECT_SLIDE_H_
#define _INCLUDED_ENDLESS_STAGE_SELECT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"
#include "ui/ui_message_window.h"

class EndlessStageSelectPage : public CmnSlidePage
{
public:
    static const TbUint32 STAGE_MAX_IN_GROUP = 6;
    typedef EndlessStageSelectPage Self;
public:

    // �R���X�g���N�^
    EndlessStageSelectPage();

    // �f�X�g���N�^
    virtual ~EndlessStageSelectPage() {}

public:

    // ������
    void Initialize(TbSint32 begin , TbBool hasPrev, TbBool hasNext);

public:

    // �X�e�[�W���Ԑݒ�
    TbBool SetStageIndex( TbUint32 stageIndex );

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
    Group                           m_Group;
    CmnCursorSprite                 m_CursorSprite;
    TbPointS32                      m_FocusPoint;
};

class EndlessStageSelectSlide : public CmnSlide
{
	typedef CmnSlide Super;
    typedef EndlessStageSelectSlide Self;
public:

	// �R���X�g���N�^s
    EndlessStageSelectSlide();

	// �f�X�g���N�^
	virtual ~EndlessStageSelectSlide();

public:

    // ���A
    void Resume( TbUint32 stageId );

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
    TbUint32                                    m_StageIndex;
    TbStaticArray<EndlessStageSelectPage,10>	m_EndlessStageSelectPage;
};

#endif