/*!
 * @file
 * @brief GL用深度ステンシル
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_GL_H_
#define _INCLUDED_TB_DEPTH_STENCIL_GL_H_

namespace toybox
{

struct TbDepthStencilDepend
{
    union {
        GLuint textureId; // テクスチャをレンダリングターゲットにする
        GLuint rboId;     // RBOID
    };
};

}

#endif