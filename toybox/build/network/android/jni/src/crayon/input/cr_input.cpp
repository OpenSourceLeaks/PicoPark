/*!
 * 仮想パッド( パッドやキーボード入力、複数デバイスを意識せずに操作するためのクラス )
 * tb_todo(全体見直すべし)
 * @author teco
 * @since 2011.09.14
 */

#include "tb_fwd.h"
#include <crayon/cr_def.h>
#include <crayon/input/cr_input.h>
#include "input/tb_pad.h"
#include "input/tb_keyboard.h"
#include <framework/debug/menu/tb_debug_menu_all.h>
#include <crayon/input/cr_remote_input_server.h>

using namespace toybox;

namespace crayon
{

namespace 
{
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
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(BUTTON_PAD_BIT)==CrInput::BUTTON_MAX);

    static const TbKeyType KEYBOARD_TABLE[CR_PLAYER_MAX][CrInput::BUTTON_MAX]
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

};

CrInput::Impl* CrInput::s_Impl = nullptr;

/*!
 * 初期化
 * @author teco
 */
void CrInput::Initialize()
{
    if (!s_Impl) {
        s_Impl = new Impl();
    }
}

/*!
 * 後始末
 * @author teco
 */
void CrInput::Finalize()
{
    TB_SAFE_DELETE(s_Impl);
}

/*!
 * コンストラクタ
 * @author teco
 */
CrInput::Impl::Impl()
    : m_ModeBit(CrInput::MODE_BIT_DEFAULT)
    , m_EnablePad(TB_TRUE)
    , m_KeyboardBit(1)
    , m_InputLayout(CR_INPUT_LAYOUT_A_DECIDE)
{
    TbCopyArray(BUTTON_PAD_BIT,m_ButtonPadBit);
    TbCopyArray2(KEYBOARD_TABLE,m_KeyboardTable);
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PadIndexTable); ++i) {
        m_PadIndexTable[i] = i;
    }
}

/*!
 * ですトラクタ
 * @author teco
 */
CrInput::Impl::~Impl()
{
}

/*!
 * プレイヤーのパッド番号
 * @author teco
 * @since 2014.06.27
 */
void CrInput::Impl::SetPlayerPadIndex( TbUint32 player , TbUint32 index )
{
    if( 0 <= player && player < TB_ARRAY_COUNT_OF(m_PadIndexTable) ){
        TbUint32 old = m_PadIndexTable[player];
        if ( old != index ){
            for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PadIndexTable); ++i ){
                if( m_PadIndexTable[i] == index ){
                    m_PadIndexTable[i] = old;
                    break;
                }
            }
            m_PadIndexTable[player] = index;
        }
    }
}

/*!
 * プレイヤーのパッド番号取得
 * @author teco
 * @since 2014.10.02
 */
TbUint32 CrInput::Impl::GetPlayerPadIndex( TbUint32 player )
{
    if( 0 <= player && player < TB_ARRAY_COUNT_OF(m_PadIndexTable) ){
        return m_PadIndexTable[player];
    }
    return 0;
}

/*!
 * キーボードが何Pの入力を受け付けるか
 * @author teco
 */
void CrInput::Impl::SetKeyboardIndexBit( TbUint32 bit )
{
    m_KeyboardBit = bit;
}

/*!
 * キーボードが何Pの入力を受け付けるか
 * @author teco
 */
TbUint32 CrInput::Impl::GetKeyboardIndexBit()
{
    // デバッグメニュー表示中は通常のキーボード操作をつけつけない
    if( TbDebugMenuManager::GetInstance()->IsVisible() ){
        return 0;
    }
    return m_KeyboardBit;
}

/*!
* パッド有効
* @author teco
*/
void CrInput::Impl::SetEnablePad(TbBool isEnable)
{
    m_EnablePad = isEnable;
}

/*!
 * 入力レイアウト設定
 * @author teco
 */
void CrInput::Impl::SetInputLayout(CrInputLayout layout)
{
    m_InputLayout = layout;
    switch (m_InputLayout)
    {
    case CR_INPUT_LAYOUT_A_DECIDE:
        {
            m_ButtonPadBit[BUTTON_DECIDE] = TB_PAD_A_BIT;
            m_ButtonPadBit[BUTTON_JUMP]   = TB_PAD_A_BIT;
            m_ButtonPadBit[BUTTON_CANCEL] = TB_PAD_B_BIT;
        }
        break;
    case CR_INPUT_LAYOUT_B_DECIDE:
        {
            m_ButtonPadBit[BUTTON_DECIDE] = TB_PAD_B_BIT;
            m_ButtonPadBit[BUTTON_JUMP]   = TB_PAD_B_BIT;
            m_ButtonPadBit[BUTTON_CANCEL] = TB_PAD_A_BIT;
        }
        break;
    }
}

/*! 
 * 指定モードビットすべて立っている
 * @author teco
 */
TbBool CrInput::Impl::IsOnAllModeBit( TbUint32 modeBit )
{
    return ( m_ModeBit & modeBit ) == m_ModeBit;
}

