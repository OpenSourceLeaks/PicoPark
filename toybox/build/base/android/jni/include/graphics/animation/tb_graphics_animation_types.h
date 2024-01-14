/*!
 * 描画系アニメーション型
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */

#ifndef _INCLUDED_TB_GRAPHICS_ANIMATION_TYPES_H_
#define _INCLUDED_TB_GRAPHICS_ANIMATION_TYPES_H_

#include <base/animation/tb_animation_key_frame.h>
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

// カラーアニメーション
typedef TbAnimationKeyFrameData<TbColorF128> TbAnimationKeyFrameDataColorR32G32B32A32;
typedef TbAnimationKeyFrame<TbColorF128> TbAnimationKeyFrameColorR32G32B32A32;
typedef TbAnimationKeyFrameData<TbColorU32> TbAnimationKeyFrameDataColorR8G8B8A8;
typedef TbAnimationKeyFrame<TbColorU32> TbAnimationKeyFrameColorR8G8B8A8;

// 矩形アニメーション
typedef TbAnimationKeyFrameData<TbRectF32> TbAnimationKeyFrameDataRectF32;
typedef TbAnimationKeyFrame<TbRectF32> TbAnimationKeyFrameRectF32;

}

#endif
