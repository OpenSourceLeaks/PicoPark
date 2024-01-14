/*!
 * ワープオブジェクト
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_WARP_OBJECT_H_
#define _INCLUDED_SIDE_ACTION_WARP_OBJECT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "side_action/side_action_types.h"
#include <base/util/tb_dynamic_cast.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/stage/cr_action_stage.h>

class SideActionWarpObject : public CrActionActor
{
public:

    typedef CrActionActor Super;
    typedef SideActionWarpObject Self;

public:

    // デストラクタ
    virtual ~SideActionWarpObject() {}

public:

    // ワープ
    void RequestWarp(const TbVector2& pos) {
        m_RequestedPos = pos;
    }

    // ワープ座標持っている
    TbBool HasWarpPos() {
        return !TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq());
    }

public:

    // ロジック更新
    virtual void OnPostAction(TbFloat32 deltatime) {
        UpdateWarp();
    }

protected:

    // ワープ更新
    void UpdateWarp()
    {
        // ワープ処理
        if (TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq())) {
            return;
        }
        CrActionCollision* collision = GetCollision();
        TbVector2 backup = collision->GetPos();
        collision->SetPos(m_RequestedPos);
        if (!collision->GetScene()->IsCollidedByPushBackTarget(*collision)) {
            SetPosForce(m_RequestedPos);
            ResetVelo();
            m_RequestedPos.Clear();
        }
        else {
            collision->SetPos(backup);
        }
    }

public:
    
    TbVector2                   m_RequestedPos;

};

#endif
