/*!
 * @file
 * @brief ビューポートシザーステートの機種依存コード
 * @author teco
 */

#ifndef _INCLUDED_TB_VIEWPORT_SCISSOR_STATE_DEPEND_H_
#define _INCLUDED_TB_VIEWPORT_SCISSOR_STATE_DEPEND_H_

#if TB_3DAPI_IS_GFX
#   include "./gfx/tb_viewport_scissor_state_gfx.h"
#else
namespace toybox
{

class TbViewportScissorStateDepend
{
public:
    void Initialize(const TbViewportScissorStateParam& parma) {}
    void Finalize();
};

}
#endif

#endif