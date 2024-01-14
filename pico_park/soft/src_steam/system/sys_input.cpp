/*!
 * 仮想パッド( パッドやキーボード入力、複数デバイスを意識せずに操作するためのクラス )
 * tb_todo(全体見直すべし)
 * @author teco
 * @since 2011.09.14
 */

#include "fwd.h"
#include "system/sys_input.h"
#include "input/tb_pad.h"
#include "input/tb_keyboard.h"
#include <framework/debug/menu/tb_debug_menu_all.h>

#if NET_INPUT_ENABLE
#include "network/net_input_server.h"
#endif

using namespace toybox;

namespace 
{

    TbUint32 s_FlagBit = SysInput::FLAG_BIT_RETAIL | SysInput::FLAG_BIT_DEBUG;
    TbBool   s_EnablePad = TB_TRUE;
    TbUint32 s_MainPlayerKeyboardPadBit = TB_BIT(0);

    static const TbUint32 BUTTON_PAD_BIT[] = 
    {
        TB_PAD_A_BIT ,
        TB_PAD_B_BIT ,
        TB_PAD_A_BIT ,
        TB_PAD_UP_BIT , 
        TB_PAD_DOWN_BIT ,
        TB_PAD_LEFT_BIT ,
        TB_PAD_RIGHT_BIT ,
        TB_PAD_A_BIT ,
        TB_PAD_B_BIT ,
        TB_PAD_START_BIT ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(BUTTON_PAD_BIT)==SysInput::BUTTON_MAX);

    static TbKeyType s_KeyboardTable[CMN_PLAYER_MAX][SysInput::BUTTON_MAX]
    {
        // 1P
        {
            TB_KEY_SPACE , // BUTTON_DECIDE
            TB_KEY_BACK ,   // BUTTON_CANCEL
            TB_KEY_SPACE ,     // BUTTON_JUMP
            TB_KEY_UP ,     // BUTTON_UP
            TB_KEY_DOWN ,   // BUTTON_DOWN
            TB_KEY_LEFT ,   // BUTTON_LEFT
            TB_KEY_RIGHT ,  // BUTTON_RIGHT
            TB_KEY_SPACE ,      // BUTTON_ROTATE_R
            TB_KEY_INVALID ,// BUTTON_ROTATE_L
            TB_KEY_RETURN ,  // BUTTON_START
        },
        // 2P
        {
            TB_KEY_W ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_W ,  // BUTTON_JUMP
            TB_KEY_W ,  // BUTTON_UP
            TB_KEY_S ,  // BUTTON_DOWN
            TB_KEY_A ,  // BUTTON_LEFT
            TB_KEY_D ,  // BUTTON_RIGHT
            TB_KEY_W ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 3P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 4P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 5P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 6P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 7P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 8P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 8P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },
        // 10P
        {
            TB_KEY_INVALID ,  // BUTTON_DECIDE
            TB_KEY_INVALID ,  // BUTTON_CANCEL
            TB_KEY_INVALID ,  // BUTTON_JUMP
            TB_KEY_INVALID ,  // BUTTON_UP
            TB_KEY_INVALID ,  // BUTTON_DOWN
            TB_KEY_INVALID ,  // BUTTON_LEFT
            TB_KEY_INVALID ,  // BUTTON_RIGHT
            TB_KEY_INVALID ,  // BUTTON_ROTATE_R
            TB_KEY_INVALID ,  // BUTTON_ROTATE_L
            TB_KEY_INVALID ,  // BUTTON_START
        },    
    };

    // パッドのプレイヤーとパッド番号の関連付け
    static TbUint32 s_PadIndexTable[] = 
    {
        0 , 
        1 ,
        2 ,
        3 ,
        4 ,
        5 ,
        6 ,
        7 ,
        8 ,
        9 , 
        10 , 
        11 , 
        12 ,
        13 , 
        14 , 
        15 , 
    };

};

/*!
 * プレイヤーのパッド番号
 * @author teco
 * @since 2014.06.27
 */
void SysInput::SetPlayerPadIndex( TbUint32 player , TbUint32 index )
{
    if( 0 <= player && player < TB_ARRAY_COUNT_OF(s_PadIndexTable) ){
        TbUint32 old = s_PadIndexTable[player];
        if ( old != index ){
            for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(s_PadIndexTable); ++i ){
                if( s_PadIndexTable[i] == index ){
                    s_PadIndexTable[i] = old;
                    break;
                }
            }
            s_PadIndexTable[player] = index;
        }
    }
}

/*!
 * プレイヤーのパッド番号取得
 * @author teco
 * @since 2014.10.02
 */
TbUint32 SysInput::GetPlayerPadIndex( TbUint32 player )
{
    if( 0 <= player && player < TB_ARRAY_COUNT_OF(s_PadIndexTable) ){
        return s_PadIndexTable[player];
    }
    return 0;
}

