/*!
 * ���C�����j���[�p�y�[�W
 * @author teco
 */

#ifndef _INCLUDED_WORLD_SELECT_SLIDE_H_
#define _INCLUDED_WORLD_SELECT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"

class WorldSelectPage : public CmnSlidePage
{
public:
    typedef WorldSelectPage Self;
    typedef CmnSlidePage Super;
public:
    static const TbUint32 WORLD_MAX_IN_GROUP = 6;
    static const TbUint32 WORLD_WIDTH = 3;
    static const TbUint32 WORLD_HEIGHT = WORLD_MAX_IN_GROUP/ WORLD_WIDTH;
public:

    // �R���X�g���N�^
    WorldSelectPage();

    // �f�X�g���N�^
    virtual ~WorldSelectPage();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // ������
    void Initialize(TbSint32 begin , TbBool hasPrev, TbBool hasNext);

public:

    // ���[���h�S������[�h
    void SetOpenAllWorldMode();

    // ���[���h�ݒ�
    void SetWorld(TbSint32 world) {
        TB_RESQUE(m_WorldRange.begin<=world&&world<m_WorldRange.end);
        TbSint32 worldIndex = world-m_WorldRange.begin;
        m_FocusPoint = getPoint(worldIndex);
    }

    // ���[���h�ݒ�
    void SetWorld(TbSint32 x, TbSint32 y) {
        TbSint32 world = getWorldIndex(x, y) + m_WorldRange.begin;
        if(m_WorldRange.begin <= world&&world<m_WorldRange.end){
            SetWorld(world);
        }
    }

    // �I�����[���h�擾
    TbSint32 GetWorld() const {
        return getWorldIndex(m_FocusPoint.x, m_FocusPoint.y) + m_WorldRange.begin;
    }

    // �I��������
    TbBool IsSelected() const {
        return  m_IsSelected;
    }

    // �t�H�[�J�X�ʒu�擾
    TbPointS32 GetFocusPoint() const {
        return m_FocusPoint;
    }

    // �I���\��
    TbBool IsReadySelect() const {
        return GetPhase() < PHASE_OPEN_WORLD_CLEAR;
    }

    // �����X�V
    void UpdateCrown();

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
        CmnSimpleSprite sprite[WORLD_MAX_IN_GROUP];
        CmnSimpleSprite crownSprie[WORLD_MAX_IN_GROUP];
    };

    enum
    {
        PHASE_DEFAULT = 0 ,
        PHASE_OPEN_WORLD_CLEAR = 10
    };

private:

    // �J�[�\���ړ�
    TbBool moveCursor( TbSint32 x , TbSint32 y );

    // �ʒu���烏�[���h�C���f�b�N�X�擾
    TbSint32 getWorldIndex( TbSint32 x, TbSint32 y ) const;

    // ���[���h�C���f�b�N�X������W�擾
    TbPointS32 getPoint( TbSint32 worldIndex ) const;

    // ���[���h���p�\��
    TbBool   isEnableWorld( TbSint32 x, TbSint32 y ) const;

private:
    TbRangeS32                      m_WorldRange;
    TbBool                          m_HasPrev;
    TbBool                          m_HasNext;
    TbBool                          m_IsSelected;
    Group                           m_Group;
    CmnCursorSprite                 m_CursorSprite;
    TbPointS32                      m_FocusPoint;
};

class WorldStageSelectPage : public CmnSlidePage
{
public:

    // �R���X�g���N�^
    WorldStageSelectPage();

    // �f�X�g���N�^
    virtual ~WorldStageSelectPage();

public:

    // ���[���h�ݒ�
    void SetWorld( TbSint32 world );

    // �X�e�[�W�ݒ�
    void SetStage(TbUint32 stage) {
        if( stage < getStageCount() ){
            m_SelectedStage = stage;
        }
    }

    // �X�e�[�W�擾
    TbUint32 GetSelectedStageIndex() const {
        return m_SelectedStage;
    }

    // �X�e�[�W�擾
    TbUint32 GetSelectedStageId() const;

    // �X�e�[�W�Z���N�g��������
    TbBool IsReadySelectStage() const {
        return PHASE_SELECT_STAGE <= GetPhase();
    }

    // �S���[���h�I�[�v��
    TbBool IsOpenAllWorld() const {
        return PHASE_OPEN_ALL_WORLD == GetPhase();
    }

public:

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& offset) TB_OVERRIDE;

private:

    enum
    {
        PHASE_CHECK_CLEAR,
        PHASE_NEW_STAGE = 10,
        PHASE_WORLD_CLEAR = 20,
        PHASE_OPEN_ALL_WORLD = 25,
        PHASE_SELECT_STAGE = 30,
    };

    struct Stage
    {
        Stage()
            : ratio(0.0f)
        {
        }
        TbBool IsActive() const {
            return TB_FLOAT32_EQUAL(ratio,1.0f);
        }
        CmnSimpleSprite sprite;
        CmnWhiteSprite  line;
        CmnSimpleSprite crownSprie;
        TbFloat32       ratio;
    };

private:

    // �X�e�[�W���擾
    TbSint32 getStageCount() const;

private:

    TbUint32		    m_SelectedStage;
    TbSint32            m_World;
    TbSint32            m_LatestStage;
    TbFloat32           m_WorldClearPosX;
    CmnCursorSprite     m_CursorSprite;
    Stage*              m_Stage;

};

class WorldSelectSlide : public CmnSlide
{
	typedef CmnSlide Super;
public:

	// �R���X�g���N�^s
    WorldSelectSlide();

	// �f�X�g���N�^
	virtual ~WorldSelectSlide();

public:

    // �S���[���h������[�h�ݒ�
    void SetOpenAllWorldMode();

    // ���A
    void Resume( TbSint32 stageId , TbBool isNextStage);

    // ���[���h�ݒ�
    void SetWorld(TbSint32 world) {
        m_World = world;
        TbUint32 worldSelectIndex = getWorldSelectPage(world);
        m_WorldSelectPage[worldSelectIndex].SetWorld(world);
        m_WorldStageSelectPage.SetWorld(world);
    }

        // ���[���h�擾
    TbUint32 GetWorld() const {
        return m_World;
    }

    // �X�e�[�W�ݒ�
    void SetStageIndex(TbSint32 stageIndex) {
        m_WorldStageSelectPage.SetStage(stageIndex);
    }

    // �X�e�[�W�擾
    TbUint32 GetStageIndex() const {
        return m_WorldStageSelectPage.GetSelectedStageIndex();
    }

    // �X�e�[�WID�擾
    TbUint32 GetStageId() const {
        return m_WorldStageSelectPage.GetSelectedStageId();
    }

    // BGM�Đ��\��Ԃ�
    TbBool IsReadyBgm() const {
        return m_IsReadyBgm;
    }

    // �S���[���h�I�[�v����
    TbBool IsOpenAllWorld() const {
        return m_WorldStageSelectPage.IsOpenAllWorld();
    }

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

    // �Y������y�[�W���擾
    TbUint32 getWorldSelectPage(TbUint32 world) {
        return world/ WorldSelectPage::WORLD_MAX_IN_GROUP;
    }

private:
    TbUint32                            m_World;
    TbBool                              m_IsReadyBgm;
    TbStaticArray<WorldSelectPage,10>	m_WorldSelectPage;
    WorldStageSelectPage	            m_WorldStageSelectPage;
};

#endif