/*!
 * プレイヤー用磁石
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_GUARD_H_
#define _INCLUDED_ACTION_PLAYER_GUARD_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/actor/side_action_player.h"

class SideActionPlayer;

// ワープ銃
class SideActionPlayerGuard : public CrActionActor
{
public:

    typedef SideActionPlayerGuard Self;
    typedef CrActionActor Super;

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // コンストラクタ
    SideActionPlayerGuard( SideActionPlayer* player , CmnDir dir );

    // デストラクタ
    virtual ~SideActionPlayerGuard();

public:

    // プレイヤー番号取得
    TbUint32 GetPlayerIndex() const {
        return m_Player->GetPlayerIndex();
    }

protected:

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    // 衝突時コールバック
    void onCollided(CrActionCollision*);

    TbUint32 getSpriteIndex() const
    {
        TbUint32 TABLE[] =
        {
            1 ,
            1 , 
            0 , 
            0 ,
        };
        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(TABLE)==CMN_DIR_MAX);
        return TABLE[m_Dir];
    }

private:

    TbBitArray32            m_BitArray;
    SideActionPlayer*       m_Player;
    CmnSpritePlayerMaterial m_Material;
    CmnDir                  m_Dir;

};

#endif
