/*!
 * 共通ユーティリティ関数軍
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "cmn_util.h"

namespace
{
    const TbColor COLOR_TABLE[] = 
    {
        TbColor::Blue() ,           //
        TbColor::Green() ,          //
        TbColor::Cyan() ,           //
        TbColor::Magenta() ,        //
        TbColor::Yellow() ,         //
        TbColor::Blue() ,           //
        TbColor::Green() ,          //
        TbColor::Cyan() ,           //
        TbColor::Magenta() ,        //
        TbColor::Yellow() ,         //
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
