/*!
 * アクション用コンポーネント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_ACTOR_COMPONENT_H_
#define _INCLUDED_ACTION_ACTOR_COMPONENT_H_

#include <base/math/tb_vector2.h>
#include "common/stage/action_stage.h"

class ActionActorComponent
{
public:
    ActionActorComponent() : m_Owner(nullptr) {}
    virtual ~ActionActorComponent() {}
private:
    TB_DISALLOW_COPY_AND_ASSIGN(ActionActorComponent);
public:
    // オーナー設定
    void SetOwner( ActionActor* owner ) {
        m_Owner = owner;
    }
    // オーナー取得
    ActionActor* GetOwner() {
        return m_Owner;
    }
    // オーナー取得
    const ActionActor* GetOwner() const {
        return m_Owner;
    }
public:
    // 位置更新
    virtual void OnUpdatePos( const TbVector2& pos ) {}
    // シーンに入った
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {}
    // シーンから出た
    virtual void OnExit() {}
    // 速度が設定された
    virtual void OnSetVelo( TbVector2* velo ) {}
private:
    ActionActor*    m_Owner;    // オーナー
};

#endif
