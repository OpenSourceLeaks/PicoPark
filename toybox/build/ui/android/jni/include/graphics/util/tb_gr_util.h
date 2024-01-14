/*!
 * 描画に関わるユーティリティ関数
 * @author teco
 */

#ifndef _INCLUD_TB_GR_UTIL_H_
#define _INCLUD_TB_GR_UTIL_H_

#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{

TbColor TbGammaCorrection(const TbColor& color)
{
    static const TbFloat32 gamma = 2.2f;
    TbColor result = color.Pow(gamma);
    return result;
}

}

#endif
