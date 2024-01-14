/*!
 * 子供コンポーネント
 * @author teco
 */

#ifndef _INCLUDED_ACTION_ACTOR_CHILD_H_
#define _INCLUDED_ACTION_ACTOR_CHILD_H_

#include "common/actor/action_actor_component.h"

class ActionActorChild : public ActionActorComponent
{
public:
    ActionActorChild()
        : m_Child(nullptr)
    {
    }
private:
    TB_DISALLOW_COPY_AND_ASSIGN(ActionActorChild);
public:
    // アクター設定
    void SetActor( ActionActor* actor ) {
        m_Child = actor;
    }
    // アクター取得
    ActionActor* GetActor() {
        return m_Child;
    }
    // アクター取得
    const ActionActor* GetActor() const {
        return m_Child;
    }
public:
    // シーンに入った
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle );
    // シーンから出た
    virtual void OnExit();
private:
    TbVector2    m_InitPos;
    ActionActor* m_Child;
};

#endif
