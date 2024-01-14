/*!
 * 共通ユーティリティ関数軍
 * @author teco
 */

#include "fwd.h"
#include "cmn_util.h"
#include <base/system/tb_process.h>
#include <base/io/tb_file_manager.h>

namespace
{
    const TbColor COLOR_TABLE[] = 
    {
        TbColor::FromRgbaU8(127,191,255,255) ,           //
        TbColor::FromRgbaU8(255,140,140,255) ,           //
        TbColor::FromRgbaU8(255,255,140,255) ,         //
        TbColor::FromRgbaU8(168,255,168,255) ,          //
        TbColor::FromRgbaU8(255,183,130,255) ,           //
        TbColor::FromRgbaU8(255,168,255,255) ,        //
        TbColor::FromRgbaU8(207,159,255,255) ,           //
        TbColor::FromRgbaU8(224,224,224,255) ,         //
        TbColor::FromRgbaU8(207,183,159,255) ,          //
        TbColor::FromRgbaU8(255,255,255,255) ,        //
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
        TbUint32 index = CrInput::GetPlayerPadIndex(playerIndex);
        return COLOR_TABLE[index];
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

// 入力ID補正
CrInput::Button CmnUtil::CorrectButton(CrInput::Button button)
{
    CrInput::Button result = button;
    switch (button)
    {
    case CrInput::BUTTON_JUMP:
    case CrInput::BUTTON_DECIDE:
        result = CrInput::BUTTON_CANCEL;
        break;
    case CrInput::BUTTON_CANCEL:
        result = CrInput::BUTTON_JUMP;
        break;
    }
    return result;
}

// テキストファイルをエディターで開く(デバッグ用)
void CmnUtil::OpenTextFileByEditor(const char* path , TbFilePathId pathId)
{
#if TB_PLATFORM_IS_WIN
    TbStaticString512 fullPath = TbFileManager::GetInstance().GetRootPath(pathId);
    fullPath += "/";
    fullPath += path;
    const char* appPath = "\"C:\\Program Files (x86)\\sakura\\sakura.exe\"";
    TbProcess::Start(appPath, fullPath.GetBuffer());
#endif
}

