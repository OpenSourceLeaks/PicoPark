/*!
 * @file
 * @brief GFX用レンダーターゲット
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_TARGET_GFX_H_
#define _INCLUDED_TB_RENDER_TARGET_GFX_H_

namespace toybox
{

class TbRenderView;
class TbTexture;

struct TbRenderTargetDepend
{
    TbRenderTargetDepend() 
        : view(nullptr)
        , texture(nullptr)
        , width(0)
        , height(0)
        , colorTargetView(nullptr)
    {}
    
    TbRenderView*               view;
    TbTexture*                  texture;
    TbUint32                    width;
    TbUint32                    height;
    nn::gfx::ColorTargetView*   colorTargetView;
};

}

#endif