/*!
 * �ҏW���[�h
 * @author teco
 */

#ifndef _INCLUDED_EIDIT_MODE_BASE_H_
#define _INCLUDED_EIDIT_MODE_BASE_H_

#include <base/math/tb_vector2.h>
#include "edit/edit_types.h"
#include "edit/edit_util.h"
#include "side_action/stage/side_action_stage.h"
#include <input/tb_mouse.h>

class EditModeBase
{
public:

    // �R���X�g���N�^
    EditModeBase( const char* name )
        : m_Name(name)
        , m_Stage(nullptr)
    {}

    // �f�X�g���N�^
    virtual ~EditModeBase() {}

public:

    // ���O�擾
    const char* GetName() const {
        return m_Name.GetBuffer();
    }

    // �X�e�[�W�ݒ�
    void SetStage(SideActionStage* stage) {
        m_Stage = stage;
    }

    // �X�e�[�W�擾
    SideActionStage* GetStage() {
        return m_Stage;
    }

public:

    // ������
    virtual void Initialize() {}

public:

    // �������
    virtual void OnUpdate(TbFloat32 deltasec) {}

    // �A�N�e�B�u��Ԃ��ω�����
    virtual void OnChangeActive( TbBool isActive ) {}

    // �`�揈��
    virtual void OnDraw(TbPointS32* pos) {}

private:

    TbStaticString32    m_Name;
    SideActionStage*    m_Stage;

};

#endif