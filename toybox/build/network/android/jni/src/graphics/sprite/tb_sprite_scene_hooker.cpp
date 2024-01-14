/*!
 * スプライトシーンの処理をフックするクラス
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#include <tb_fwd.h>
#include "graphics/sprite/tb_sprite_scene_hooker.h"
#include "graphics/sprite/tb_sprite_scene.h"
#include "graphics/sprite/tb_sprite_material.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.09.27
 */
TbSpriteSceneHooker::TbSpriteSceneHooker()
    : m_Z(0.0f)
    , m_IsVisible(TB_TRUE)
    , m_Material(nullptr)
    , m_Scene(nullptr)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.09.27
 */
TbSpriteSceneHooker::~TbSpriteSceneHooker()
{
    Exit();
}

/*!
 * シーンに入る
 * @author Miyake Shunsuke
 * @since 2013.09.27
 */
void TbSpriteSceneHooker::Enter( TbSpriteScene* scene )
{
    if( !scene || m_Scene ) {
        return;
    }
    scene->addHooker(this);
    m_Scene = scene;
}

/*!
 * シーンから解除
 * @author Miyake Shunsuke
 * @since 2013.09.27
 */
void TbSpriteSceneHooker::Exit()
{
    if( m_Scene ){
        m_Scene->removeHooker(this);
        m_Scene = nullptr;
    }
}

/*!
 * 描画シーンから解除
 * @author Miyake Shunsuke
 * @since 2013.09.27
 */

TbResult TbSpriteSceneHooker::Draw(const TbMatrix& poseMatrix, TbSpriteMaterial* defaultMaterial)
{
    if (m_Material) {
        // 自前指定のマテリアルがあれば自前描画
        if (defaultMaterial) {
            defaultMaterial->EndDraw();
        }
        if (m_DrawFunc.IsValid() && m_IsVisible) {
            m_DrawFunc(poseMatrix, m_Material);
        }
        if (defaultMaterial) {
            defaultMaterial->BeginDraw();
        }
    }
    else {
        if (m_DrawFunc.IsValid() && m_IsVisible) {
            m_DrawFunc(poseMatrix, defaultMaterial);
        }
    }
    return TB_S_OK;
}


}
