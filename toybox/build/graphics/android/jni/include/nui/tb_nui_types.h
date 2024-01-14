/*! 
 * NUI(Natural User Interface)用型定義
 * @author Miyake Shunsuke
 * @since 2011.07.21
 */

#ifndef _INCLUDED_TB_NUI_TYPES_H_
#define _INCLUDED_TB_NUI_TYPES_H_

#include <base/math/tb_vector4.h>

// API
#define TB_NUIAPI_KINECT 1   // Kinect SDK for Windows
#define TB_NUIAPI_KINECT2 2   // Kinect SDK2

#if TB_PLATFORM_IS_WIN
    #define TB_NUIAPI_TARGET TB_NUIAPI_KINECT2
#else
    #error "Invalid Platform"
#endif

#define TB_NUIAPI_IS_KINECT  (TB_NUIAPI_TARGET == TB_NUIAPI_KINECT)
#define TB_NUIAPI_IS_KINECT2 (TB_NUIAPI_TARGET == TB_NUIAPI_KINECT2)

// 必要なインクルード
#if TB_NUIAPI_IS_KINECT
    #include <NuiApi.h>
    #include <NuiSkeleton.h>
    #include <NuiImageCamera.h>
    #include <NuiSkeleton.h>
    #pragma comment(lib,"Kinect10.lib")
#elif TB_NUIAPI_IS_KINECT2
    #include <Kinect.h>
    #include <Kinect.Face.h>
    #pragma comment(lib,"Kinect20.lib")
#elif TB_NUIAPI_TARGET == TB_NUIAPI_OPENNI
//    #include <>
    #error "not implemented"
#else
    #error "unknown api"
#endif

namespace toybox
{

// 初期化フラグ
enum TbNuiInitalizeBit{
    TB_NUI_INITIALIZE_BIT_USE_COLOR = TB_BIT(1) , 
    TB_NUI_INITIALIZE_BIT_USE_SKELETON = TB_BIT(2) , 
    TB_NUI_INITIALIZE_BIT_USE_DEPTH = TB_BIT(3) , 
    TB_NUI_INITIALIZE_BIT_DEFAULT_HARDWARE_THREAD = 0xFFFFFFFF , 
};

// 初期化パラメータ
struct TbNuiCreateParam
{
    TbUint32 flagBit;
};

// 追跡状態
enum TbNuiSkeletonTrackState
{
    TB_NUI_SKELETON_TRACKED_NOT , // 追跡できていない
    TB_NUI_SKELETON_TRACKED_POS ,    // 位置情報のみ
    TB_NUI_SKELETON_TRACKED ,     // 追跡できている
};

// 骨格部位の追跡状態
enum TbNuiSkeletonPosTrackState
{
    TB_NUI_SKELETON_POS_TRACKED_NOT , // 追跡できていない
    TB_NUI_SKELETON_POS_TRACKED_INFERRED ,    // 推論
    TB_NUI_SKELETON_POS_TRACKED ,     // 追跡できている
};

// 骨格位置(Kinectv1準拠)
enum TbNuiSkeletonPos
{
    TB_NUI_SKELETON_POS_HIP_C       , // 腰中心
    TB_NUI_SKELETON_POS_SPINE       , // 背骨
    TB_NUI_SKELETON_POS_SHOULDER_C  , // 肩中心
    TB_NUI_SKELETON_POS_HEAD        , // 頭
    TB_NUI_SKELETON_POS_SHOULDER_L  , // 肩左
    TB_NUI_SKELETON_POS_ELBOW_L     , // 肘左
    TB_NUI_SKELETON_POS_WRIST_L     , // 手首左
    TB_NUI_SKELETON_POS_HAND_L      , // 手左
    TB_NUI_SKELETON_POS_SHOULDER_R  , // 肩右
    TB_NUI_SKELETON_POS_ELBOW_R     , // 肘右
    TB_NUI_SKELETON_POS_WRIST_R     , // 手首右
    TB_NUI_SKELETON_POS_HAND_R      , // 手右
    TB_NUI_SKELETON_POS_HIP_L       , // 腰左
    TB_NUI_SKELETON_POS_KNEE_L      , // 膝左
    TB_NUI_SKELETON_POS_ANKLE_L     , // 足首左
    TB_NUI_SKELETON_POS_FOOT_L      , // 足左
    TB_NUI_SKELETON_POS_HIP_R       , // 腰右
    TB_NUI_SKELETON_POS_KNEE_R      , // 膝右
    TB_NUI_SKELETON_POS_ANKLE_R     , // 足首右
    TB_NUI_SKELETON_POS_FOOT_R      , // 足右
    TB_NUI_SKELETON_POS_COUNT , 
};

// 追跡スケルトン情報
struct TbNuiTrackedSkeleton
{
    TbBitArray32 bitArray;                               // ビット配列
    TbVector4    skeletonPos[TB_NUI_SKELETON_POS_COUNT]; // 座標
};

// スケルトン情報
struct TbNuiSkeleton
{
    enum {
        FLAG_CLIPPED_RIGHT , 
        FLAG_CLIPPED_LEFT , 
        FLAG_CLIPPED_TOP , 
        FLAG_CLIPPED_BOTTOM , 

