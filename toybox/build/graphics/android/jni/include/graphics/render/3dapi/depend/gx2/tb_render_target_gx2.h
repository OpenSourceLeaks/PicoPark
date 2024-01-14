/*!
 * @file
 * @brief GX2用レンダーターゲット
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_TARGET_GX2_H_
#define _INCLUDED_TB_RENDER_TARGET_GX2_H_

namespace toybox
{

struct TbRenderTargetDepend
{
    TbRenderTargetDepend() 
        : view(nullptr()
        , width(0)
        , height(0)
    {}
    
    TbRenderView*  view;
    TbUint32       width;
    TbUint32       height;
    GX2ColorBuffer colorBuffer;
};

}

#endif