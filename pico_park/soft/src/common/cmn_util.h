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
#include <crayon/cr_def.h>
#include <crayon/input/cr_input.h>
#include <base/io/tb_file_manager.h>

class CmnUtil
{
public:

    // メインカラー取得
    static TbColor GetMainColor() {
        return TbColor::FromRgbaU8(255, 134, 77, 255);
    }

    // マジョリティカラー取得
    static TbColor GetMajorityColor() {
        return TbColor::FromRgbaU8(191, 255, 223, 255);
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
    static const TbVector2& GetContactFaceNormal( CrContactFace face ) {
        static const TbVector2 OFS[] = 
        {
            TbVector2(0.0f,-1.0f) , // CR_CONTACT_UP
            TbVector2(0.0f,1.0f) , // CR_CONTACT_DOWN
            TbVector2(-1.0f,0.0f) , // CR_CONTACT_LEFT
            TbVector2(1.0f,0.0f) , // CR_CONTACT_RIGHT
        };
        return OFS[face];
    }

    // 入力ID補正
    static CrInput::Button CorrectButton(CrInput::Button button);

    // サウンドレベル->音量
    static TbFloat32 ConvertSoundLevelToVolume( TbSint32 level ){
        return static_cast<TbFloat32>(level)/static_cast<TbFloat32>(CMN_SOUND_VOLUME_LEVEL_DEFAULT);
    }

    // テキストファイルをエディターで開く(デバッグ用)
    static void OpenTextFileByEditor( const char* path , TbFilePathId pathId );

    // ベクトル取得
    static TbVector2 GetDirVector(CmnDir dir)
    {
        static const TbVector2 TABLE[] =
        {
            TbVector2( 0.0f,-1.0f) , // CMN_DIR_UP ,
            TbVector2( 0.0f, 1.0f) , // CMN_DIR_DOWN,
            TbVector2(-1.0f, 0.0f) , // CMN_DIR_LEFT,
            TbVector2( 1.0f, 0.0f) , // CMN_DIR_RIGHT,
        };
        TbVector2 result;
        if (dir < CMN_DIR_MAX)
        {
            result = TABLE[dir];
        }
        return result;
    }

};

#endif
