/*!
 * ジャンプエリア
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_JUMP_AREA_H_
#define _INCLUDED_SIDE_ACTION_JUMP_AREA_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionJumpArea : public CrActionActor
{
public:
    
    typedef SideActionJumpArea Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionJumpArea();

    // デストラクタ
    virtual ~SideActionJumpArea();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // 移動先
    void SetJumpVelo( const TbVector2& velo ) {
        m_JumpVelo = velo;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbVector2 m_JumpVelo;

};

#endif
