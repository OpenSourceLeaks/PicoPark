/*!
 * レンダーターゲット(Stage3D依存)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#ifndef _INCLUDED_TB_RENDER_TARGET_S3D_H_
#define _INCLUDED_TB_RENDER_TARGET_S3D_H_

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
    };
};

}

#endif