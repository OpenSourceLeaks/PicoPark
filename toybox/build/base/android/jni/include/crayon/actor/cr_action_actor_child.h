/*!
 * 子供コンポーネント
 * @author teco
 */

#ifndef _INCLUDED_CR_ACTION_ACTOR_CHILD_H_
#define _INCLUDED_CR_ACTION_ACTOR_CHILD_H_

#include <crayon/actor/cr_action_actor_component.h>

namespace crayon
{

class CrActionActorChild : public CrActionActorComponent
{
public:
    CrActionActorChild()
        : m_Child(nullptr)
    {
    }
private:
    TB_DISALLOW_COPY_AND_ASSIGN(CrActionActorChild);
public:
    // アクター設定
    void SetActor( CrActionActor* actor ) {
        m_Child = actor;
    }
    // アクター取得
    CrActionActor* GetActor() {
        return m_Child;
    }
    // アクター取得
    const CrActionActor* GetActor() const {
        return m_Child;
    }
public:
    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle );
    // シーンから出た
    virtual void OnExit();
private:
    toybox::TbVector2    m_InitPos;
    CrActionActor* m_Child;
};

}

#endif
