/*!
 * ����p����
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_GIANT_INPUT_H_
#define _INCLUDED_SYS_GIANT_INPUT_H_

#include "sys_input.h"
#include <input/tb_pad.h>
#include <framework/debug/menu/tb_debug_menu_all.h>

class SysGiantInput : public TbSingletonRef<SysGiantInput>
{
    typedef SysGiantInput Self;
public:

    // �R���X�g���N�^
    SysGiantInput();

    // �f�X�g���N�^
    ~SysGiantInput();

public:
    
    // ������
    void Initialize();
    
    // �������
    void Update();

    // �{�^���������Ă��邩
    TbBool IsOn( SysInput::Button button , TbUint32 playerIndex );

    // �{�^���������Ă��邩
    TbBool IsOnRepeat( SysInput::Button button , TbUint32 playerIndex );

    // �{�^������������
    TbBool IsPress( SysInput::Button button , TbUint32 playerIndex);

    // �{�^����b������
    TbBool IsRelease( SysInput::Button button , TbUint32 playerIndex);

public:

    // �f�o�b�O���j���[�Z�b�g�A�b�v
    void SetupDebugMenu( TbDebugMenu* debugMenu );

private:

    TbSint32 getIndex( SysInput::Button button );

    // ���̓Z�b�g�A�b�v
    void     onClickedSetupInput();

    // �ݒ菑���o��
    void writeGiantSetting();

private:

    TbPadInfo   m_PadInfo;
    TbBool      m_DebugPhase;

};

#endif
