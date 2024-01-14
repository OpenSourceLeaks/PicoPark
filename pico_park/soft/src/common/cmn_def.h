/*!
 * 全体で共有する定数
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CMN_DEF_H_
#define _INCLUDED_CMN_DEF_H_

// ブランチ
#define CMN_BUILD_TYPE_NORMAL (0)
#define CMN_BUILD_TYPE_GIANT_INPUT (1)
#define CMN_BUILD_TYPE_100P (2)
#define CMN_BUILD_TYPE_TETRIS (3)
#define CMN_BUILD_TYPE_ALPHA (4)
#define CMN_BUILD_TYPE_OTHER_EVENT (5)
#define CMN_BUILD_TYPE_ARCADE (6)
#define CMN_BUILD_TYPE_OLD    (7) // 古いPICO PARK

#define CMN_ENABLE_BUILD_SOWN (0)           // センスオブワンダーナイト用
#define CMN_ENABLE_BUILD_GIANT_INPUT (CMN_BUILD_TYPE==CMN_BUILD_TYPE_GIANT_INPUT)    // 巨大コントローラ専用
#define CMN_ENABLE_BUILD_100P (CMN_BUILD_TYPE==CMN_BUILD_TYPE_100P)           // 100人同時専用
#define CMN_ENABLE_BUILD_TETRIS (CMN_BUILD_TYPE==CMN_BUILD_TYPE_TETRIS)         // テトリス専用
#define CMN_ENABLE_BUILD_ALPHA (CMN_BUILD_TYPE==CMN_BUILD_TYPE_ALPHA)
#define CMN_ENABLE_BUILD_ARCADE (CMN_BUILD_TYPE==CMN_BUILD_TYPE_ARCADE)
#define CMN_ENABLE_BUILD_OLD (CMN_BUILD_TYPE==CMN_BUILD_TYPE_OLD)
#define CMN_ENABLE_BUILD_EVENT (CMN_BUILD_TYPE==CMN_BUILD_TYPE_OTHER_EVENT)
#define CMN_ENABLE_BUILD_8BITCAFE (0)

#define CMN_BUILD_TYPE (CMN_BUILD_TYPE_NORMAL)

#define CMN_SOUND_VOLUME_LEVEL_DEFAULT (10)
#define CMN_SOUND_VOLUME_LEVEL_MAX     (20)

// 最大プレイヤー数
#define CMN_PLAYER_MAX (10)
// 最小プレイヤー数
#define CMN_PLAYER_MIN (2)

#if CMN_ENABLE_BUILD_ARCADE
#define CMN_PLAYER_COUNT (6)
#elif TB_PLATFORM_IS_NX
#define CMN_PLAYER_COUNT (8)
#elif CMN_ENABLE_BUILD_OLD
#define CMN_PLAYER_COUNT (CMN_PLAYER_MAX)
#else
#define CMN_PLAYER_COUNT (8)
#endif

#define CMN_ENABLE_REMOTE_INPUT (0)

enum CmnPlayerColor
{
    CMN_PLAYER_COLOR_SKYBLUE , 
    CMN_PLAYER_COLOR_GREEN   , 
    CMN_PLAYER_COLOR_ORANGE , 
    CMN_PLAYER_COLOR_PINK   , 
    CMN_PLAYER_COLOR_YELLOW , 
    CMN_PLAYER_COLOR_RED    , 
    CMN_PLAYER_COLOR_BLUE   , 
    CMN_PLAYER_COLOR_PURPLE , 
    CMN_PLAYER_COLOR_BLOWN  , 
    CMN_PLAYER_COLOR_GRAY   , 
};

enum CmnStageType
{
    CMN_STAGE_TYPE_BEST_TIME , 
    CMN_STAGE_TYPE_BATTLE , 
    CMN_STAGE_TYPE_SCORE , 
};

enum CmnDir
{
    CMN_DIR_UP , 
    CMN_DIR_DOWN,
    CMN_DIR_LEFT,
    CMN_DIR_RIGHT,
    CMN_DIR_MAX
};

// 画面の横幅,縦幅
#if 0
#define CMN_WINDOW_WIDTH (640) 
#define CMN_WINDOW_HEIGHT (480) 
#elif CMN_ENABLE_BUILD_100P
#define CMN_WINDOW_WIDTH (770)
#define CMN_WINDOW_HEIGHT (762)
#elif 0
#define CMN_WINDOW_WIDTH (1280)
#define CMN_WINDOW_HEIGHT (800)
#elif TB_PLATFORM_IS_NX || TB_PLATFORM_IS_WIN
#define CMN_WINDOW_WIDTH (1280)
#define CMN_WINDOW_HEIGHT (720)
#else
#define CMN_WINDOW_WIDTH (1248)
#define CMN_WINDOW_HEIGHT (768)
#endif

// 共有テクスチャサイズ
#define CMN_TEX_SIZE (1024)

#define CMN_TEX_POS( value ) static_cast<TbFloat32>(value)/static_cast<TbFloat32>(CMN_TEX_SIZE)

#define CMN_TEX_UV_RECT( x , y , w , h ) static_cast<TbFloat32>(x)/static_cast<TbFloat32>(CMN_TEX_SIZE), \
                                         static_cast<TbFloat32>(y)/static_cast<TbFloat32>(CMN_TEX_SIZE), \
                                         static_cast<TbFloat32>(w)/static_cast<TbFloat32>(CMN_TEX_SIZE), \
                                         static_cast<TbFloat32>(h)/static_cast<TbFloat32>(CMN_TEX_SIZE)
#define CMN_TEX_UV_RECT_2( x , y , w , h , error ) static_cast<TbFloat32>(x)/static_cast<TbFloat32>(CMN_TEX_SIZE), \
                                                   static_cast<TbFloat32>(y)/static_cast<TbFloat32>(CMN_TEX_SIZE)+error, \
                                                   static_cast<TbFloat32>(w)/static_cast<TbFloat32>(CMN_TEX_SIZE)+error, \
                                                   static_cast<TbFloat32>(h)/static_cast<TbFloat32>(CMN_TEX_SIZE)
#define CMN_TEX_UV( val ) (static_cast<TbFloat32>(val)/static_cast<TbFloat32>(CMN_TEX_SIZE))

#define CMN_SPRITE_MAX (100)

// 奥行き
#define CMN_SPRITE_Z_FRONT_6 (-0.6f)
#define CMN_SPRITE_Z_FRONT_5 (-0.5f)
#define CMN_SPRITE_Z_FRONT_4 (-0.4f)
#define CMN_SPRITE_Z_FRONT_3 (-0.3f)
#define CMN_SPRITE_Z_FRONT_2 (-0.2f)
#define CMN_SPRITE_Z_FRONT_1 (-0.1f)
#define CMN_SPRITE_Z_BACK_1  (0.1f)
#define CMN_SPRITE_Z_BACK_2  (0.2f)
#define CMN_SPRITE_Z_BACK_3  (0.3f)

#define CMN_ANIM_INTERVAL_W (32.0f/CMN_TEX_SIZE)
#define CMN_ANIM_FRAME_STEP (12.0f)

enum CmnTaskLine
{
    CMN_TASK_LINE_UPDATE ,
    CMN_TASK_LINE_DRAW ,
    CMN_TASK_LINE_MAX 
};

// ステージファイルディレクトリ
#if CMN_ENABLE_BUILD_SOWN
#   define  CMN_STAGE_DIR "sown/"
#else
#   define  CMN_STAGE_DIR ""
#endif

#define CMN_DEPRECATED 1

#endif
