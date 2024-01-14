/*!
 * UIのUV管理クラス系
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_UV_H_
#define _INCLUDED_TB_UI_UV_H_

#include <base/math/tb_geometry.h>
#include <graphics/sprite/tb_elastic_sprite.h>

namespace toybox
{

enum
{
    TB_UI_UV_SYSTEM_BEGIN , 
    TB_UI_UV_WINDOW = TB_UI_UV_SYSTEM_BEGIN , 
    TB_UI_UV_WINDOW_FULLSCREEN  , 
    TB_UI_UV_BUTTON             , 
    TB_UI_UV_BUTTON_FOCUS       , 
    TB_UI_UV_BUTTON_PRESS       , 
    TB_UI_UV_TEXT_BOX           , 
    TB_UI_UV_TEXT_BOX_FOCUS     , 
    TB_UI_UV_FRAME              , 
    TB_UI_UV_FRAME_FILL         , 
    TB_UI_UV_SYSTEM_END ,
    TB_UI_UV_USER_BEGIN = TB_UI_UV_SYSTEM_END ,

    TB_UI_UV_SYSTEM_MAX = TB_UI_UV_SYSTEM_END - TB_UI_UV_SYSTEM_BEGIN
};
typedef TbSint32 TbUiUvType;

enum
{
    TB_UI_UV_RECT_SYSTEM_BEGIN                      ,
    TB_UI_UV_RECT_LINE = TB_UI_UV_RECT_SYSTEM_BEGIN , 
    TB_UI_UV_RECT_SYSTEM_END                        ,
    TB_UI_UV_RECT_USER_BEGIN = TB_UI_UV_RECT_SYSTEM_END ,

    TB_UI_UV_RECT_SYSTEM_MAX = TB_UI_UV_RECT_SYSTEM_END - TB_UI_UV_RECT_SYSTEM_BEGIN ,
};
typedef TbSint32 TbUiUvRectType;

class TbUiUvContainer
{
public:

    // コンストラクタ
    TbUiUvContainer() {}

    // デストラクタ
    virtual ~TbUiUvContainer() {}

public:

    // UVパラメータ取得
    TbElasticSprite::UvParam GetUV( TbUiUvType type , TbUint32 playerBit ) const = 0;

    // UVパラメータ取得
    TbRectF32 GetUVRect( TbUiUvRectType type , TbUint32 playerBit ) const = 0;

};

}

#endif
