/*!
 * ���z�p�b�h( �p�b�h��L�[�{�[�h���́A�����f�o�C�X���ӎ������ɑ��삷�邽�߂̃N���X )
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_CR_INPUT_H_
#define _INCLUDED_CR_INPUT_H_

#include <crayon/cr_def.h>
#include <input/tb_keyboard.h>

namespace crayon
{

enum CrInputId
{
    CR_INPUT_ID_BOOL_CAPTURE ,          // ��ʃL���v�`���[�{�^��
    CR_INPUT_ID_BOOL_FINISH ,           // �I��
    CR_INPUT_ID_BOOL_RELOAD ,           // �����[�h
    CR_INPUT_ID_BOOL_RETURN_TITLE ,     // �����^�C�g��
    CR_INPUT_ID_BOOL_RETURN_STAGE_SELECT , // �����^�C�g��
    CR_INPUT_ID_BOOL_SYSTEM_PAUSE ,            // �|�[�Y
    CR_INPUT_ID_BOOL_RETRY ,            // ���g���C
    CR_INPUT_ID_BOOL_NEXT  ,            // ���V�[�P���X
    CR_INPUT_ID_BOOL_FORCE_NEXT  ,      // ���V�[�P���X
    CR_INPUT_ID_BOOL_CANCEL_KEYBOARD ,   // �L�[�{�[�h�ŕ���
    CR_INPUT_ID_BOOL_BREAK ,   // �L�[�{�[�h�ŕ���
};

enum CrInputLayout
{
    CR_INPUT_LAYOUT_A_DECIDE , // A�Ō���
    CR_INPUT_LAYOUT_B_DECIDE , // B�Ō���
    CR_INPUT_LAYOUT_MAX
};

#define CR_INPUT_PLAYER_ALL_BIT (0xFFFF)

class CrInput
{
public:
    enum Button
    {
        BUTTON_DECIDE , // ����
        BUTTON_ATTACK = BUTTON_DECIDE , // �U��
        BUTTON_CANCEL , // �L�����Z��
        BUTTON_SPECIAL = BUTTON_CANCEL ,
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
        MODE_BASE , // �ʏ�
        MODE_DEBUG   , // �f�o�b�O
        MODE_USER1   , // ���[�U�[��`1
        MODE_USER2   , // ���[�U�[��`2
        MODE_USER3   , // ���[�U�[��`3
        MODE_MAX
    };
    enum ModeBit{
        MODE_BIT_BASE = TB_BIT(MODE_BASE) , // �ʏ�
        MODE_BIT_DEBUG  = TB_BIT(MODE_DEBUG) ,  // �f�o�b�O
        MODE_BIT_USER1 = TB_BIT(MODE_USER1),  // �f�o�b�O
        MODE_BIT_USER2 = TB_BIT(MODE_USER2),  // �f�o�b�O
        MODE_BIT_USER3 = TB_BIT(MODE_USER3),  // �f�o�b�O
    };
    static const TbUint32 MODE_BIT_DEFAULT = MODE_BIT_BASE | MODE_BIT_DEBUG;

public:

    // �v���C���[�̃p�b�h�ԍ�
    static void SetPlayerPadIndex(TbUint32 player, TbUint32 index) {
        if( s_Impl ) {
            s_Impl->SetPlayerPadIndex(player, index);
        }
    }

    // �v���C���[�̃p�b�h�ԍ��擾
    static TbUint32 GetPlayerPadIndex(TbUint32 player) {
        return s_Impl ? s_Impl->GetPlayerPadIndex(player) : 0;
    }

    // �L�[�{�[�h����P�̓��͂��󂯕t���邩
    static void SetKeyboardIndexBit(TbUint32 bit) {
        if (s_Impl) {
            s_Impl->SetKeyboardIndexBit(bit);
        }
    }

    // �L�[�{�[�h����P�̓��͂��󂯕t���邩
    static TbUint32 GetKeyboardIndexBit() {
        return s_Impl ? s_Impl->GetKeyboardIndexBit() : 0;
    }

    // �{�^���ƃL�[�{�[�h
    static void BindButtonKeyboardKey(TbUint32 padIndex, Button button, toybox::TbKeyType key) {
        if (s_Impl) {
            s_Impl->BindButtonKeyboardKey(padIndex,button, key);
        }
    }

    // �L�[�{�[�h����P�̓��͂��󂯕t���邩
    static toybox::TbKeyType GetButtonKeyboardKey(TbUint32 padIndex, Button button) {
        return s_Impl ? s_Impl->GetButtonKeyboardKey(padIndex, button) : toybox::TB_KEY_INVALID;
    }

    // �p�b�h�L��
    static void SetEnablePad(TbBool isEnable) {
        if (s_Impl) {
            s_Impl->SetEnablePad(isEnable);
        }
    }

    // ���̓��C�A�E�g�ݒ�
    static void SetInputLayout(CrInputLayout layout) {
        if (s_Impl) {
            s_Impl->SetInputLayout(layout);
        }
    }

    // �w�胂�[�h�r�b�g���ׂė����Ă���
    static TbBool IsOnAllModeBit(TbUint32 flagBit) {
        return s_Impl ? s_Impl->IsOnAllModeBit(flagBit) : TB_FALSE;
    }

    // �w�胂�[�h�r�b�g���ׂė����Ă���
    static TbBool IsOnAnyModeBit(TbUint32 flagBit) {
        return s_Impl ? s_Impl->IsOnAnyModeBit(flagBit) : TB_FALSE;
    }

    // �w�胂�[�h�r�b�g���ׂė����Ă���
    static void SetModeBit(TbUint32 flagBit) {
        if (s_Impl) {
            s_Impl->SetModeBit(flagBit);
        }
    }

    // �����N�������Ă��邩
    static TbBool IsOnAny(Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsOnAny(mode) : TB_FALSE;
    }

    // �{�^���������Ă��邩
    static TbBool IsOn(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsOn(button, playerIndex, mode) : TB_FALSE;
    }

    // �{�^�������s�[�g�����Ă��邩
    static TbBool IsOnRepeat(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsOnRepeat(button, playerIndex, mode) : TB_FALSE;
    }

    // �{�^������������
    static TbBool IsPress(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsPress(button, playerIndex, mode) : TB_FALSE;
    }

    // �{�^����b������
    static TbBool IsRelease(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsRelease(button, playerIndex, mode) : TB_FALSE;
    }

    // �{�^���������Ă��邩
    static TbBool IsAnyoneOn(Button button, TbUint32 playerBit, Mode mode = MODE_BASE, TbUint32* playerIndex = nullptr) {
        return s_Impl ? s_Impl->IsAnyoneOn(button, playerBit, mode, playerIndex) : TB_FALSE;
    }

    // �{�^�������s�[�g�����Ă��邩
    static TbBool IsAnyoneOnRepeat(Button button, TbUint32 playerBit, Mode mode = MODE_BASE, TbUint32* playerIndex = nullptr) {
        return s_Impl ? s_Impl->IsAnyoneOnRepeat(button, playerBit, mode, playerIndex) : TB_FALSE;
    }

    // �{�^������������
    static TbBool IsAnyonePress(Button button, TbUint32 playerBit, Mode mode = MODE_BASE , TbUint32* playerIndex = nullptr ) {
        return s_Impl ? s_Impl->IsAnyonePress(button, playerBit, mode, playerIndex) : TB_FALSE;
    }

    // �{�^����b������
    static TbBool IsAnyoneRelease(Button button, TbUint32 playerBit, Mode mode = MODE_BASE, TbUint32* playerIndex = nullptr) {
        return s_Impl ? s_Impl->IsAnyoneRelease(button,playerBit,mode, playerIndex) : TB_FALSE;
    }

public:

    // Bool�l�擾GetBool
    static TbBool GetBool(CrInputId id, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->GetBool(id,mode) : TB_FALSE;
    }

public:

    // ������
    static void Initialize();

    // ��n��
    static void Finalize();

private:

    class Impl
    {
    public:
        Impl();
        ~Impl();
    public:
        // �v���C���[�̃p�b�h�ԍ�
        void SetPlayerPadIndex(TbUint32 player, TbUint32 index);
        // �v���C���[�̃p�b�h�ԍ��擾
        TbUint32 GetPlayerPadIndex(TbUint32 player);
        // �L�[�{�[�h����P�̓��͂��󂯕t���邩
        void SetKeyboardIndexBit(TbUint32 bit);
        // �L�[�{�[�h����P�̓��͂��󂯕t���邩
        TbUint32 GetKeyboardIndexBit();
        // �{�^���ƃL�[�{�[�h
        void BindButtonKeyboardKey(TbUint32 padIndex, Button button, toybox::TbKeyType key);
        // �L�[�{�[�h����P�̓��͂��󂯕t���邩
        toybox::TbKeyType GetButtonKeyboardKey(TbUint32 padIndex, Button button);
        // �p�b�h�L��
        void SetEnablePad(TbBool isEnable);
        // ���̓��C�A�E�g�ݒ�
        void SetInputLayout(CrInputLayout layout);
        // �w�胂�[�h�r�b�g���ׂė����Ă���
        TbBool IsOnAllModeBit(TbUint32 flagBit);
        // �w�胂�[�h�r�b�g���ׂė����Ă���
        TbBool IsOnAnyModeBit(TbUint32 flagBit);
        // �w�胂�[�h�r�b�g���ׂė����Ă���
        void SetModeBit(TbUint32 flagBit);
        // �����N�������Ă��邩
        TbBool IsOnAny(Mode mode);
        // �{�^���������Ă��邩
        TbBool IsOn(Button button, TbUint32 playerIndex, Mode mode);
        // �{�^�������s�[�g�����Ă��邩
        TbBool IsOnRepeat(Button button, TbUint32 playerIndex, Mode mode);
        // �{�^������������
        TbBool IsPress(Button button, TbUint32 playerIndex, Mode mode);
        // �{�^����b������
        TbBool IsRelease(Button button, TbUint32 playerIndex, Mode mode);
        // �{�^���������Ă��邩
        TbBool IsAnyoneOn(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // �{�^�������s�[�g�����Ă��邩
        TbBool IsAnyoneOnRepeat(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // �{�^������������
        TbBool IsAnyonePress(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // �{�^����b������
        TbBool IsAnyoneRelease(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // Bool�l�擾GetBool
        TbBool GetBool(CrInputId id, Mode mode);
    private:
        TbUint32            m_ModeBit;
        TbBool              m_EnablePad;
        TbUint32            m_KeyboardBit;
        TbUint32            m_InputLayout;
        TbUint32            m_ButtonPadBit[BUTTON_MAX];
        toybox::TbKeyType   m_KeyboardTable[CR_PLAYER_MAX][BUTTON_MAX];
        TbUint32            m_PadIndexTable[16];
    };

private:
    CrInput();
    ~CrInput();

private:
    static Impl* s_Impl;
};

}

#endif
