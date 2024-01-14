/*!
 * ����p����
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_GIANT_INPUT_H_
#define _INCLUDED_SYS_GIANT_INPUT_H_

#include "crayon/input/cr_input.h"
#include <input/tb_pad.h>
#include <framework/debug/menu/tb_debug_menu_all.h>

namespace crayon
{

class CrGiantInput : public toybox::TbSingletonRef<CrGiantInput>
{
    typedef CrGiantInput Self;
public:

    // �R���X�g���N�^
    CrGiantInput();

    // �f�X�g���N�^
    ~CrGiantInput();

public:
    
    // ������
    void Initialize();
    
    // �������
    void Update();

    // �{�^���������Ă��邩
    TbBool IsOn( CrInput::Button button , TbUint32 playerIndex );

    // �{�^���������Ă��邩
    TbBool IsOnRepeat( CrInput::Button button , TbUint32 playerIndex );

    // �{�^������������
    TbBool IsPress( CrInput::Button button , TbUint32 playerIndex);

    // �{�^����b������
    TbBool IsRelease( CrInput::Button button , TbUint32 playerIndex);

public:

    // �f�o�b�O���j���[�Z�b�g�A�b�v
    void SetupDebugMenu( toybox::TbDebugMenu* debugMenu );

private:

    TbSint32 getIndex( CrInput::Button button );

    // ���̓Z�b�g�A�b�v
    void     onClickedSetupInput();

    // �ݒ菑���o��
    void writeGiantSetting();

private:

    toybox::TbPadInfo   m_PadInfo;
    TbBool      m_DebugPhase;

};

}

#endif