/*!
 * キーボードが何Pの入力を受け付けるか
 * @author teco
 */
void SysInput::SetMainPlayerKeyboardPadIndexBit( TbUint32 bit )
{
    s_MainPlayerKeyboardPadBit = bit;
}

/*!
 * キーボードが何Pの入力を受け付けるか
 * @author teco
 */
TbUint32 SysInput::GetMainPlayerKeyboardPadIndexBit()
{
    // デバッグメニュー表示中は通常のキーボード操作をつけつけない
    if( TbDebugMenuManager::GetInstance()->IsVisible() ){
        return 0;
    }
    return s_MainPlayerKeyboardPadBit;
}

/*!
 * ボタンとキーボード
 * @author teco
 */
void SysInput::BindButtonKeyboardKey( TbUint32 padIndex , Button button , TbKeyType key )
{
    if( padIndex < CMN_PLAYER_MAX && 0 <= button && button < BUTTON_MAX ){
        s_KeyboardTable[padIndex][button] = key;
    }
}

/*!
 * キーボードが何Pの入力を受け付けるか
 * @author teco
 */
TbKeyType SysInput::GetButtonKeyboardKey( TbUint32 padIndex , Button button )
{
    if( padIndex < CMN_PLAYER_MAX && 0 <= button && button < BUTTON_MAX ){
       return  s_KeyboardTable[padIndex][button];
    }
    return TB_KEY_INVALID;
}

/*! 
 * パッド有効
 * @author teco
 */
void SysInput::SetEnablePad( TbBool isEnable )
{
    s_EnablePad = isEnable;
}

/*! 
 * 指定フラグビットすべて立っている
 * @author teco
 */
TbBool SysInput::IsOnAllFlagBit( TbUint32 flagBit )
{
    return ( s_FlagBit & flagBit ) == s_FlagBit;
}

/*!
 * 指定フラグビットすべて立っている
 * @author teco
 */
TbBool SysInput::IsOnAnyFlagBit( TbUint32 flagBit )
{
    return ( s_FlagBit & flagBit ) != 0;
}

/*!
 * 指定フラグビットすべて立っている
 * @author teco
 */
void SysInput::SetFlagBit( TbUint32 flagBit )
{
    s_FlagBit = flagBit;
}

/*!
 * 何か誰か押しているか
 * @author teco
 */
TbBool SysInput::IsOnAny( Mode mode )
{
    for( TbSint32 i = 0; i < BUTTON_MAX; ++i ) {
        if( IsAnyoneOn( static_cast<SysInput::Button>(i) , 0xFFFFFFFF , mode ) ){
            return TB_TRUE;
        }
    }
    return TB_FALSE;    
}

/*!
 * ボタンを押しているか
 * @author teco
 */
TbBool SysInput::IsOn( Button button , TbUint32 playerIndex , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        TbSint32 padIndex = s_PadIndexTable[playerIndex];
        if( s_EnablePad && TbPad::IsOn( BUTTON_PAD_BIT[button] , padIndex ) ){
            return TB_TRUE;
        }
#if NET_INPUT_ENABLE
        if( NetInputServer::GetInstance().IsOn( BUTTON_PAD_BIT[button] , padIndex ) ) {
            return TB_TRUE;
        }
#endif
        if( GetMainPlayerKeyboardPadIndexBit()&TB_BIT(padIndex) ){
            if( TbKeyboard::IsOn(s_KeyboardTable[0][button]) ){
                return TB_TRUE;
            }    
        }
        if( 0 < padIndex && padIndex < CMN_PLAYER_MAX ) {
            if( TbKeyboard::IsOn(s_KeyboardTable[padIndex][button]) ){
                return TB_TRUE;
            }    
        }
    }
    return TB_FALSE;
}

/*!
 * ボタンを押しているか(リピート)
 * @author teco
 */
