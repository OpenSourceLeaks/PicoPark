/*!
* @file
* @brief ラスタライザステートの機種依存コード
* @author teco
*/

#ifndef _INCLUDED_TB_RASTEROZER_STATE_DEPEND_H_
#define _INCLUDED_TB_RASTEROZER_STATE_DEPEND_H_

#if TB_3DAPI_IS_GFX
#   include "./gfx/tb_rasterizer_state_gfx.h"
#else
namespace toybox
{

class TbRasterizerStateDepend
{
public:
    void Initialize(const TbRasterizerStateParam& parma) {}
    void Finalize(){}
};

}
#endif

#endif