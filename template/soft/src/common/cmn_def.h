/*!
 * 全体で共有する定数
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CMN_DEF_H_
#define _INCLUDED_CMN_DEF_H_

// 最大プレイヤー数
#define CMN_PLAYER_MAX (10)
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
};

enum CmnStageId
{
    CMN_STAGE_ID_YOU_CAN_BE_A_STAR ,
    CMN_STAGE_ID_THRILL_ME , 
    CMN_STAGE_ID_DEFAULT = CMN_STAGE_ID_THRILL_ME
};

enum CmnCondition
{
    CMN_CONDITION_OR,
    CMN_CONDITION_AND,
};


#define CMN_SPR_SCALE (3.0f)
#define CMN_SPR_POS( value ) static_cast<TbFloat32>(value)*static_cast<TbFloat32>(CMN_SPR_SCALE)
#define CMN_SPR_POS2( x , y ) static_cast<TbFloat32>(x)*static_cast<TbFloat32>(CMN_SPR_SCALE), \
                              static_cast<TbFloat32>(y)*static_cast<TbFloat32>(CMN_SPR_SCALE)

#define CMN_SPR_RECT( x , y , w , h ) static_cast<TbFloat32>(x)*static_cast<TbFloat32>(CMN_SPR_SCALE), \
                                      static_cast<TbFloat32>(y)*static_cast<TbFloat32>(CMN_SPR_SCALE), \
                                      static_cast<TbFloat32>(w)*static_cast<TbFloat32>(CMN_SPR_SCALE), \
                                      static_cast<TbFloat32>(h)*static_cast<TbFloat32>(CMN_SPR_SCALE)

#define THRILL_SPR_SCALE (1.5f)
#define THRILL_SPR_POS( value ) static_cast<TbFloat32>(value)*static_cast<TbFloat32>(THRILL_SPR_SCALE)
#define THRILL_SPR_RECT( x , y , w , h ) static_cast<TbFloat32>(x)*static_cast<TbFloat32>(THRILL_SPR_SCALE), \
                                         static_cast<TbFloat32>(y)*static_cast<TbFloat32>(THRILL_SPR_SCALE), \
                                         static_cast<TbFloat32>(w)*static_cast<TbFloat32>(THRILL_SPR_SCALE), \
                                         static_cast<TbFloat32>(h)*static_cast<TbFloat32>(THRILL_SPR_SCALE)

// 画面の横幅,縦幅
#define CMN_WINDOW_WIDTH (672)
#define CMN_WINDOW_HEIGHT (448)

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

enum CmnTaskLine
{
    CMN_TASK_LINE_UPDATE ,
    CMN_TASK_LINE_DRAW ,
    CMN_TASK_LINE_MAX 
};

// アニメーション
const TbFloat32 CMN_ANIM_FRAME_STEP = 12.0f;

// ステージファイルディレクトリ
#if CMN_ENABLE_BUILD_SOWN
#   define  CMN_STAGE_DIR "sown/"
#else
#   define  CMN_STAGE_DIR ""
#endif

#define CMN_DEPRECATED 1

#endif
