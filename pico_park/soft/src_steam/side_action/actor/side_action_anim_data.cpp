/*!
 * プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_anim_data.h"
#include "common/stage/stage.h"
#include "common/map/action_map.h"
#include "side_action/actor/state/side_action_player_state_default.h"
#include "side_action/actor/state/side_action_player_state_dead.h"
#include "root.h"

#include <base/math/tb_math_util.h>

static TbFloat32 ANIM_INTERVAL_W = 32.0f/CMN_TEX_SIZE;
static TbFloat32 ANIM_FRAME_STEP = 12.0f; // アニメーションのスピード
static TbFloat32 UV_ERROR = 0.0f;

static TbRectF32Pod PLAYER_UV_DEFAULT = {(5.0f)/CMN_TEX_SIZE , (3.0f)/CMN_TEX_SIZE , (22.0f)/CMN_TEX_SIZE , (24.0f)/CMN_TEX_SIZE};

const TbRectF32Pod SideActionAnimData::PLAYER_UV = 
{
    PLAYER_UV_DEFAULT.x , PLAYER_UV_DEFAULT.y , PLAYER_UV_DEFAULT.width , PLAYER_UV_DEFAULT.height ,
};

// デフォルトアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_DEFAULT_KEY[] = {
    { 0 , TbRectF32() }
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::PLAYER_DEFAULT(ANIM_DEFAULT_KEY,TB_ARRAY_COUNT_OF(ANIM_DEFAULT_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

// ジャンプアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_JUMP_KEY[] = {
    { 0 , TbRectF32(ANIM_INTERVAL_W*5.0f,0.0f,0.0f,0.0f) }
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::PLAYER_JUMP(ANIM_JUMP_KEY,TB_ARRAY_COUNT_OF(ANIM_JUMP_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

// 死亡アニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_DEAD_KEY[] = {
    { 0 , TbRectF32(ANIM_INTERVAL_W*1.0f,0.0f,0.0f,0.0f) }
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::PLAYER_DEAD(ANIM_DEAD_KEY,TB_ARRAY_COUNT_OF(ANIM_DEAD_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

// 寝るアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_SLEEP_KEY[] = {
    { 0 , TbRectF32(ANIM_INTERVAL_W*10.0f,0.0f,0.0f,0.0f) } , 
#if 0
    { ANIM_FRAME_STEP * 1 , TbRectF32(ANIM_INTERVAL_W*9.0f,ANIM_INTERVAL_W,0.0f,0.0f) } ,
    { ANIM_FRAME_STEP * 2 , TbRectF32(ANIM_INTERVAL_W*10.0f,ANIM_INTERVAL_W,0.0f,0.0f) } ,
    { ANIM_FRAME_STEP * 3 , TbRectF32(ANIM_INTERVAL_W*10.0f,0.0f,0.0f,0.0f) } ,
#endif
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::PLAYER_SLEEP(ANIM_SLEEP_KEY,TB_ARRAY_COUNT_OF(ANIM_SLEEP_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

// 歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_WALK_KEY[] = {
    { 0.0f , TbRectF32(ANIM_INTERVAL_W*4.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*1 , TbRectF32(ANIM_INTERVAL_W*5.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*2 , TbRectF32(ANIM_INTERVAL_W*4.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*3 , TbRectF32(ANIM_INTERVAL_W*3.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*4 , TbRectF32(ANIM_INTERVAL_W*2.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*5 , TbRectF32(ANIM_INTERVAL_W*5.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*6 , TbRectF32(ANIM_INTERVAL_W*2.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*7 , TbRectF32(ANIM_INTERVAL_W*3.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*8 , TbRectF32(ANIM_INTERVAL_W*4.0f,0.0f,0.0f,0.0f) }
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::PLAYER_WALK(ANIM_WALK_KEY,TB_ARRAY_COUNT_OF(ANIM_WALK_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

// 押し歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_PUSH_WALK_KEY[] = {
    { 0.0f , TbRectF32(ANIM_INTERVAL_W*8.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*1 , TbRectF32(ANIM_INTERVAL_W*9.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*2 , TbRectF32(ANIM_INTERVAL_W*8.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*3 , TbRectF32(ANIM_INTERVAL_W*7.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*4 , TbRectF32(ANIM_INTERVAL_W*6.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*5 , TbRectF32(ANIM_INTERVAL_W*9.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*6 , TbRectF32(ANIM_INTERVAL_W*6.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*7 , TbRectF32(ANIM_INTERVAL_W*7.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*8 , TbRectF32(ANIM_INTERVAL_W*8.0f,0.0f,0.0f,0.0f) },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::PLAYER_PUSH_WALK(ANIM_PUSH_WALK_KEY,TB_ARRAY_COUNT_OF(ANIM_PUSH_WALK_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

// 寝るアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_SLEEP_ICON_KEY[] = {
    { 0 , TbRectF32(ANIM_INTERVAL_W*0.0f,0.0f,0.0f,0.0f) } ,
    { ANIM_FRAME_STEP * 1 , TbRectF32(ANIM_INTERVAL_W*1.0f,0.0f,0.0f,0.0f) } ,
    { ANIM_FRAME_STEP * 2 , TbRectF32(ANIM_INTERVAL_W*2.0f,0.0f,0.0f,0.0f) } ,
    { ANIM_FRAME_STEP * 3 , TbRectF32(ANIM_INTERVAL_W*0.0f,0.0f,0.0f,0.0f) } ,
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::SLEEP_ICON(ANIM_SLEEP_ICON_KEY, TB_ARRAY_COUNT_OF(ANIM_SLEEP_ICON_KEY), TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT, TB_FALSE);

/////////////////////////////
// ステップエネミー用
/////////////////////////////

const TbRectF32Pod SideActionAnimData::STEP_ENEMY_UV = 
{
    (32*11.0f+6.0f)/CMN_TEX_SIZE , (8.0f)/CMN_TEX_SIZE , (25.0f)/CMN_TEX_SIZE , (14.0f)/CMN_TEX_SIZE
};

// 歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_STEP_ENEMY_WALK_KEY[] = {
    { 0.0f            , TbRectF32() },
    { ANIM_FRAME_STEP , TbRectF32(0.0f,ANIM_INTERVAL_W,0.0f,0.0f) },
    { ANIM_FRAME_STEP*2.0f , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::STEP_ENEMY_WALK(ANIM_STEP_ENEMY_WALK_KEY,TB_ARRAY_COUNT_OF(ANIM_STEP_ENEMY_WALK_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

/////////////////////////////
// アップダウンエネミー用
/////////////////////////////

const TbRectF32Pod SideActionAnimData::UPDOWN_ENEMY_UV = 
{
    CMN_TEX_UV_RECT(320+64+1,1-1,28+2,24+2) ,
};

// 押し歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_UPDOWN_ENEMY_LOOK_KEY[] = {
    { 0.0f                   , TbRectF32() },
    { ANIM_FRAME_STEP*16.0f  , TbRectF32(ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*20.0f  , TbRectF32(ANIM_INTERVAL_W*2.0f,0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*24.0f  , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::UPDOWN_ENEMY_LOOK(ANIM_UPDOWN_ENEMY_LOOK_KEY,TB_ARRAY_COUNT_OF(ANIM_UPDOWN_ENEMY_LOOK_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

/////////////////////////////
// ジャンプ台用
/////////////////////////////

const TbRectF32Pod SideActionAnimData::JUMP_STAND_UV = 
{
    CMN_TEX_UV_RECT(192,368,16,21) ,
};

// 押し歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_JUMP_STAND_ACTION_KEY[] = {
    { 0.0f   , TbRectF32() },
    { 4.0f   , TbRectF32(ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
    { 8.0f   , TbRectF32(ANIM_INTERVAL_W*2.0f,0.0f,0.0f,0.0f) },
    { 12.0f  , TbRectF32(ANIM_INTERVAL_W*3.0f,0.0f,0.0f,0.0f) },
    { 16.0f  , TbRectF32(ANIM_INTERVAL_W*2.0f,0.0f,0.0f,0.0f) },
    { 20.0f  , TbRectF32(ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
    { 24.0f  , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::JUMP_STAND_ACTION(ANIM_JUMP_STAND_ACTION_KEY,
                                                                           TB_ARRAY_COUNT_OF(ANIM_JUMP_STAND_ACTION_KEY),
                                                                           TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

/////////////////////////////
// 発射台用
/////////////////////////////

const TbRectF32Pod SideActionAnimData::LAUNCH_PAD_UV = 
{
    CMN_TEX_UV_RECT(480,112,22,5) ,
};

// 押し歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_LAUNCH_PAD_ACTION_KEY[] = {
    { 0.0f   , TbRectF32() },
    { 4.0f   , TbRectF32(0.0f,CMN_TEX_UV(7.0f),0.0f,0.0f) },
    { 8.0f   , TbRectF32(0.0f,CMN_TEX_UV(7.0f*2.0f),0.0f,0.0f) },
    { 12.0f  , TbRectF32(0.0f,CMN_TEX_UV(7.0f*3.0f),0.0f,0.0f) },
    { 16.0f  , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::LAUNCH_PAD_ACTION(ANIM_LAUNCH_PAD_ACTION_KEY,
                                                                           TB_ARRAY_COUNT_OF(ANIM_LAUNCH_PAD_ACTION_KEY),
                                                                           TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

/////////////////////////////
// コイン用
/////////////////////////////
const TbRectF32Pod SideActionAnimData::COIN_UV = 
{
    CMN_TEX_UV_RECT(160,320,12,20)
};

// 歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_STEP_COIN_KEY[] = {
    { 0.0f            , TbRectF32() },
    { ANIM_FRAME_STEP*2 , TbRectF32(CMN_TEX_UV(16),0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*3 , TbRectF32(CMN_TEX_UV(32),0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*4 , TbRectF32(CMN_TEX_UV(16),0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*5 , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::COIN_ANIM(ANIM_STEP_COIN_KEY,TB_ARRAY_COUNT_OF(ANIM_STEP_COIN_KEY),TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);

/////////////////////////////
// 扇風機
/////////////////////////////

const TbRectF32Pod SideActionAnimData::WIND_UV = 
{
    CMN_TEX_UV_RECT(444,320,12,17) ,
};

// 押し歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_WINDD_ACTION_KEY[] = {
    { 0.0f   , TbRectF32() },
    { ANIM_FRAME_STEP   , TbRectF32(CMN_TEX_UV(4),0.0f,0.0f,0.0f) },
    { ANIM_FRAME_STEP*2   , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 SideActionAnimData::WIND_ANIM(ANIM_WINDD_ACTION_KEY,
                                                                           TB_ARRAY_COUNT_OF(ANIM_WINDD_ACTION_KEY),
                                                                           TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT,TB_FALSE);