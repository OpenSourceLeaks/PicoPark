/*!
 * アクターシーンクラス
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */

#include "tb_fwd.h"
#include "base/actor/tb_actor_scene.h"
#include "base/actor/tb_actor.h"

namespace toybox
{

/*!
 * シーンに登録
 * @author Miyake Shunsuke
 * @since 2013.07.10
 */
void TbActor::Enter( TbActorScene* scene , TbUint32 layerIndex ,  void* userData )
{
    if( TB_VERIFY(!m_Scene) ) {
        scene->addActor(this,layerIndex);
        m_LayerIndex = layerIndex;
        OnEnter(userData);
    }
}

/*!
 * シーンから解除
 * @author Miyake Shunsuke
 * @since 2013.07.10
 */
void TbActor::Exit()
{
    m_BitArray.SetBit(FLAG_EXIT);
}

/*!
 * 即時に解除
 * @author Miyake_Shunsuke
 * @since 2014.08.02
 */
void TbActor::exitForce()
{
    if( m_Scene ) {
        OnExit();
        m_Scene->removeActor(this);
    }
}


}
