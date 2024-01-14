/*!
 * プレイヤー用磁石
 * @author teco
 */

#include "fwd.h"
#include "side_action_player_plane.h"
#include "side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include <crayon/map/cr_action_map.h>
#include "common/cmn_util.h"
#include <base/math/tb_random.h>
#include "root.h"


//////////////////////////////////////////
// SideActionPlayerPlane
//////////////////////////////////////////

static const TbRectF32Pod SPRITE_RECT =
{
    -32.0f , -62.0f , 64.0f , 62.0f
};

static const TbRectF32Pod SPRITE_UV_RECT =
{
    CMN_TEX_UV_RECT(1.0f+32.0f,1.0f+64.0f,31.0f,31.0f)
};

/////////////////////////////
// SideActionPlayerPlane
/////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerPlane::SideActionPlayerPlane( SideActionPlayer* player )
    : Super()
    , m_Player(player)
{
    m_Player->Retain();
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
    sprite.SetMaterial(&m_Material);
    m_Material.SetColor(CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex()));
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerPlane::~SideActionPlayerPlane()
{
    TB_SAFE_RELEASE(m_Player);
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionPlayerPlane::OnUpdateAction(TbFloat32 deltatime)
{
    SetPosX(m_Player->GetPosX());
    TbFloat32 z = m_Player->GetMainSprite().GetZ() - 0.005f;
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(z);
}
