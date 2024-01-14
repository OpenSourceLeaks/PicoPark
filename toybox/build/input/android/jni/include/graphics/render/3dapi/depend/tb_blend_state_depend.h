/*!
 * @file
 * @brief ブレンドステートの機種依存コード
 * @author teco
 */

#ifndef _INCLUDED_TB_BLEND_STATE_DEPEND_H_
#define _INCLUDED_TB_BLEND_STATE_DEPEND_H_

#if TB_3DAPI_IS_GFX
#   include "./gfx/tb_blend_state_gfx.h"
#else
namespace toybox
{

class TbBlendStateDepend
{
public:
    void Initialize(const TbBlendStateParam& parma) {}
    void Finalize() {}
};

}
#endif

#endif