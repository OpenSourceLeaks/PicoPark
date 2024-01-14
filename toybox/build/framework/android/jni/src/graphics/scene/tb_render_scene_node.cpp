/*!
 * レンダーシーンノード
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */

#include <tb_fwd.h>
#include "graphics/scene/tb_render_scene_node.h"
#include "graphics/scene/tb_render_scene.h"

namespace toybox
{

/*!
 * シーンに登録
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
void TbRenderSceneNode::Enter( TbRenderScene* scene )
{
    if( TB_VERIFY(m_Scene) ) {
        return;
    }
    m_Scene = scene;
    m_Scene->addNode(this);
}

// シーンから解除
void TbRenderSceneNode::Exit()
{
    if( m_Scene ) {
        m_Scene->removeNode(this);
    }
}

}