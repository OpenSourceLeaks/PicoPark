/*!
 * スプライト型
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_TYPES_H_
#define _INCLUDED_TB_SPRITE_TYPES_H_

#include <base/animation/tb_animation_key_frame.h>
#include <graphics/animation/tb_graphics_animation_types.h>
#include <base/math/tb_math_animation_types.h>

namespace toybox
{

enum TbSpriteType
{
    TB_SPRITE_TYPE_INVALID ,        // 不正値
    TB_SPRITE_TYPE_RECT ,           // 矩形
    TB_SPRITE_TYPE_RECT_TEXTURE ,   // テクスチャ付き矩形
};

enum TbSpriteInheritType
{
    TB_SPRITE_INHERIT_TYPE_INVALID , 
};

#define TB_SPRITE_NODE_INDEX_ROOT (0)
#define TB_SPRITE_NODE_INDEX_INVALID (0xFFFFFFFF)

enum TbSpriteAnimationType
{
    TB_SPRITE_ANIMATION_TYPE_INVALID ,      // 不正値
    TB_SPRITE_ANIMATION_TYPE_BEGIN , 
    TB_SPRITE_ANIMATION_TYPE_OVERWRITE = TB_SPRITE_ANIMATION_TYPE_BEGIN ,    // 上書きアニメーション
    TB_SPRITE_ANIMATION_TYPE_ADD ,          // 加算アニメーション
    TB_SPRITE_ANIMATION_TYPE_END , 
    TB_SPRITE_ANIMATION_TYPE_MAX = TB_SPRITE_ANIMATION_TYPE_END - TB_SPRITE_ANIMATION_TYPE_BEGIN
};

// スプライトノードアニメーション用初期化パラメータ
struct TbSpriteNodeAnimInitParam
{
    TbSpriteNodeAnimInitParam()
        : name(nullptr)
        , rectAnim(nullptr)
        , uvOffsetAnim(nullptr)
        , translateAnim( nullptr )
        , rotAnim( nullptr ) 
        , scaleAnim( nullptr )
        , colorAnim( nullptr )
    {}
    ~TbSpriteNodeAnimInitParam()
    {}
    const char*                           name;               // 名前
    const TbAnimationKeyFrameDataRectF32* rectAnim;           // 矩形
    const TbAnimationKeyFrameDataRectF32* uvOffsetAnim;         // UV矩形
    const TbAnimationKeyFrameDataVec2f*   translateAnim;      // 移動
    const TbAnimationKeyFrameDataF32*     rotAnim;            // 回転
    const TbAnimationKeyFrameDataVec2f*   scaleAnim;          // 拡大
    const TbAnimationKeyFrameDataColorR32G32B32A32*   colorAnim;          // 拡大
};

}

#endif
