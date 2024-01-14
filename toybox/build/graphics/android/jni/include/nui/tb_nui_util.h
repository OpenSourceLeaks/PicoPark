/*!
 * Utilityヘッダーファイル
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */

#ifndef _INCLUDED_TB_NUI_UTIL_H_
#define _INCLUDED_TB_NUI_UTIL_H_

#include <nui/tb_nui_types.h>
#include <base/math/tb_math_util.h>

namespace toybox
{

/*!
 * 深度値から、半分以上の割合と半分以下の割合を取得
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
inline void TbNuiGetHalfRateFromDepth( TbFloat32& highHalfRate , TbFloat32& lowHalfRate , const TbUint16 depth )
{
#if TB_NUIAPI_IS_KINECT
    static const TbUint16 depthMaxHalf = ( 0xFFFF >> 3 ) / 2;
#elif TB_NUIAPI_IS_KINECT2
    static const TbUint16 depthMaxHalf = 4000 / 2;
#endif
    highHalfRate = ( static_cast<TbFloat32>( TbMax(0,depth - depthMaxHalf) ) / static_cast<TbFloat32>(depthMaxHalf) );
    lowHalfRate = ( static_cast<TbFloat32>( depth & depthMaxHalf ) / static_cast<TbFloat32>(depthMaxHalf) );
}

/*!
 * 解像度取得
 * @author teco
 */
inline TbDimensionS32 TbNuiGetResolution( TbNuiImageResolutionType type )
{
    struct Data
    {
        TbSint32 x;
        TbSint32 y;
    };
    static const Data TABLE[] = {
        { 80 , 60 } , // TB_NUI_IMAGE_RESOLUTION_80x60
        { 320 , 240 } , // TB_NUI_IMAGE_RESOLUTION_320x240 , 
        { 512 , 424 } , // TB_NUI_IMAGE_RESOLUTION_512x424 , 
        { 640 , 480 } , // TB_NUI_IMAGE_RESOLUTION_640x480
        { 960 , 540 } , // TB_NUI_IMAGE_RESOLUTION_960x540
        { 1280 , 960 } , // TB_NUI_IMAGE_RESOLUTION_1280x960
        { 1920 , 1080 } // TB_NUI_IMAGE_RESOLUTION_1920x1080
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(TABLE)==TB_NUI_IMAGE_RESOLUTION_MAX);
    return TbDimensionS32(TABLE[type].x,TABLE[type].y);
}

}

#endif