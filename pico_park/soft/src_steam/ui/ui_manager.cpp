/*!
 * UI管理クラス
 * @author teco
 */

#include "fwd.h"
#include "ui/ui_manager.h"
#include "system/sys_input.h"

#include <framework/sub/tb_framework_simple.h>

namespace
{
    // UV情報


    static const TbRectF32Pod BUTTON_UV[] = 
    {
        { CMN_TEX_UV_RECT(352,224,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(352,228,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(352,232,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(356,224,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(356,228,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(356,232,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(360,224,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(360,228,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(360,232,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod BUTTON_UV_FOCUS[] = 
    {
        { CMN_TEX_UV_RECT(352,240,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(352,244,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(352,248,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(356,240,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(356,244,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(356,248,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(360,240,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(360,244,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(360,248,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod BUTTON_UV_PRESS[] = 
    {
        { CMN_TEX_UV_RECT(352,240+176,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(352,244+176,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(352,248+176,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(356,240+176,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(356,244+176,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(356,248+176,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(360,240+176,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(360,244+176,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(360,248+176,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod FRAME_UV[] = 
    {
        { CMN_TEX_UV_RECT(400,224,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(400,228,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(400,232,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(404,224,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(404,228,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(404,232,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(408,224,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(408,228,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(408,232,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod FRAME_UV_FILL[] = 
    {
        { CMN_TEX_UV_RECT(384,224,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(384,228,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(384,232,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(388,224,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(388,228,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(388,232,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(392,224,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(392,228,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(392,232,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod TEXT_BOX_UV[] = 
    {
        { CMN_TEX_UV_RECT(416,224,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(416,228,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(416,232,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(420,224,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(420,228,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(420,232,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(424,224,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(424,228,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(424,232,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod TEXT_BOX_UV_FOCUS[] = 
    {
        { CMN_TEX_UV_RECT(383,496,4,4) } , // PLACE_LU
        { CMN_TEX_UV_RECT(383,500,4,4) } , // PLACE_LC
        { CMN_TEX_UV_RECT(383,504,4,4) } , // PLACE_LB
        { CMN_TEX_UV_RECT(387,496,4,4) } , // PLACE_UP
        { CMN_TEX_UV_RECT(387,500,4,4) } , // PLACE_IN
        { CMN_TEX_UV_RECT(387,504,4,4) } , // PLACE_BT
        { CMN_TEX_UV_RECT(391,496,4,4) } , // PLACE_RU
        { CMN_TEX_UV_RECT(391,500,4,4) } , // PLACE_RC
        { CMN_TEX_UV_RECT(391,504,4,4) } , // PLACE_RB
    };

    static const TbRectF32Pod WINDOW_UV[] = 
    {
        { CMN_TEX_UV_RECT(352,160,8,8) } , // PLACE_LU
        { CMN_TEX_UV_RECT(352,168,8,8) } , // PLACE_LC
        { CMN_TEX_UV_RECT(352,176,8,8) } , // PLACE_LB
        { CMN_TEX_UV_RECT(360,160,8,8) } , // PLACE_UP
        { CMN_TEX_UV_RECT(360,168,8,8) } , // PLACE_IN
        { CMN_TEX_UV_RECT(360,176,8,8) } , // PLACE_BT
        { CMN_TEX_UV_RECT(368,160,8,8) } , // PLACE_RU
        { CMN_TEX_UV_RECT(368,168,8,8) } , // PLACE_RC
        { CMN_TEX_UV_RECT(368,176,8,8) } , // PLACE_RB
    };

    static const TbRectF32Pod WINDOW_UV_FULLSCREEN[] = 
    {
        { CMN_TEX_UV_RECT(352,256,8,8) } , // PLACE_LU
        { CMN_TEX_UV_RECT(352,264,8,8) } , // PLACE_LC
        { CMN_TEX_UV_RECT(352,272,8,8) } , // PLACE_LB
        { CMN_TEX_UV_RECT(360,256,8,8) } , // PLACE_UP
        { CMN_TEX_UV_RECT(360,264,8,8) } , // PLACE_IN
        { CMN_TEX_UV_RECT(360,272,8,8) } , // PLACE_BT
        { CMN_TEX_UV_RECT(368,256,8,8) } , // PLACE_RU
        { CMN_TEX_UV_RECT(368,264,8,8) } , // PLACE_RC
        { CMN_TEX_UV_RECT(368,272,8,8) } , // PLACE_RB
    };

    static const TbRectF32Pod LINE_UV = 
    {
        CMN_TEX_UV_RECT(464 , 224 , 16 , 2)
    };
    static const TbRectF32Pod WINDOW_CLOSE_UV =
    {
        CMN_TEX_UV_RECT(352 , 112 , 8 , 8)
    };
}

/*!
 * コンストラクタ
 * @author teco
 */
UiManager::UiManager()
    : m_Manager(TbFrameworkSimple::RENDER_PASS_2D)
{
    m_Manager.SetInput(&m_Input);
    m_Manager.SetFullscreenSize(CMN_WINDOW_WIDTH,CMN_WINDOW_HEIGHT);

    // UV情報設定
    m_Manager.SetUV(TB_UI_UV_BUTTON,BUTTON_UV);
    m_Manager.SetUV(TB_UI_UV_BUTTON_FOCUS,BUTTON_UV_FOCUS);
    m_Manager.SetUV(TB_UI_UV_BUTTON_PRESS,BUTTON_UV_PRESS);
    m_Manager.SetUV(TB_UI_UV_FRAME,FRAME_UV);
    m_Manager.SetUV(TB_UI_UV_FRAME_FILL,FRAME_UV_FILL);
    m_Manager.SetUV(TB_UI_UV_TEXT_BOX,TEXT_BOX_UV);
    m_Manager.SetUV(TB_UI_UV_TEXT_BOX_FOCUS,TEXT_BOX_UV_FOCUS);
    m_Manager.SetUV(TB_UI_UV_WINDOW,WINDOW_UV);
    m_Manager.SetUV(TB_UI_UV_WINDOW_FULLSCREEN,WINDOW_UV_FULLSCREEN);
    
    m_Manager.SetUVRect(TB_UI_UV_RECT_LINE,LINE_UV);
    m_Manager.SetUVRect(TB_UI_UV_RECT_WINDOW_CLOSE,WINDOW_CLOSE_UV);
}

