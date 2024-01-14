/*!
 * �������p����
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_UNION_INPUT_H_
#define _INCLUDED_SYS_UNION_INPUT_H_

#include <crayon/input/cr_input.h>
#include <base/container/tb_array.h>

class SysMajorityInput
{
public:
    
    // �R���X�g���N�^
    SysMajorityInput();
    
    // �f�X�g���N�^
    ~SysMajorityInput();
    
public:
    
    // ������
    void Initialize( TbUint32 count , TbFloat32 rate );
    
    // �X�V
    void Update( TbFloat32 deltatime );

public:

    // �{�^���������Ă��邩
    TbBool IsOn( CrInput::Button button  ) const;

    // �{�^������������
    TbBool IsPress( CrInput::Button button ) const;

    // �{�^����b������
    TbBool IsRelease( CrInput::Button button ) const;

    // ���݂̓��͔䗦�擾
    TbFloat32 GetCurrentInputRate( CrInput::Button button ) const {
        return m_CurrentRate[button];
    }

private:
    
    // ���͍X�V
    void update( CrInput::Button button , TbFloat32 deltatime);


private:

    struct InputParam
    {
        InputParam() { TbFill(time,0.0f); }
        TbFloat32 time[CrInput::BUTTON_MAX]; // ���͂��Ă���̎���
    };

private:

    TbBitArray32        m_Now;                                  //���t���[�����͏��
    TbBitArray32        m_Old;                                  //�O�t���[�����͏��
    TbArray<InputParam> m_InputParam;                           // ���͏��
    TbFloat32           m_InputRate;                            // ���͐�������
    TbFloat32           m_CurrentRate[CrInput::BUTTON_MAX];    // ���͂��Ă���䗦
    TbUint32            m_InputCount;                           // ���͎�t��

};

#endif
