/*!
 * 仮想パッド( パッドやキーボード入力、複数デバイスを意識せずに操作するためのクラス )
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
    CR_INPUT_ID_BOOL_CAPTURE ,          // 画面キャプチャーボタン
    CR_INPUT_ID_BOOL_FINISH ,           // 終了
    CR_INPUT_ID_BOOL_RELOAD ,           // リロード
    CR_INPUT_ID_BOOL_RETURN_TITLE ,     // 強制タイトル
    CR_INPUT_ID_BOOL_RETURN_STAGE_SELECT , // 強制タイトル
    CR_INPUT_ID_BOOL_SYSTEM_PAUSE ,            // ポーズ
    CR_INPUT_ID_BOOL_RETRY ,            // リトライ
    CR_INPUT_ID_BOOL_NEXT  ,            // 次シーケンス
    CR_INPUT_ID_BOOL_FORCE_NEXT  ,      // 次シーケンス
    CR_INPUT_ID_BOOL_CANCEL_KEYBOARD ,   // キーボードで閉じる
    CR_INPUT_ID_BOOL_BREAK ,   // キーボードで閉じる
};

enum CrInputLayout
{
    CR_INPUT_LAYOUT_A_DECIDE , // Aで決定
    CR_INPUT_LAYOUT_B_DECIDE , // Bで決定
    CR_INPUT_LAYOUT_MAX
};

#define CR_INPUT_PLAYER_ALL_BIT (0xFFFF)

class CrInput
{
public:
    enum Button
    {
        BUTTON_DECIDE , // 決定
        BUTTON_ATTACK = BUTTON_DECIDE , // 攻撃
        BUTTON_CANCEL , // キャンセル
        BUTTON_SPECIAL = BUTTON_CANCEL ,
        BUTTON_JUMP , // ジャンプ
        BUTTON_UP     , // 上
        BUTTON_DOWN   , // 下
        BUTTON_LEFT   , // 左
        BUTTON_RIGHT  , // 右
        BUTTON_ROTATE_R , // 右回転
        BUTTON_ROTATE_L , // 左回転
        BUTTON_START  , // スタート
        BUTTON_MAX    , 
    };
    enum ButtonBit
    {
        BUTTON_BIT_DECIDE   = TB_BIT(BUTTON_DECIDE), // 決定
        BUTTON_BIT_ATTACK   = TB_BIT(BUTTON_ATTACK) , // 攻撃
        BUTTON_BIT_CANCEL   = TB_BIT(BUTTON_CANCEL) , // キャンセル
        BUTTON_BIT_JUMP     = TB_BIT(BUTTON_JUMP), // ジャンプ
        BUTTON_BIT_UP       = TB_BIT(BUTTON_UP) , // 上
        BUTTON_BIT_DOWN     = TB_BIT(BUTTON_DOWN) , // 下
        BUTTON_BIT_LEFT     = TB_BIT(BUTTON_LEFT) , // 左
        BUTTON_BIT_RIGHT    = TB_BIT(BUTTON_RIGHT) , // 右
        BUTTON_BIT_ROTATE_R = TB_BIT(BUTTON_ROTATE_R) , // 右回転
        BUTTON_BIT_ROTATE_L = TB_BIT(BUTTON_ROTATE_L) , // 左回転
    };
    enum Mode{
        MODE_BASE , // 通常
        MODE_DEBUG   , // デバッグ
        MODE_USER1   , // ユーザー定義1
        MODE_USER2   , // ユーザー定義2
        MODE_USER3   , // ユーザー定義3
        MODE_MAX
    };
    enum ModeBit{
        MODE_BIT_BASE = TB_BIT(MODE_BASE) , // 通常
        MODE_BIT_DEBUG  = TB_BIT(MODE_DEBUG) ,  // デバッグ
        MODE_BIT_USER1 = TB_BIT(MODE_USER1),  // デバッグ
        MODE_BIT_USER2 = TB_BIT(MODE_USER2),  // デバッグ
        MODE_BIT_USER3 = TB_BIT(MODE_USER3),  // デバッグ
    };
    static const TbUint32 MODE_BIT_DEFAULT = MODE_BIT_BASE | MODE_BIT_DEBUG;

public:

    // プレイヤーのパッド番号
    static void SetPlayerPadIndex(TbUint32 player, TbUint32 index) {
        if( s_Impl ) {
            s_Impl->SetPlayerPadIndex(player, index);
        }
    }

    // プレイヤーのパッド番号取得
    static TbUint32 GetPlayerPadIndex(TbUint32 player) {
        return s_Impl ? s_Impl->GetPlayerPadIndex(player) : 0;
    }

    // キーボードが何Pの入力を受け付けるか
    static void SetKeyboardIndexBit(TbUint32 bit) {
        if (s_Impl) {
            s_Impl->SetKeyboardIndexBit(bit);
        }
    }

    // キーボードが何Pの入力を受け付けるか
    static TbUint32 GetKeyboardIndexBit() {
        return s_Impl ? s_Impl->GetKeyboardIndexBit() : 0;
    }

    // ボタンとキーボード
    static void BindButtonKeyboardKey(TbUint32 padIndex, Button button, toybox::TbKeyType key) {
        if (s_Impl) {
            s_Impl->BindButtonKeyboardKey(padIndex,button, key);
        }
    }

    // キーボードが何Pの入力を受け付けるか
    static toybox::TbKeyType GetButtonKeyboardKey(TbUint32 padIndex, Button button) {
        return s_Impl ? s_Impl->GetButtonKeyboardKey(padIndex, button) : toybox::TB_KEY_INVALID;
    }

    // パッド有効
    static void SetEnablePad(TbBool isEnable) {
        if (s_Impl) {
            s_Impl->SetEnablePad(isEnable);
        }
    }

    // 入力レイアウト設定
    static void SetInputLayout(CrInputLayout layout) {
        if (s_Impl) {
            s_Impl->SetInputLayout(layout);
        }
    }

    // 指定モードビットすべて立っている
    static TbBool IsOnAllModeBit(TbUint32 flagBit) {
        return s_Impl ? s_Impl->IsOnAllModeBit(flagBit) : TB_FALSE;
    }

    // 指定モードビットすべて立っている
    static TbBool IsOnAnyModeBit(TbUint32 flagBit) {
        return s_Impl ? s_Impl->IsOnAnyModeBit(flagBit) : TB_FALSE;
    }

    // 指定モードビットすべて立っている
    static void SetModeBit(TbUint32 flagBit) {
        if (s_Impl) {
            s_Impl->SetModeBit(flagBit);
        }
    }

    // 何か誰か押しているか
    static TbBool IsOnAny(Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsOnAny(mode) : TB_FALSE;
    }

    // ボタンを押しているか
    static TbBool IsOn(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsOn(button, playerIndex, mode) : TB_FALSE;
    }

    // ボタンをリピート押しているか
    static TbBool IsOnRepeat(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsOnRepeat(button, playerIndex, mode) : TB_FALSE;
    }

    // ボタンを押したか
    static TbBool IsPress(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsPress(button, playerIndex, mode) : TB_FALSE;
    }

    // ボタンを話したか
    static TbBool IsRelease(Button button, TbUint32 playerIndex, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->IsRelease(button, playerIndex, mode) : TB_FALSE;
    }

    // ボタンを押しているか
    static TbBool IsAnyoneOn(Button button, TbUint32 playerBit, Mode mode = MODE_BASE, TbUint32* playerIndex = nullptr) {
        return s_Impl ? s_Impl->IsAnyoneOn(button, playerBit, mode, playerIndex) : TB_FALSE;
    }

    // ボタンをリピート押しているか
    static TbBool IsAnyoneOnRepeat(Button button, TbUint32 playerBit, Mode mode = MODE_BASE, TbUint32* playerIndex = nullptr) {
        return s_Impl ? s_Impl->IsAnyoneOnRepeat(button, playerBit, mode, playerIndex) : TB_FALSE;
    }

    // ボタンを押したか
    static TbBool IsAnyonePress(Button button, TbUint32 playerBit, Mode mode = MODE_BASE , TbUint32* playerIndex = nullptr ) {
        return s_Impl ? s_Impl->IsAnyonePress(button, playerBit, mode, playerIndex) : TB_FALSE;
    }

    // ボタンを話したか
    static TbBool IsAnyoneRelease(Button button, TbUint32 playerBit, Mode mode = MODE_BASE, TbUint32* playerIndex = nullptr) {
        return s_Impl ? s_Impl->IsAnyoneRelease(button,playerBit,mode, playerIndex) : TB_FALSE;
    }

public:

    // Bool値取得GetBool
    static TbBool GetBool(CrInputId id, Mode mode = MODE_BASE) {
        return s_Impl ? s_Impl->GetBool(id,mode) : TB_FALSE;
    }

public:

    // 初期化
    static void Initialize();

    // 後始末
    static void Finalize();

private:

    class Impl
    {
    public:
        Impl();
        ~Impl();
    public:
        // プレイヤーのパッド番号
        void SetPlayerPadIndex(TbUint32 player, TbUint32 index);
        // プレイヤーのパッド番号取得
        TbUint32 GetPlayerPadIndex(TbUint32 player);
        // キーボードが何Pの入力を受け付けるか
        void SetKeyboardIndexBit(TbUint32 bit);
        // キーボードが何Pの入力を受け付けるか
        TbUint32 GetKeyboardIndexBit();
        // ボタンとキーボード
        void BindButtonKeyboardKey(TbUint32 padIndex, Button button, toybox::TbKeyType key);
        // キーボードが何Pの入力を受け付けるか
        toybox::TbKeyType GetButtonKeyboardKey(TbUint32 padIndex, Button button);
        // パッド有効
        void SetEnablePad(TbBool isEnable);
        // 入力レイアウト設定
        void SetInputLayout(CrInputLayout layout);
        // 指定モードビットすべて立っている
        TbBool IsOnAllModeBit(TbUint32 flagBit);
        // 指定モードビットすべて立っている
        TbBool IsOnAnyModeBit(TbUint32 flagBit);
        // 指定モードビットすべて立っている
        void SetModeBit(TbUint32 flagBit);
        // 何か誰か押しているか
        TbBool IsOnAny(Mode mode);
        // ボタンを押しているか
        TbBool IsOn(Button button, TbUint32 playerIndex, Mode mode);
        // ボタンをリピート押しているか
        TbBool IsOnRepeat(Button button, TbUint32 playerIndex, Mode mode);
        // ボタンを押したか
        TbBool IsPress(Button button, TbUint32 playerIndex, Mode mode);
        // ボタンを話したか
        TbBool IsRelease(Button button, TbUint32 playerIndex, Mode mode);
        // ボタンを押しているか
        TbBool IsAnyoneOn(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // ボタンをリピート押しているか
        TbBool IsAnyoneOnRepeat(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // ボタンを押したか
        TbBool IsAnyonePress(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // ボタンを話したか
        TbBool IsAnyoneRelease(Button button, TbUint32 playerBit, Mode mode, TbUint32* playerIndex);
        // Bool値取得GetBool
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
