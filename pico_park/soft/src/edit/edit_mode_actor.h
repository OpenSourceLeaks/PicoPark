/*!
* �ҏW���[�h
* @author teco
*/

#ifndef _INCLUDED_EDIT_MODE_ACTOR_H_
#define _INCLUDED_EDIT_MODE_ACTOR_H_

#include "edit_mode_base.h"

class EditModeActor : public EditModeBase
{
public:

    typedef EditModeActor Self;
    typedef EditModeBase Super;

public:

    // �R���X�g���N�^
    EditModeActor();

    // �f�X�g���N�^
    virtual ~EditModeActor();

public:

    // ������
    void Initialize() TB_OVERRIDE;

public:

    // �������
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(TbPointS32* pos) TB_OVERRIDE;

};

#endif