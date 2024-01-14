/*!
 * 共通ユーティリティ関数軍
 * @author teco
 */

#include "fwd.h"
#include "cmn_util.h"

namespace
{
    const TbColor COLOR_TABLE[] = 
    {
        TbColor::FromRgbaU8(191,255,223,255) ,           //
        TbColor::FromRgbaU8(255,140,140,255) ,           //
        TbColor::FromRgbaU8(255,255,140,255) ,         //
        TbColor::FromRgbaU8(168,255,168,255) ,          //
        TbColor::FromRgbaU8(255,183,130,255) ,           //
        TbColor::FromRgbaU8(255,168,255,255) ,        //
        TbColor::FromRgbaU8(127,191,255,255) ,          //
        TbColor::FromRgbaU8(207,159,255,255) ,           //
        TbColor::FromRgbaU8(207,183,159,255) ,        //
        TbColor::FromRgbaU8(224,224,224,255) ,         //
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(COLOR_TABLE)==CMN_PLAYER_MAX);
}

/*! 
 * プレイヤーカラー取得
 * @author teco
 * @since 2013.10.12
 */
TbColor CmnUtil::GetPlayerColor( TbUint32 playerIndex )
{
    if( playerIndex < CMN_PLAYER_MAX ){
        return COLOR_TABLE[playerIndex];
    }
    return TbColor::Black();
}

// プレイヤーポーズUV
TbRectF32 CmnUtil::GetPlayerPoseUV( TbUint32 playerIndex )
{
    TbRectF32 result( CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f) );
    result.y += CMN_TEX_POS(32.0f*(playerIndex));
    return result;
}
