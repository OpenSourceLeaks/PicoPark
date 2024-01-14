/*!
 * @file
 * @brief パッド系定義
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_PAD_TYPES_H_
#define _INCLDUED_TB_PAD_TYPES_H_

#include <base/math/tb_vector2.h>

namespace toybox
{

enum TbPadButton
{
    TB_PAD_UP,
    TB_PAD_DOWN,
    TB_PAD_LEFT,
    TB_PAD_RIGHT,
    TB_PAD_START,
    TB_PAD_SELECT,
    TB_PAD_A,
    TB_PAD_B,
    TB_PAD_X,
    TB_PAD_Y,
    TB_PAD_L1,
    TB_PAD_R1,
    TB_PAD_L2,
    TB_PAD_R2,
    TB_PAD_BUTTON_MAX
};

#define TB_PAD_UP_BIT     TB_BIT(TB_PAD_UP)
#define TB_PAD_DOWN_BIT   TB_BIT(TB_PAD_DOWN)
#define TB_PAD_LEFT_BIT   TB_BIT(TB_PAD_LEFT)
#define TB_PAD_RIGHT_BIT  TB_BIT(TB_PAD_RIGHT)
#define TB_PAD_START_BIT  TB_BIT(TB_PAD_START)
#define TB_PAD_SELECT_BIT TB_BIT(TB_PAD_SELECT)
#define TB_PAD_A_BIT      TB_BIT(TB_PAD_A)
#define TB_PAD_B_BIT      TB_BIT(TB_PAD_B)
#define TB_PAD_X_BIT      TB_BIT(TB_PAD_X)
#define TB_PAD_Y_BIT      TB_BIT(TB_PAD_Y)
#define TB_PAD_L1_BIT     TB_BIT(TB_PAD_L1)
#define TB_PAD_R1_BIT     TB_BIT(TB_PAD_R1)
#define TB_PAD_L2_BIT     TB_BIT(TB_PAD_L2)
#define TB_PAD_R2_BIT     TB_BIT(TB_PAD_R2)
#define TB_PAD_ANY_BIT    (0xFFFFFFFF)
    

struct TbPadButtonSetting
{
    TbPadButtonSetting() : productGuid(0) {}
    TbBool operator<( TbUint64 guid ) {
        return productGuid < guid;
    }
    TbBool operator==( TbUint64 guid ) {
        return productGuid == guid;
    }
    TbUint64      productGuid;
    TbUint32      table[TB_PAD_BUTTON_MAX];
};

// パッドの機種共通情報
struct TbPadInfo
{
    enum {
        FLAG_ACTIVE,
        FLAG_CONNECT,
        FLAG_REMOVE,
        FLAG_INSERT,
    };
    
    TbPadInfo() {
        Init();
    }
    void Init()
    {
        now=0;
        old=0;
        repeat=0;
        repeatFirst=0;
        flag.Clear();
        TbMemClear(repeatMilliSec,sizeof(repeatMilliSec));
        flag.SetBit(FLAG_ACTIVE);
        guid = 0;
        productGuid = 0;
        productName.Clear();
        setting = nullptr;
    }
    TbUint32            now;              //現フレーム入力情報
    TbUint32            old;              //前フレーム入力情報
    TbUint32            repeat;           //リピートフラグ
    TbUint32            repeatFirst;      //リピートフラグ(初回)
    TbVector2           leftStick;        //左側アナログスティックの方向
    TbVector2           rightStick;       //右側アナログスティックの方向
    TbUint8             leftTrigger;      //左側トリガー
    TbUint8             rightTrigger;     //右側トリガー
    TbFloat32           repeatMilliSec[TB_PAD_BUTTON_MAX];
    TbBitArray32        flag;             //フラグ
    TbUint64            guid;             // デバイスのGUID
    TbUint64            productGuid;      // 製品のGUID
    TbStaticString512   productName;
    TbPadButtonSetting* setting;
};

}

#if TB_PLATFORM_IS_WIN
#include <input/depend/win32/tb_pad_types_win32.h>
#endif

#endif