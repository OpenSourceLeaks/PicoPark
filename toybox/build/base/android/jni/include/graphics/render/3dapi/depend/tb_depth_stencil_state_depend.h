/*!
 * @file
 * @brief 深度ステンシルステートの機種依存コード
 * @author teco
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_STATE_DEPEND_H_
#define _INCLUDED_TB_DEPTH_STENCIL_STATE_DEPEND_H_

#if TB_3DAPI_IS_GFX
#   include "./gfx/tb_depth_stencil_state_gfx.h"
#else
namespace toybox
{

class TbDepthStencilStateDepend
{
public:
    void Initialize(const TbDepthStencilStateParam& parma) {}
    void Finalize() {}
};

}
#endif

#endif