/*!
 * プレイヤーのアニメーション情報
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_PLAYER_ANIM_H_
#define _INCLUDED_SIDE_ACTION_PLAYER_ANIM_H_

#include <graphics/sprite/tb_sprite.h>

class SideActionAnimData
{
public:
        
    // プレイヤー用
    static const TbRectF32Pod                   PLAYER_UV;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_DEFAULT;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_WALK;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_PUSH_WALK;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_JUMP;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_DEAD;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_SLEEP;
    static const TbAnimationKeyFrameDataRectF32 PLAYER_PLANE;

    // zZZ
    static const TbAnimationKeyFrameDataRectF32 SLEEP_ICON;

    // ステップエネミー用
    static const TbRectF32Pod                   STEP_ENEMY_UV;
    static const TbAnimationKeyFrameDataRectF32 STEP_ENEMY_WALK;

    // アップダウンエネミー用
    static const TbRectF32Pod                   UPDOWN_ENEMY_UV;
    static const TbAnimationKeyFrameDataRectF32 UPDOWN_ENEMY_LOOK;

    // ジャンプ台用
    static const TbRectF32Pod                   JUMP_STAND_UV;
    static const TbAnimationKeyFrameDataRectF32 JUMP_STAND_ACTION;

    // 発射台用
    static const TbRectF32Pod                   LAUNCH_PAD_UV;
    static const TbAnimationKeyFrameDataRectF32 LAUNCH_PAD_ACTION;

    // ステップエネミー用
    static const TbRectF32Pod                   COIN_UV;
    static const TbAnimationKeyFrameDataRectF32 COIN_ANIM;

    // 扇風機用
    static const TbRectF32Pod                   WIND_UV;
    static const TbAnimationKeyFrameDataRectF32 WIND_ANIM;

private:
    SideActionAnimData();
    ~SideActionAnimData();
};

#endif
