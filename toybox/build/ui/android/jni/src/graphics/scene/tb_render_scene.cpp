/*!
 * レンダーシーン
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */

#include <tb_fwd.h>
#include <graphics/scene/tb_render_scene.h>
#include <graphics/scene/tb_render_scene_node.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
TbRenderScene::TbRenderScene()
    : m_BitArray()
    , m_Top(nullptr)
    , m_RenderPass(0)
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
TbRenderScene::~TbRenderScene()
{
}
    
/*!
 * 更新(アニメーションの更新など)
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
void TbRenderScene::Step( TbFloat32 deltatime )
{
    if( !m_Top ){
        return;
    }
    if( !IsEnableStep() ){
        return;
    }
    TbRenderSceneNode* node = m_Top;
    do {
        node->Step(deltatime);
        node = node->m_Next;
    } while( node != m_Top );
    node = m_Top;
    // 後処理
    do {
        node->Post();
        node = node->m_Next;
    } while( node != m_Top );
}

/*!
 * 描画
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
void TbRenderScene::Draw()
{
    if( !m_Top ){
        return;
    }
    if( !IsEnableDraw() ){
        return;
    }
    TbRenderSceneNode* node = m_Top;
    do {
        node->Draw(m_RenderPass);
        node = node->m_Next;
    } while( node != m_Top );
}

/*!
 * ノード登録
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
void TbRenderScene::addNode( TbRenderSceneNode* node )
{
    TB_ASSERT(!node->m_Scene);
    if( m_Top ) {
        node->m_Next = m_Top;
        node->m_Prev = m_Top->m_Prev;
        m_Top->m_Prev->m_Next = node;
        m_Top->m_Prev  =node;
    }else{
        m_Top = node;
        node->m_Next = node;
        node->m_Prev = node;
    }
    node->m_Scene = this;
}

/*!
 * ノードを除去
 * @author Miyake_Shunsuke
 * @since 2014.05.06
 */
void TbRenderScene::removeNode( TbRenderSceneNode* node )
{
    if( !TB_VERIFY(node->m_Scene == this) ) {
        return;
    }
    if( m_Top == node ) {
        if( node->m_Next == node ) {
            m_Top = nullptr;
        }else{
            node->m_Prev->m_Next = node->m_Next;
            node->m_Next->m_Prev = node->m_Prev;
            m_Top = node->m_Next;
        }
    }else{
        node->m_Prev->m_Next = node->m_Next;
        node->m_Next->m_Prev = node->m_Prev;
    }
    node->m_Prev = nullptr;
    node->m_Next = nullptr;
    node->m_Scene = nullptr;
}

}
