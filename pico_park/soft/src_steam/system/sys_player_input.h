/*!
 * �v���C���[���͗p
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_PLAYER_INPUT_H_
#define _INCLUDED_SYS_PLAYER_INPUT_H_

#include "sys_input.h"
#include <input/tb_bit_input.h>

class SysPlayerCustomInput
{
public:

    // �������
    virtual void Update( float deltasec ) {}

    // �{�^���������Ă��邩
    virtual TbBool IsOn( SysInput::Button button , TbUint32 playerIndex ) const = 0;

    // �{�^���������Ă��邩
    virtual TbBool IsOnRepeat( SysInput::Button button , TbUint32 playerIndex ) const = 0;

    // �{�^������������
    virtual TbBool IsPress( SysInput::Button button , TbUint32 playerIndex) const = 0;

    // �{�^����b������
    virtual TbBool IsRelease( SysInput::Button button , TbUint32 playerIndex) const = 0;

};

class SysPlayerInput
{
public:

    enum Mode
    {
        MODE_DEFAULT , 
        MODE_GIANT , 
        MODE_HYBRID ,
    };

public:

    // �R���X�g���N�^
    SysPlayerInput();

    // �f�X�g���N�^
    ~SysPlayerInput();

public:
    
    // ���[�h�ݒ�
    void SetMode( Mode mode );

    // �|�[�Y�ݒ�
    void SetPause( TbBool isPause ) {
        m_IsPause = isPause;
    }

    // �|�[�Y�擾
    TbBool IsPause() const {
        return m_IsPause;
    }

    // �J�X�^���C���v�b�g�ݒ�
    void SetCustom( SysPlayerCustomInput* custom ) {
        m_Custom = custom;
    }

    // �{�^���������Ă��邩
    TbBool IsOn( SysInput::Button button , TbUint32 playerIndex ) const;

    // �{�^���������Ă��邩
    TbBool IsOnAny( TbUint32 playerIndex ) const;

    // �{�^���������Ă��邩
    TbBool IsOnRepeat( SysInput::Button button , TbUint32 playerIndex ) const;

    // �{�^������������
    TbBool IsPress( SysInput::Button button , TbUint32 playerIndex) const;

    // �{�^����b������
    TbBool IsRelease( SysInput::Button button , TbUint32 playerIndex) const;

public:

    // �������
    void Update( float deltasec );

private:

    Mode                    m_Mode;
    SysPlayerCustomInput*   m_Custom;
    TbBool                  m_IsPause;

};

#endif
