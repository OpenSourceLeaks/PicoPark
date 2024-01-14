/*!
 * 子供コンポーネント
 * @author teco
 */

#include "fwd.h"
#include "common/actor/action_actor_child.h"
#include "common/actor/action_actor.h"

/*! 
 * シーンに入った
 * @author teco
 */
void ActionActorChild::OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
    if( m_Child ) {
        m_InitPos = m_Child->GetInitPos();
        TbVector2 p = m_InitPos;
        p.Add( pos.GetX() , pos.GetY() );
        m_Child->SetStage(stage);
        m_Child->SetInitPos( p );
        m_Child->Enter(&stage->GetActorScene(),stage);
    }
}

/*! 
 * シーンから出た
 * @author teco
 */
void ActionActorChild::OnExit() {
    if( m_Child ) {
        m_Child->Exit();
        m_Child->SetInitPos(m_InitPos);
    }
}
