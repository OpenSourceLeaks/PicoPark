/*!
 * @file
 * @brief パッド入力管理クラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "input/tb_keyboard.h"

namespace toybox
{

    // 変換テーブル
    static const TbChar8* CONVERT_TABLE[] = 
    {
        // アルファベット
        "A" , // TB_KEY_A,
        "B" , // TB_KEY_B,
        "C" , // TB_KEY_C,
        "D" , // TB_KEY_D,
        "E" , // TB_KEY_E,
        "F" , // TB_KEY_F,
        "G" , // TB_KEY_G,
        "H" , // TB_KEY_H,
        "I" , // TB_KEY_I,
        "J" , // TB_KEY_J,
        "K" , // TB_KEY_K,
        "L" , // TB_KEY_L,
        "M" , // TB_KEY_M,
        "N" , // TB_KEY_N,
        "O" , // TB_KEY_O,
        "P" , // TB_KEY_P,
        "Q" , // TB_KEY_Q,
        "R" , // TB_KEY_R,
        "S" , // TB_KEY_S,
        "T" , // TB_KEY_T,
        "U" , // TB_KEY_U,
        "V" , // TB_KEY_V,
        "W" , // TB_KEY_W,
        "X" , // TB_KEY_X,
        "Y" , // TB_KEY_Y,
        "Z" , // TB_KEY_Z,
        // 数字
        "1" , // TB_KEY_1,
        "2" , // TB_KEY_2,
        "3" , // TB_KEY_3,
        "4" , // TB_KEY_4,
        "5" , // TB_KEY_5,
        "6" , // TB_KEY_6,
        "7" , // TB_KEY_7,
        "8" , // TB_KEY_8,
        "9" , // TB_KEY_9,
        "0" , // TB_KEY_0,
        // 特殊文字
        "-" , // TB_KEY_MINUS,
        "=" , // TB_KEY_EQUAL,
        "SPACE" , // TB_KEY_SPACE,
        "ESCAPE" , // TB_KEY_ESCAPE ,
        "TAB" , // TB_KEY_TAB ,
        "BACK" , // TB_KEY_BACK ,
        "RETURN" , // TB_KEY_RETURN ,
        "F1" , // TB_KEY_F1 ,
        "F2" , // TB_KEY_F2 ,
        "F3" , // TB_KEY_F3 ,
        "F4" , // TB_KEY_F4 ,
        "F5" , // TB_KEY_F5 ,
        "F6" , // TB_KEY_F6 ,
        "F7" , // TB_KEY_F7 ,
        "F8" , // TB_KEY_F8 ,
        "F9" , // TB_KEY_F9 ,
        "F10" , // TB_KEY_F10 ,
        "F11" , // TB_KEY_F11 ,
        "F12" , // TB_KEY_F12 ,
        "F13", // TB_KEY_F13 ,
        "F14", // TB_KEY_F14 ,
        "F15" , // TB_KEY_F15 ,
        "PAGE UP" , // TB_KEY_PAGE_UP ,
        "PAGE DOWN", // TB_KEY_PAGE_DOWN ,
        "L CONTROL", // TB_KEY_CONTROL_L ,
        "R CONTROL" , // TB_KEY_CONTROL_R ,
        "L SHIFT" , // TB_KEY_SHIFT_L ,
        "R SHIFT" , // TB_KEY_SHIFT_R ,
        "L ALT" , // TB_KEY_ALT_L ,
        "R ALT" , // TB_KEY_ALT_R ,
        "UP" , // TB_KEY_UP ,
        "DOWN" , // TB_KEY_DOWN ,
        "RIGHT" , // TB_KEY_RIGHT ,
        "LEFT" , // TB_KEY_LEFT ,
        "NUMPAD 1" , // TB_KEY_NUMPAD_1 ,
        "NUMPAD 2" , // TB_KEY_NUMPAD_2 ,
        "NUMPAD 3" , // TB_KEY_NUMPAD_3 ,
        "NUMPAD 4" , // TB_KEY_NUMPAD_4 ,
        "NUMPAD 5" , // TB_KEY_NUMPAD_5 ,
        "NUMPAD 6" , // TB_KEY_NUMPAD_6 ,
        "NUMPAD 7" , // TB_KEY_NUMPAD_7 ,
        "NUMPAD 8" , // TB_KEY_NUMPAD_8 ,
        "NUMPAD 9" , // TB_KEY_NUMPAD_9 ,
        "NUMPAD 0" , // TB_KEY_NUMPAD_0 ,
        "NUMPAD *" , // TB_KEY_MULTIPLY ,
        "NUMPAD -" , // TB_KEY_SUBSTRACT ,
        "NUMPAD +" , // TB_KEY_ADD ,
        "NUMPAD ." , // TB_KEY_DECIMAL ,
        "ENTER" , // TB_KEY_NUMPAD_ENTER ,
        "NUMPAD /" , // TB_KEY_NUMPAD_DEVIDE ,
        "L BRACKET" , // TB_KEY_BRACKET_L ,
        "R BRACKET" , // TB_KEY_BRACKET_R,
    // TB_KEY_MAX ,
};
TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_KEY_MAX );

const char* TbKeyboard::GetString(TbKeyType keyType)
{
    if( 0 <= keyType && keyType < TB_ARRAY_COUNT_OF(CONVERT_TABLE) ){
        return CONVERT_TABLE[keyType];
    }
    return "";
}

}