        FLAG_CLIPPED_RIGHT_BIT = TB_BIT(FLAG_CLIPPED_RIGHT) , 
        FLAG_CLIPPED_LEFT_BIT = TB_BIT(FLAG_CLIPPED_LEFT) , 
        FLAG_CLIPPED_TOP_BIT = TB_BIT(FLAG_CLIPPED_TOP) , 
        FLAG_CLIPPED_BOTTOM_BIT = TB_BIT(FLAG_CLIPPED_BOTTOM) , 
    };
    TbBitArray32             bitArray;         // ビット配列
    TbVector4                pos;              // 情報
    TbNuiSkeletonTrackState  trackState;       // 追跡状態
    TbNuiTrackedSkeleton*    trackedSkeleton;  // 追跡情報
};

enum TbNuiImageType
{
    TB_NUI_IMAGE_TYPE_COLOR , 
    TB_NUI_IMAGE_TYPE_DEPTH , 
    TB_NUI_IMAGE_TYPE_MAX , 
};

// 画像解像度
enum TbNuiImageResolutionType
{
    TB_NUI_IMAGE_RESOLUTION_INVALID = -1 , 
    TB_NUI_IMAGE_RESOLUTION_80x60 , 
    TB_NUI_IMAGE_RESOLUTION_320x240 , 
    TB_NUI_IMAGE_RESOLUTION_512x424 , 
    TB_NUI_IMAGE_RESOLUTION_640x480 , 
    TB_NUI_IMAGE_RESOLUTION_960x540 ,
    TB_NUI_IMAGE_RESOLUTION_1280x960 , 
    TB_NUI_IMAGE_RESOLUTION_1920x1080 ,
    TB_NUI_IMAGE_RESOLUTION_MAX ,
};

// 深度座標情報
struct TbNuiDepthPos
{
    TbNuiDepthPos() : resolutionType(TB_NUI_IMAGE_RESOLUTION_INVALID),x(0),y(0),depth(0) {}
    TbNuiDepthPos( TbNuiImageResolutionType  r , TbSint32 tmpX , TbSint32 tmpY , TbUint16 d ) 
        :resolutionType(r) , x(tmpX) , y(tmpY) , depth(d)
    {}
    TbNuiImageResolutionType resolutionType;
    TbSint32 x;
    TbSint32 y;
    TbUint16 depth;
};

// カラー座標情報
struct TbNuiColorPos
{
    TbNuiColorPos() : resolutionType(TB_NUI_IMAGE_RESOLUTION_INVALID),x(0),y(0) {}
    TbNuiColorPos( TbNuiImageResolutionType  r , TbSint32 tmpX , TbSint32 tmpY ) 
        :resolutionType(r) , x(tmpX) , y(tmpY)
    {}
    TbNuiImageResolutionType resolutionType;
    TbSint32 x;
    TbSint32 y;
};

// API別定数
#if TB_NUIAPI_IS_KINECT
#define TB_NUI_SKELETON_MAX             NUI_SKELETON_COUNT
#define TB_NUI_TRACKED_SKELETON_MAX     NUI_SKELETON_MAX_TRACKED_COUNT
#define TB_NUI_TRACKING_ID_INVALID      NUI_SKELETON_INVALID_TRACKING_ID
#define TB_NUI_CAMERA_TILT_MOTOR_ANGKE_MAX NUI_CAMERA_ELEVATION_MAXIMUM
#define TB_NUI_CAMERA_TILT_MOTOR_ANGKE_MIN NUI_CAMERA_ELEVATION_MINIMUM
#define TB_NUI_CAMERA_COLOR_RESOLUTION      TB_NUI_IMAGE_RESOLUTION_640x480
#define TB_NUI_CAMERA_DEPTH_RESOLUTION      TB_NUI_IMAGE_RESOLUTION_320x240
#elif TB_NUIAPI_IS_KINECT2
#define TB_NUI_SKELETON_MAX                 BODY_COUNT
#define TB_NUI_TRACKED_SKELETON_MAX         BODY_COUNT
#define TB_NUI_TRACKING_ID_INVALID          (-1)
#define TB_NUI_CAMERA_TILT_MOTOR_ANGKE_MAX  0
#define TB_NUI_CAMERA_TILT_MOTOR_ANGKE_MIN  0
#define TB_NUI_CAMERA_COLOR_RESOLUTION      TB_NUI_IMAGE_RESOLUTION_1920x1080
#define TB_NUI_CAMERA_DEPTH_RESOLUTION      TB_NUI_IMAGE_RESOLUTION_512x424
#else
#define TB_NUI_SKELETON_MAX 1
#define TB_NUI_TRACKED_SKELETON_MAX 1
#define TB_NUI_TRACKING_ID_INVALID 0
#define TB_NUI_CAMERA_TILT_MOTOR_ANGKE_MAX 1
#define TB_NUI_CAMERA_TILT_MOTOR_ANGKE_MIN -1
#define TB_NUI_CAMERA_COLOR_RESOLUTION      TB_NUI_IMAGE_RESOLUTION_640x480
#define TB_NUI_CAMERA_DEPTH_RESOLUTION      TB_NUI_IMAGE_RESOLUTION_320x240
#endif

// 左右の骨格に簡単にアクセスするためのマクロ
#define TB_NUI_SKELETON_POS_SHOULDER( isLeft ) (TB_NUI_SKELETON_POS_SHOULDER_L + !(isLeft) * (TB_NUI_SKELETON_POS_SHOULDER_R-TB_NUI_SKELETON_POS_SHOULDER_L))
#define TB_NUI_SKELETON_POS_ELBOW( isLeft ) (TB_NUI_SKELETON_POS_ELBOW_L + !(isLeft) * (TB_NUI_SKELETON_POS_ELBOW_R-TB_NUI_SKELETON_POS_ELBOW_L))
#define TB_NUI_SKELETON_POS_WRIST( isLeft ) (TB_NUI_SKELETON_POS_WRIST_L + !(isLeft) * (TB_NUI_SKELETON_POS_WRIST_R-TB_NUI_SKELETON_POS_WRIST_L))
#define TB_NUI_SKELETON_POS_HAND( isLeft ) (TB_NUI_SKELETON_POS_HAND_L + !(isLeft) * (TB_NUI_SKELETON_POS_HAND_R-TB_NUI_SKELETON_POS_HAND_L))
#define TB_NUI_SKELETON_POS_HIP( isLeft ) (TB_NUI_SKELETON_POS_HIP_L + !(isLeft) * (TB_NUI_SKELETON_POS_HIP_R-TB_NUI_SKELETON_POS_HIP_L))
#define TB_NUI_SKELETON_POS_KNEE( isLeft ) (TB_NUI_SKELETON_POS_KNEE_L + !(isLeft) * (TB_NUI_SKELETON_POS_KNEE_R-TB_NUI_SKELETON_POS_KNEE_L))
#define TB_NUI_SKELETON_POS_ANKLE( isLeft ) (TB_NUI_SKELETON_POS_ANKLE_L + !(isLeft) * (TB_NUI_SKELETON_POS_ANKLE_R-TB_NUI_SKELETON_POS_ANKLE_L))
#define TB_NUI_SKELETON_POS_FOOT( isLeft ) (TB_NUI_SKELETON_POS_FOOT_L + !(isLeft) * (TB_NUI_SKELETON_POS_FOOT_R-TB_NUI_SKELETON_POS_FOOT_L))


}

#endif

