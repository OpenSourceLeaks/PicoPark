/*!
 * 共通ユーティリティ関数軍
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CMN_UTIL_H_
#define _INCLUDED_CMN_UTIL_H_

#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>

class CmnUtil
{
public:

    // メインカラー取得
    static TbColor GetMainColor() {
        return TbColor::FromRgbaU8(255, 134, 77, 255);
    }

    // 背景色取得
    static TbColor GetBackgroundColor() {
        return TbColor::FromRgbaU8(255, 225, 225, 255);
    }

    // プレイヤーカラー取得
    static TbColor GetPlayerColor( TbUint32 playerIndex );

    // プレイヤーポーズUV
    static TbRectF32 GetPlayerPoseUV( TbUint32 playerIndex );

    // 接触面毎の法線
    static const TbVector2& GetContactFaceNormal( CmnContactFace face ) {
        static const TbVector2 OFS[] = 
        {
            TbVector2(0.0f,-1.0f) , // CMN_CONTACT_UP
            TbVector2(0.0f,1.0f) , // CMN_CONTACT_DOWN
            TbVector2(-1.0f,0.0f) , // CMN_CONTACT_LEFT
            TbVector2(1.0f,0.0f) , // CMN_CONTACT_RIGHT
        };
        return OFS[face];
    }

    // サウンドレベル->音量
    static TbFloat32 ConvertSoundLevelToVolume( TbSint32 level ){
        return static_cast<TbFloat32>(level)/static_cast<TbFloat32>(CMN_SOUND_VOLUME_LEVEL_DEFAULT);
    }

};

#endif
