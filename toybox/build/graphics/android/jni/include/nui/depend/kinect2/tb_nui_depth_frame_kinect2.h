/*! 
 * NUI(Natural User Interface)の深度バッファクラス
 */

#ifndef _INCLUDED_TB_NUI_DEPTH_FRAME_KINECT2_H_
#define _INCLUDED_TB_NUI_DePTH_FRAME_KINECT2_H_

namespace toybox
{

struct TbNuiDepthFrameDepend
{
    TbNuiDepthFrameDepend() : depthReader(nullptr) , bodyIndexFrameReader(nullptr) , bodyIndexBuffer(nullptr) , isReady(TB_FALSE) , depthFrame(nullptr) , bodyIndexFrame(nullptr) {}
//    IMultiSourceFrameReader*    reader;
    IDepthFrameReader*          depthReader;
    IBodyIndexFrameReader*      bodyIndexFrameReader;
    TbUint8*                    bodyIndexBuffer;
    TbBool                      isReady;
    IDepthFrame*                depthFrame;
    IBodyIndexFrame*            bodyIndexFrame;
};

}

#endif
