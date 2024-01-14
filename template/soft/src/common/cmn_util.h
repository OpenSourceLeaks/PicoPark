/*!
 * 共通ユーティリティ関数軍
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CMN_UTIL_H_
#define _INCLUDED_CMN_UTIL_H_

#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector2.h>

class CmnUtil
{
public:

    // プレイヤーカラー取得
    static TbColor GetPlayerColor( TbUint32 playerIndex );

};

#endif
