/*!
 * ���z�p�b�h( �p�b�h��L�[�{�[�h���́A�����f�o�C�X���ӎ������ɑ��삷�邽�߂̃N���X )
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_INPUT_H_
#define _INCLUDED_SYS_INPUT_H_

#include <input/tb_keyboard.h>

enum SysInputId
{
    SYS_INPUT_ID_BOOL_FINISH ,           // �I��
    SYS_INPUT_ID_BOOL_RELOAD ,           // �����[�h
    SYS_INPUT_ID_BOOL_RETURN_TITLE ,     // �����^�C�g��
    SYS_INPUT_ID_BOOL_RETURN_STAGE_SELECT , // �����^�C�g��
    SYS_INPUT_ID_BOOL_SYSTEM_PAUSE ,            // �|�[�Y
    SYS_INPUT_ID_BOOL_RETRY ,            // ���g���C
    SYS_INPUT_ID_BOOL_NEXT  ,            // ���V�[�P���X
    SYS_INPUT_ID_BOOL_FORCE_NEXT  ,      // ���V�[�P���X
    SYS_INPUT_ID_BOOL_BREAK ,   // �L�[�{�[�h�ŕ���
};

class SysInput
{
public:
    enum Button
    {
        BUTTON_DECIDE , // ����
        BUTTON_ATTACK = BUTTON_DECIDE , // �U��
        BUTTON_CANCEL , // �L�����Z��
        BUTTON_SPECIAL = BUTTON_CANCEL, // �L�����Z��
        BUTTON_JUMP , // �W�����v
        BUTTON_UP     , // ��
        BUTTON_DOWN   , // ��
        BUTTON_LEFT   , // ��
        BUTTON_RIGHT  , // �E
        BUTTON_ROTATE_R , // �E��]
        BUTTON_ROTATE_L , // ����]
        BUTTON_START  , // �X�^�[�g
        BUTTON_MAX    , 
    };
    enum ButtonBit
    {
        BUTTON_BIT_DECIDE   = TB_BIT(BUTTON_DECIDE), // ����
        BUTTON_BIT_ATTACK   = TB_BIT(BUTTON_ATTACK) , // �U��
        BUTTON_BIT_CANCEL   = TB_BIT(BUTTON_CANCEL) , // �L�����Z��
        BUTTON_BIT_JUMP     = TB_BIT(BUTTON_JUMP), // �W�����v
        BUTTON_BIT_UP       = TB_BIT(BUTTON_UP) , // ��
        BUTTON_BIT_DOWN     = TB_BIT(BUTTON_DOWN) , // ��
        BUTTON_BIT_LEFT     = TB_BIT(BUTTON_LEFT) , // ��
        BUTTON_BIT_RIGHT    = TB_BIT(BUTTON_RIGHT) , // �E
        BUTTON_BIT_ROTATE_R = TB_BIT(BUTTON_ROTATE_R) , // �E��]
        BUTTON_BIT_ROTATE_L = TB_BIT(BUTTON_ROTATE_L) , // ����]
    };
    enum Mode{
        MODE_RETAIL , // �ʏ�
        MODE_DEBUG ,  // �f�o�b�O
        MODE_MAX
    };
    enum FlagBit{
        FLAG_BIT_RETAIL = TB_BIT(MODE_RETAIL) , // �ʏ�
        FLAG_BIT_DEBUG = TB_BIT(MODE_DEBUG) ,  // �f�o�b�O
    };
public:

    // �v���C���[�̃p�b�h�ԍ�
    static void SetPlayerPadIndex( TbUint32 player , TbUint32 index );

    // �v���C���[�̃p�b�h�ԍ��擾
    static TbUint32 GetPlayerPadIndex( TbUint32 player );

    // �{�^���ƃL�[�{�[�h
    static void BindButtonKeyboardKey( TbUint32 padIndex , Button button , TbKeyType key );

    // �L�[�{�[�h����P�̓��͂��󂯕t���邩
    static TbKeyType GetButtonKeyboardKey( TbUint32 padIndex , Button button );

    // �p�b�h�L��
    static void SetEnablePad( TbBool isEnable );

    // �w��t���O�r�b�g���ׂė����Ă���
    static TbBool IsOnAllFlagBit( TbUint32 flagBit );

    // �w��t���O�r�b�g���ׂė����Ă���
    static TbBool IsOnAnyFlagBit( TbUint32 flagBit );

    // �w��t���O�r�b�g���ׂė����Ă���
    static void SetFlagBit( TbUint32 flagBit );

    // �����N�������Ă��邩
    static TbBool IsOnAny( Mode mode = MODE_RETAIL );

    // �{�^���������Ă��邩
    static TbBool IsOn( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // �{�^�������s�[�g�����Ă��邩
    static TbBool IsOnRepeat( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // �{�^������������
    static TbBool IsPress( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // �{�^����b������
    static TbBool IsRelease( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // �{�^���������Ă��邩
    static TbBool IsAnyoneOn( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

    // �{�^�������s�[�g�����Ă��邩
    static TbBool IsAnyoneOnRepeat( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

    // �{�^������������
    static TbBool IsAnyonePress( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

    // �{�^����b������
    static TbBool IsAnyoneRelease( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

public:

    // �f�o�b�O�p�L�[�{�[�h����P�̓��͂��󂯕t���邩
    static void SetMainPlayerKeyboardPadIndexBit( TbUint32 bit );

    // �f�o�b�O�p�L�[�{�[�h����P�̓��͂��󂯕t���邩
    static TbUint32 GetMainPlayerKeyboardPadIndexBit();

public:

    // Bool�l�擾
    static TbBool GetBool( SysInputId id , Mode mode = MODE_RETAIL );

private:
    SysInput();
    ~SysInput();
};

#endif
