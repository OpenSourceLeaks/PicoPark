/*!
 * 子供コンポーネント
 * @author teco
 */

#include "tb_fwd.h"
#include "crayon/actor/cr_action_actor_child.h"
#include "crayon/actor/cr_action_actor.h"

using namespace toybox;

namespace crayon
{

/*! 
 * シーンに入った
 * @author teco
 */
void CrActionActorChild::OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
    if( m_Child ) {
        m_InitPos = m_Child->GetInitPos();
        toybox::TbVector2 p = m_InitPos;
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
void CrActionActorChild::OnExit() {
    if( m_Child ) {
        m_Child->Exit();
        m_Child->SetInitPos(m_InitPos);
    }
}

}
