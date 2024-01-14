/*! 
 * NUI(Natural User Interface)のカラーバッファクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_NUI_COLOR_FRAME_KINECT2_H_
#define _INCLUDED_TB_NUI_COLOR_FRAME_KINECT2_H_

namespace toybox
{

struct TbNuiColorFrameDepend
{
    TbNuiColorFrameDepend() : reader(nullptr) , frame(nullptr) , scaleResolution(1) , isReady(TB_FALSE) {}
    IColorFrameReader*      reader;
    IColorFrame*            frame;
    TbUint32                scaleResolution;
    TbBool                  isReady;

};

}

#endif