TbBool SysInput::IsOnRepeat( Button button , TbUint32 playerIndex , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        TbSint32 padIndex = s_PadIndexTable[playerIndex];
        if( s_EnablePad && TbPad::IsOnRepeat( BUTTON_PAD_BIT[button] , padIndex ) ){
            return TB_TRUE;
        }
#if NET_INPUT_ENABLE
        if( NetInputServer::GetInstance().IsOnRepeat( BUTTON_PAD_BIT[button] , padIndex ) ) {
            return TB_TRUE;
        }
#endif
        if( GetMainPlayerKeyboardPadIndexBit()&TB_BIT(padIndex) ){
            if( TbKeyboard::IsOnRepeat(s_KeyboardTable[0][button]) ){
                return TB_TRUE;
            }    
        }
        if( 0 < padIndex && padIndex < CMN_PLAYER_MAX ) {
            if( TbKeyboard::IsOnRepeat(s_KeyboardTable[padIndex][button]) ){
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

/*!
 * ボタンを押したか
 * @author teco
 */
TbBool SysInput::IsPress( Button button , TbUint32 playerIndex , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        TbSint32 padIndex = s_PadIndexTable[playerIndex];
        TbUint32 padBit = BUTTON_PAD_BIT[button];
        if( s_EnablePad && TbPad::IsPress( padBit , padIndex ) ){
            return TB_TRUE;
        }
#if NET_INPUT_ENABLE
        if( NetInputServer::GetInstance().IsPress( padBit , padIndex ) ){
            return TB_TRUE;
        }
#endif
        if( GetMainPlayerKeyboardPadIndexBit()&TB_BIT(padIndex) ){
            if( TbKeyboard::IsPress(s_KeyboardTable[0][button]) ){
                return TB_TRUE;
            }    
        }
        if( 0 < padIndex && padIndex < CMN_PLAYER_MAX ) {
            if( TbKeyboard::IsPress(s_KeyboardTable[padIndex][button]) ){
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

/*!
 * ボタンを話したか
 * @author teco
 */
TbBool SysInput::IsRelease( Button button , TbUint32 playerIndex , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        TbSint32 padIndex = s_PadIndexTable[playerIndex];
        if( s_EnablePad && TbPad::IsRelease( BUTTON_PAD_BIT[button] , padIndex ) ){
            return TB_TRUE;
        }
#if NET_INPUT_ENABLE
        if(  NetInputServer::GetInstance().IsRelease( BUTTON_PAD_BIT[button] , padIndex ) ){
            return TB_TRUE;
        }
#endif
        if( GetMainPlayerKeyboardPadIndexBit()&TB_BIT(padIndex) ){
            if( TbKeyboard::IsRelease(s_KeyboardTable[0][button]) ){
                return TB_TRUE;
            }
        }
        if( 0 < padIndex && padIndex < CMN_PLAYER_MAX ) {
            if( TbKeyboard::IsRelease(s_KeyboardTable[padIndex][button]) ){
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

/*!
 * 誰かボタンを押しているか
 * @author teco
 */
TbBool SysInput::IsAnyoneOn( Button button , TbUint32 playerBit , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i < CMN_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsOn(button,i,mode) ){
                    return TB_TRUE;
                }
            }
        }
    }
    return TB_FALSE;
}

/*!
 * 誰かボタンを押しているか(リピート)
 * @author teco
 */
TbBool SysInput::IsAnyoneOnRepeat( Button button , TbUint32 playerBit , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i < CMN_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsOnRepeat(button,i,mode) ){
                    return TB_TRUE;
                }
            }
        }
    }
    return TB_FALSE;
}

/*!
 * 誰かボタンを押したか
 * @author teco
 */
TbBool SysInput::IsAnyonePress( Button button , TbUint32 playerBit , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i < CMN_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsPress(button,i,mode) ){
                    return TB_TRUE;
                }
            }
        }
    }
    return TB_FALSE;
}

/*!
 * 誰かボタンを離したか
 * @author teco
 */
TbBool SysInput::IsAnyoneRelease( Button button , TbUint32 playerBit , Mode mode )
{
    if( s_FlagBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i < CMN_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsRelease(button,i,mode) ){
                    return TB_TRUE;
                }
            }
        }
    }
    return TB_FALSE;
}

/*!
 * Bool値取
 * @author teco
 */
TbBool SysInput::GetBool( SysInputId id , Mode mode )
{
    if( !( s_FlagBit & TB_BIT(mode) ) ){
        return TB_FALSE;
    }
    TbBool result = TB_FALSE;
    TbBool isDebugMenuVisible = TbDebugMenuManager::GetInstance()->IsVisible();
     
    switch( id ){
    case SYS_INPUT_ID_BOOL_FINISH:
        result = TbKeyboard::IsOn( TB_KEY_ESCAPE );
        break;
#if TB_IS_DEBUG
    case SYS_INPUT_ID_BOOL_RETURN_TITLE:
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_T );
        break;
    case SYS_INPUT_ID_BOOL_RETURN_STAGE_SELECT:
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_E );
        break;
    case SYS_INPUT_ID_BOOL_SYSTEM_PAUSE:
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_SPACE );
        break;
    case SYS_INPUT_ID_BOOL_RETRY:
        result = !isDebugMenuVisible && TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_R );
        break;
#endif
    case SYS_INPUT_ID_BOOL_NEXT:
        result = !isDebugMenuVisible && TbKeyboard::IsPress( TB_KEY_RETURN );
        break;
    case SYS_INPUT_ID_BOOL_FORCE_NEXT:
#if TB_IS_DEBUG
        result = !isDebugMenuVisible && TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_N );
#endif
        break;
    case SYS_INPUT_ID_BOOL_BREAK:
#if TB_IS_DEBUG
        result = !isDebugMenuVisible && TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_B );
#endif
        break;
    }
    return result;
}
