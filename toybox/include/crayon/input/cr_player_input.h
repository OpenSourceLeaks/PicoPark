/*!
 * �v���C���[���͗p
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_CR_PLAYER_INPUT_H_
#define _INCLUDED_CR_PLAYER_INPUT_H_

#include "crayon/input/cr_input.h"
#include <input/tb_bit_input.h>

namespace crayon
{

class CrPlayerCustomInput
{
public:

    // �������
    virtual void Update( float deltasec ) {}

    // �{�^���������Ă��邩
    virtual TbBool IsOn( CrInput::Button button , TbUint32 playerIndex ) const = 0;

    // �{�^���������Ă��邩
    virtual TbBool IsOnRepeat( CrInput::Button button , TbUint32 playerIndex ) const = 0;

    // �{�^������������
    virtual TbBool IsPress( CrInput::Button button , TbUint32 playerIndex) const = 0;

    // �{�^����b������
    virtual TbBool IsRelease( CrInput::Button button , TbUint32 playerIndex) const = 0;

};

class CrPlayerInput
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
    CrPlayerInput();

    // �f�X�g���N�^
    ~CrPlayerInput();

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
    void SetCustom( CrPlayerCustomInput* custom ) {
        m_Custom = custom;
    }

    // �{�^���������Ă��邩
    TbBool IsOn( CrInput::Button button , TbUint32 playerIndex ) const;

    // �{�^���������Ă��邩
    TbBool IsOnRepeat( CrInput::Button button , TbUint32 playerIndex ) const;

    // �{�^������������
    TbBool IsPress( CrInput::Button button , TbUint32 playerIndex) const;

    // �{�^����b������
    TbBool IsRelease( CrInput::Button button , TbUint32 playerIndex) const;

public:

    // �������
    void Update( float deltasec );

private:

    Mode                    m_Mode;
    CrPlayerCustomInput*    m_Custom;
    TbBool                  m_IsPause;

};

}

#endif
