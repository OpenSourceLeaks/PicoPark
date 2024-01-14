/*!
 * ワープゾーン
 * @author teco
 * @since 2015.01.25
 */

#ifndef _INCLUDED_SIDE_ACTION_COLLISION_SWITCH_H_
#define _INCLUDED_SIDE_ACTION_COLLISION_SWITCH_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionCollisionSwitch : public ActionActor
{
public:
    
    typedef SideActionCollisionSwitch Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionCollisionSwitch( TbFloat32 width , TbFloat32 height );

    // デストラクタ
    virtual ~SideActionCollisionSwitch();

public:

    // ターゲット設定
    void SetTagetName( const char* name ) {
        m_TargetName = name;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 描画
    virtual void OnDraw( TbFloat32 deltatime ) TB_OVERRIDE;

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

private:

    TbBool              m_IsEnd;
    TbStaticString64    m_TargetName;

};

#endif
