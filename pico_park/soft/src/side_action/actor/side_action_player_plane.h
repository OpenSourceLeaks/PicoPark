/*!
 * プレイヤー用磁石
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_PLANE_H_
#define _INCLUDED_ACTION_PLAYER_PLANE_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/actor/side_action_player.h"

class SideActionPlayer;

// ワープ銃
class SideActionPlayerPlane : public CrActionActor
{
public:

    typedef SideActionPlayerPlane Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerPlane( SideActionPlayer* player );

    // デストラクタ
    virtual ~SideActionPlayerPlane();

public:

    // プレイヤー番号取得
    TbUint32 GetPlayerIndex() const {
        return m_Player->GetPlayerIndex();
    }

protected:

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;
private:

    TbBitArray32                                    m_BitArray;
    SideActionPlayer*                               m_Player;
     CmnSpritePlayerMaterial                        m_Material;

};

#endif
