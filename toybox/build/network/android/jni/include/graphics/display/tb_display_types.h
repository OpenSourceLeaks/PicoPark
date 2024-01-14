/*!
 * ディスプレイ型定義
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */

#ifndef _INCLUDED_TB_DISPLAY_TYPES_H_
#define _INCLUDED_TB_DISPLAY_TYPES_H_

#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{

struct TbDisplayMode
{
    TbUint32 width;         //!< 横幅
    TbUint32 height;        //!< 縦幅
    TbUint32 refreshRate;   //!< リフレッシュレート
    TbColorFormat format;   //!< フォーマット
};

}

#endif