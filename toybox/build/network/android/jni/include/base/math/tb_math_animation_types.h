/*!
 * 数学系アニメーション型
 * @author Miyake Shunsuke
 * @since 2012.01.07
 */

#ifndef _INCLUDED_TB_MATH_ANIMATION_TYPES_H_
#define _INCLUDED_TB_MATH_ANIMATION_TYPES_H_

#include <base/animation/tb_animation_key_frame.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector4.h>

namespace toybox
{

// ベクトルアニメーション
typedef TbAnimationKeyFrameData<TbVector4> TbAnimationKeyFrameDataVec4;
typedef TbAnimationKeyFrame<TbVector4> TbAnimationKeyFrameVec4;

// 2次元ベクトルアニメーション
typedef TbAnimationKeyFrameData<TbVector2> TbAnimationKeyFrameDataVec2f;
typedef TbAnimationKeyFrame<TbVector2> TbAnimationKeyFrameVec2f;

}

#endif
