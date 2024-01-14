/*!
 * 仮想パッド( パッドやキーボード入力、複数デバイスを意識せずに操作するためのクラス )
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_INPUT_H_
#define _INCLUDED_SYS_INPUT_H_

#include <input/tb_keyboard.h>

enum SysInputId
{
    SYS_INPUT_ID_BOOL_FINISH ,           // 終了
    SYS_INPUT_ID_BOOL_RELOAD ,           // リロード
    SYS_INPUT_ID_BOOL_RETURN_TITLE ,     // 強制タイトル
    SYS_INPUT_ID_BOOL_RETURN_STAGE_SELECT , // 強制タイトル
    SYS_INPUT_ID_BOOL_SYSTEM_PAUSE ,            // ポーズ
    SYS_INPUT_ID_BOOL_RETRY ,            // リトライ
    SYS_INPUT_ID_BOOL_NEXT  ,            // 次シーケンス
    SYS_INPUT_ID_BOOL_FORCE_NEXT  ,      // 次シーケンス
    SYS_INPUT_ID_BOOL_BREAK ,   // キーボードで閉じる
};

class SysInput
{
public:
    enum Button
    {
        BUTTON_DECIDE , // 決定
        BUTTON_ATTACK = BUTTON_DECIDE , // 攻撃
        BUTTON_CANCEL , // キャンセル
        BUTTON_SPECIAL = BUTTON_CANCEL, // キャンセル
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
        MODE_RETAIL , // 通常
        MODE_DEBUG ,  // デバッグ
        MODE_MAX
    };
    enum FlagBit{
        FLAG_BIT_RETAIL = TB_BIT(MODE_RETAIL) , // 通常
        FLAG_BIT_DEBUG = TB_BIT(MODE_DEBUG) ,  // デバッグ
    };
public:

    // プレイヤーのパッド番号
    static void SetPlayerPadIndex( TbUint32 player , TbUint32 index );

    // プレイヤーのパッド番号取得
    static TbUint32 GetPlayerPadIndex( TbUint32 player );

    // ボタンとキーボード
    static void BindButtonKeyboardKey( TbUint32 padIndex , Button button , TbKeyType key );

    // キーボードが何Pの入力を受け付けるか
    static TbKeyType GetButtonKeyboardKey( TbUint32 padIndex , Button button );

    // パッド有効
    static void SetEnablePad( TbBool isEnable );

    // 指定フラグビットすべて立っている
    static TbBool IsOnAllFlagBit( TbUint32 flagBit );

    // 指定フラグビットすべて立っている
    static TbBool IsOnAnyFlagBit( TbUint32 flagBit );

    // 指定フラグビットすべて立っている
    static void SetFlagBit( TbUint32 flagBit );

    // 何か誰か押しているか
    static TbBool IsOnAny( Mode mode = MODE_RETAIL );

    // ボタンを押しているか
    static TbBool IsOn( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // ボタンをリピート押しているか
    static TbBool IsOnRepeat( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // ボタンを押したか
    static TbBool IsPress( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // ボタンを話したか
    static TbBool IsRelease( Button button , TbUint32 playerIndex , Mode mode = MODE_RETAIL );

    // ボタンを押しているか
    static TbBool IsAnyoneOn( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

    // ボタンをリピート押しているか
    static TbBool IsAnyoneOnRepeat( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

    // ボタンを押したか
    static TbBool IsAnyonePress( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

    // ボタンを話したか
    static TbBool IsAnyoneRelease( Button button , TbUint32 playerBit , Mode mode = MODE_RETAIL );

public:

    // デバッグ用キーボードが何Pの入力を受け付けるか
    static void SetMainPlayerKeyboardPadIndexBit( TbUint32 bit );

    // デバッグ用キーボードが何Pの入力を受け付けるか
    static TbUint32 GetMainPlayerKeyboardPadIndexBit();

public:

    // Bool値取得
    static TbBool GetBool( SysInputId id , Mode mode = MODE_RETAIL );

private:
    SysInput();
    ~SysInput();
};

#endif
