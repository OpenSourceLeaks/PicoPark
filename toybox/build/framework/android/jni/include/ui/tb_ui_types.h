/*!
 * UI型定義
 * @author teco
 */

#ifndef _INCLUDED_UI_TYPES_H_
#define _INCLUDED_UI_TYPES_H_

namespace toybox
{

enum TbUiAlignX
{
    TB_UI_ALIGN_X_LEFT , 
    TB_UI_ALIGN_X_CENTER , 
    TB_UI_ALIGN_X_RIGHT , 
};

enum TbUiAlignY
{
    TB_UI_ALIGN_Y_TOP , 
    TB_UI_ALIGN_Y_CENTER , 
    TB_UI_ALIGN_Y_BOTTOM , 
};

struct TbUiAlign
{
    TbUiAlign() : x(TB_UI_ALIGN_X_LEFT) , y(TB_UI_ALIGN_Y_TOP) {}
    TbUiAlignX x;
    TbUiAlignY y;
};

enum TbUiInputDevice
{
    TB_UI_INPUT_DEVICE_BUTTON , 
    TB_UI_INPUT_DEVICE_CURSOR ,
    TB_UI_INPUT_DEVICE_MAX
};

const TbFloat32 TB_UI_WINDOW_SPRITE_OFFSET = (-1.0f);
const TbUint32  TB_UI_PLAYER_BIT_MAX = (0xffffffff);


// 奥行き
const TbFloat32 TB_UI_SPRITE_Z_FRONT_6 = (-0.6f);
const TbFloat32 TB_UI_SPRITE_Z_FRONT_5 = (-0.5f);
const TbFloat32 TB_UI_SPRITE_Z_FRONT_4 = (-0.4f);
const TbFloat32 TB_UI_SPRITE_Z_FRONT_3 = (-0.3f);
const TbFloat32 TB_UI_SPRITE_Z_FRONT_2 = (-0.2f);
const TbFloat32 TB_UI_SPRITE_Z_FRONT_1 = (-0.1f);
const TbFloat32 TB_UI_SPRITE_Z_BACK_1 = (0.1f);
const TbFloat32 TB_UI_SPRITE_Z_BACK_2 = (0.2f);
const TbFloat32 TB_UI_SPRITE_Z_BACK_3 = (0.3f);

}

#endif