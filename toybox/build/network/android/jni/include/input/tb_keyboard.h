/*!
 * @file
 * @brief キーボード入力
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_KEYBOARD_H_
#define _INCLDUED_TB_KEYBOARD_H_

#include <base/util/tb_singleton.h>

namespace toybox
{

class TbWindow;

enum TbKeyType
{
    // アルファベット
    TB_KEY_BEGIN,
    TB_KEY_A = TB_KEY_BEGIN,
    TB_KEY_B,
    TB_KEY_C,
    TB_KEY_D,
    TB_KEY_E,
    TB_KEY_F,
    TB_KEY_G,
    TB_KEY_H,
    TB_KEY_I,
    TB_KEY_J,
    TB_KEY_K,
    TB_KEY_L,
    TB_KEY_M,
    TB_KEY_N,
    TB_KEY_O,
    TB_KEY_P,
    TB_KEY_Q,
    TB_KEY_R,
    TB_KEY_S,
    TB_KEY_T,
    TB_KEY_U,
    TB_KEY_V,
    TB_KEY_W,
    TB_KEY_X,
    TB_KEY_Y,
    TB_KEY_Z,
    // 数字
    TB_KEY_1,
    TB_KEY_2,
    TB_KEY_3,
    TB_KEY_4,
    TB_KEY_5,
    TB_KEY_6,
    TB_KEY_7,
    TB_KEY_8,
    TB_KEY_9,
    TB_KEY_0,
    // 特殊文字
    TB_KEY_MINUS,
    TB_KEY_EQUAL,
    TB_KEY_SPACE,
    TB_KEY_ESCAPE,
    TB_KEY_TAB,
    TB_KEY_BACK,
    TB_KEY_RETURN,
    TB_KEY_F1,
    TB_KEY_F2,
    TB_KEY_F3,
    TB_KEY_F4,
    TB_KEY_F5,
    TB_KEY_F6,
    TB_KEY_F7,
    TB_KEY_F8,
    TB_KEY_F9,
    TB_KEY_F10,
    TB_KEY_F11,
    TB_KEY_F12,
    TB_KEY_F13,
    TB_KEY_F14,
    TB_KEY_F15,
    TB_KEY_PAGE_UP,
    TB_KEY_PAGE_DOWN,
    TB_KEY_CONTROL_L,
    TB_KEY_CONTROL_R,
    TB_KEY_SHIFT_L,
    TB_KEY_SHIFT_R,
    TB_KEY_ALT_L,
    TB_KEY_ALT_R,
    TB_KEY_UP,
    TB_KEY_DOWN,
    TB_KEY_RIGHT,
    TB_KEY_LEFT,
    TB_KEY_NUMPAD_1,
    TB_KEY_NUMPAD_2,
    TB_KEY_NUMPAD_3,
    TB_KEY_NUMPAD_4,
    TB_KEY_NUMPAD_5,
    TB_KEY_NUMPAD_6,
    TB_KEY_NUMPAD_7,
    TB_KEY_NUMPAD_8,
    TB_KEY_NUMPAD_9,
    TB_KEY_NUMPAD_0,
    TB_KEY_MULTIPLY,
    TB_KEY_SUBSTRACT,
    TB_KEY_ADD,
    TB_KEY_DECIMAL,
    TB_KEY_NUMPAD_ENTER,
    TB_KEY_NUMPAD_DEVIDE,
    TB_KEY_BRACKET_L,
    TB_KEY_BRACKET_R,

    TB_KEY_END,
    TB_KEY_MAX = TB_KEY_END - TB_KEY_BEGIN ,
    TB_KEY_INVALID = TB_KEY_MAX
};

/*!
 * キーボードクラス生成パラメータ
 */
struct TbKeyboardParameter
{
    TbWindow* window;
};

}

// 依存クラスヘッダー
#include "./depend/tb_keyboard_depend.h"

namespace toybox
{

/*!
 * キーボードクラス
 * @author Miyake Shunsuke
 */
class TbKeyboard : public TbSingletonRef<TbKeyboard >
{
public:

    //コンストラクタ
    TbKeyboard( const TbKeyboardParameter* parameter , TbSint32 count );

    //デストラクタ
    virtual ~TbKeyboard(void);

public:

    //更新
    void Update();

public:

    // 有効なキーボード数
    static TbSint32 GetCount() { return GetInstance().getCount(); }
    //トリガーしたか
    static TbBool IsPress(const TbKeyType keyType,const TbUint32 index=0){return GetInstance().isPress(keyType,index);}
    //離したか
    static TbBool IsRelease(const TbKeyType keyType,const TbUint32 index=0){return GetInstance().isRelease(keyType,index);}
    //押しているか
    static TbBool IsOn(const TbKeyType keyType,const TbUint32 index=0){return GetInstance().isOn(keyType,index);}
    //押しているか(リピート)
    static TbBool IsOnRepeat(const TbKeyType keyType,const TbUint32 index=0){return GetInstance().isOnRepeat(keyType,index);}
    //リピートリセット
    static void ResetRepeat(const TbKeyType keyType,const TbUint32 index=0){return GetInstance().resetRepeat(keyType,index);}
    // 文字列取得
    static const TbChar8* GetString(TbKeyType keyType);

private:

    // 有効なキーボード数
    TbSint32 getCount() const;
    //トリガーしたか
    TbBool isPress(const TbKeyType keyType,const TbUint32 index) const;
    //離したか
    TbBool isRelease(const TbKeyType keyType,const TbUint32 index) const;
    //押しているか
    TbBool isOn(const TbKeyType keyType,const TbUint32 index) const;
    //押しているか(リピート)
    TbBool isOnRepeat(const TbKeyType keyType,const TbUint32 index) const;
    //リピートリセット
    void resetRepeat(const TbKeyType keyType,const TbUint32 index);

private:

    TbKeyboardDepend m_Depend; // 依存コード

};

}

#endif