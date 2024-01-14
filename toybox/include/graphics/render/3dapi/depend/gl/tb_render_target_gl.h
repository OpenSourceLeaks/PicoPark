/*!
 * @file
 * @brief GL用レンダーターゲット
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_RENDER_TARGET_STD_GL_H_
#define _INCLUDED_TB_RENDER_TARGET_STD_GL_H_

namespace toybox
{

class TbTexture;
class TbRenderView;

struct TbRenderTargetDepend
{
    TbUint32 width;
    TbUint32 height;
    union {
        TbTexture* texture;
        TbRenderView* view;     
        GLuint rboId;
    };
};

}

#endif