/*!
 * 指定フラグビットすべて立っている
 * @author teco
 */
TbBool CrInput::Impl::IsOnAnyModeBit( TbUint32 modeBit )
{
    return ( m_ModeBit & modeBit ) != 0;
}

/*!
 * 指定フラグビットすべて立っている
 * @author teco
 */
void CrInput::Impl::SetModeBit( TbUint32 modeBit )
{
    m_ModeBit = modeBit;
}

/*!
 * 何か誰か押しているか
 * @author teco
 */
TbBool CrInput::Impl::IsOnAny( Mode mode )
{
    for( TbSint32 i = 0; i < BUTTON_MAX; ++i ) {
        if( IsAnyoneOn( static_cast<CrInput::Button>(i) , 0xFFFFFFFF , mode , nullptr ) ){
            return TB_TRUE;
        }
    }
    return TB_FALSE;    
}

/*!
 * ボタンを押しているか
 * @author teco
 */
TbBool CrInput::Impl::IsOn( Button button , TbUint32 playerIndex , Mode mode )
{
    if (TB_ARRAY_COUNT_OF(m_PadIndexTable) <= playerIndex)
    {
        return TB_FALSE;
    }
    if (m_ModeBit & TB_BIT(mode)) {
        TbSint32 padIndex = m_PadIndexTable[playerIndex];
        if (m_EnablePad && TbPad::IsOn(m_ButtonPadBit[button], padIndex)) {
            return TB_TRUE;
        }
#if CR_REMOTE_INPUT_IS_ENABLE
        CrRemoteInputServer* remote = CrRemoteInputServer::GetInstancePtr();
        if (remote && remote->IsOn(m_ButtonPadBit[button], playerIndex)) {
            return TB_TRUE;
        }
#endif
        if (GetKeyboardIndexBit()&TB_BIT(padIndex)) {
            if (TbKeyboard::IsOn(m_KeyboardTable[0][button])) {
                return TB_TRUE;
            }
        }
        if (0 < padIndex && padIndex < CR_PLAYER_MAX) {
            if (TbKeyboard::IsOn(m_KeyboardTable[padIndex][button])) {
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
TbBool CrInput::Impl::IsOnRepeat( Button button , TbUint32 playerIndex , Mode mode )
{
    if (TB_ARRAY_COUNT_OF(m_PadIndexTable) <= playerIndex)
    {
        return TB_FALSE;
    }
    if (m_ModeBit & TB_BIT(mode)) {
        TbSint32 padIndex = m_PadIndexTable[playerIndex];
        if (m_EnablePad && TbPad::IsOnRepeat(m_ButtonPadBit[button], padIndex)) {
            return TB_TRUE;
        }
#if CR_REMOTE_INPUT_IS_ENABLE
        CrRemoteInputServer* remote = CrRemoteInputServer::GetInstancePtr();
        if (remote && remote->IsOnRepeat(m_ButtonPadBit[button], playerIndex)) {
            return TB_TRUE;
        }
#endif
        if (GetKeyboardIndexBit()&TB_BIT(padIndex)) {
            if (TbKeyboard::IsOnRepeat(m_KeyboardTable[0][button])) {
                return TB_TRUE;
            }
        }
        if (0 < padIndex && padIndex < CR_PLAYER_MAX) {
            if (TbKeyboard::IsOnRepeat(m_KeyboardTable[padIndex][button])) {
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
TbBool CrInput::Impl::IsPress( Button button , TbUint32 playerIndex , Mode mode )
{
    if (TB_ARRAY_COUNT_OF(m_PadIndexTable) <= playerIndex)
    {
        return TB_FALSE;
    }
    if (m_ModeBit & TB_BIT(mode)) {
        TbSint32 padIndex = m_PadIndexTable[playerIndex];
        TbUint32 padBit = m_ButtonPadBit[button];
        if (m_EnablePad && TbPad::IsPress(padBit, padIndex)) {
            return TB_TRUE;
        }
#if CR_REMOTE_INPUT_IS_ENABLE
        CrRemoteInputServer* remote = CrRemoteInputServer::GetInstancePtr();
        if (remote && remote->IsPress(padBit, playerIndex)) {
            return TB_TRUE;
        }
#endif
        if (GetKeyboardIndexBit()&TB_BIT(padIndex)) {
            if (TbKeyboard::IsPress(m_KeyboardTable[0][button])) {
                return TB_TRUE;
            }
        }
        if (0 < padIndex && padIndex < CR_PLAYER_MAX) {
            if (TbKeyboard::IsPress(m_KeyboardTable[padIndex][button])) {
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
TbBool CrInput::Impl::IsRelease( Button button , TbUint32 playerIndex , Mode mode )
{
    if (TB_ARRAY_COUNT_OF(m_PadIndexTable) <= playerIndex)
    {
        return TB_FALSE;
    }
    if (m_ModeBit & TB_BIT(mode)) {
        TbSint32 padIndex = m_PadIndexTable[playerIndex];
        if (m_EnablePad && TbPad::IsRelease(m_ButtonPadBit[button], padIndex)) {
            return TB_TRUE;
        }
#if CR_REMOTE_INPUT_IS_ENABLE
        CrRemoteInputServer* remote = CrRemoteInputServer::GetInstancePtr();
        if (remote && remote->IsRelease(m_ButtonPadBit[button], playerIndex)) {
            return TB_TRUE;
        }
#endif
        if (GetKeyboardIndexBit()&TB_BIT(padIndex)) {
            if (TbKeyboard::IsRelease(m_KeyboardTable[0][button])) {
                return TB_TRUE;
            }
        }
        if (0 < padIndex && padIndex < CR_PLAYER_MAX) {
            if (TbKeyboard::IsRelease(m_KeyboardTable[padIndex][button])) {
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
TbBool CrInput::Impl::IsAnyoneOn( Button button , TbUint32 playerBit , Mode mode, TbUint32* playerIndex)
{
    if( m_ModeBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i <= CR_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsOn(button,i,mode) ){
                    if (playerIndex)
                    {
                        *playerIndex = i;
                    }
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
TbBool CrInput::Impl::IsAnyoneOnRepeat( Button button , TbUint32 playerBit , Mode mode, TbUint32* playerIndex)
{
    if( m_ModeBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i <= CR_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsOnRepeat(button,i,mode) ){
                    if (playerIndex)
                    {
                        *playerIndex = i;
                    }
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
TbBool CrInput::Impl::IsAnyonePress( Button button , TbUint32 playerBit , Mode mode , TbUint32* playerIndex)
{
    if( m_ModeBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i <= CR_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsPress(button,i,mode) ){
                    if (playerIndex)
                    {
                        *playerIndex = i;
                    }
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
TbBool CrInput::Impl::IsAnyoneRelease( Button button , TbUint32 playerBit , Mode mode, TbUint32* playerIndex)
{
    if( m_ModeBit & TB_BIT(mode) ){
        for( TbUint32 i = 0; i <= CR_PLAYER_MAX; ++i ){
            if( TB_BIT(i) & playerBit ) {
                if( IsRelease(button,i,mode) ){
                    if (playerIndex)
                    {
                        *playerIndex = i;
                    }
                    return TB_TRUE;
                }
            }
        }
    }
    return TB_FALSE;
}

/*!
* ボタンとキーボード
* @author teco
*/
void CrInput::Impl::BindButtonKeyboardKey(TbUint32 padIndex, Button button, TbKeyType key)
{
    if (padIndex < CR_PLAYER_MAX && 0 <= button && button < BUTTON_MAX) {
        m_KeyboardTable[padIndex][button] = key;
    }
}

/*!
* キーボードが何Pの入力を受け付けるか
* @author teco
*/
TbKeyType CrInput::Impl::GetButtonKeyboardKey(TbUint32 padIndex, Button button)
{
    if (padIndex < CR_PLAYER_MAX && 0 <= button && button < BUTTON_MAX) {
        return  m_KeyboardTable[padIndex][button];
    }
    return TB_KEY_INVALID;
}


/*!
 * Bool値取
 * @author teco
 */
TbBool CrInput::Impl::GetBool( CrInputId id , Mode mode )
{
    if( !( m_ModeBit & TB_BIT(mode) ) ){
        return TB_FALSE;
    }
    TbBool result = TB_FALSE;
    TbBool isDebugMenuVisible = TbDebugMenuManager::GetInstance()->IsVisible();
     
    switch( id ){
    case CR_INPUT_ID_BOOL_FINISH:
        result = TbKeyboard::IsOn( TB_KEY_ESCAPE );
        break;
    case CR_INPUT_ID_BOOL_CAPTURE: // 画面キャプチャ
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_P );
//        result |= TbPad::IsPress( TB_PAD_R1_BIT );
        break;
    case CR_INPUT_ID_BOOL_RETURN_TITLE:
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_T );
        break;
    case CR_INPUT_ID_BOOL_RETURN_STAGE_SELECT:
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_Q );
        break;
    case CR_INPUT_ID_BOOL_SYSTEM_PAUSE:
        result = TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_SPACE );
        break;
    case CR_INPUT_ID_BOOL_RETRY:
        result = !isDebugMenuVisible && TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_R );
        break;
    case CR_INPUT_ID_BOOL_NEXT:
        result = !isDebugMenuVisible && TbKeyboard::IsPress( TB_KEY_RETURN );
        break;
    case CR_INPUT_ID_BOOL_FORCE_NEXT:
        result = !isDebugMenuVisible && TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_N );
        break;
    case CR_INPUT_ID_BOOL_CANCEL_KEYBOARD:
        result = !isDebugMenuVisible && TbKeyboard::IsPress( m_KeyboardTable[BUTTON_CANCEL][0] );
        break;
    case CR_INPUT_ID_BOOL_BREAK:
        result = !isDebugMenuVisible && TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_B );
        break;
    }
    return result;
}

}