/*!
 * スプライト2D一括描画
 * @author teco
 * @since 2011.12.07
 */

#include "tb_fwd.h"
#include "graphics/sprite/tb_sprite.h"
#include "graphics/sprite/tb_sprite_scene.h"
#include "graphics/sprite/tb_sprite_scene_hooker.h"
#include "graphics/sprite/tb_sprite_material.h"
#include "base/algorithm/tb_sort.h"

namespace toybox
{

struct TbSpriteScene::CompareFunc
{
    // 値が大きい順にソートするため
    bool operator()( const TbSpriteScene::Node& left , const TbSpriteScene::Node& right ) const {
        return left.GetZ() > right.GetZ();
    }
};

/*!
 * ノードの奥行き取得
 * @author teco
 * @since 2013.09.25
 */
TbFloat32 TbSpriteScene::Node::GetZ() const
{
    return type == TYPE_SPRITE ? sprite->GetZ() : hooker->GetZ();
}

/*!
 * ノードのフレーム進行
 * @author teco
 * @since 2013.09.25
 */
void TbSpriteScene::Node::StepFrame( TbFloat32 frame )
{
    type == TYPE_SPRITE ? sprite->StepFrame(frame) : hooker->StepFrame(frame);
}

/*!
 * ノードの計算
 * @author teco
 * @since 2013.09.25
 */
void TbSpriteScene::Node::Calculate()
{
    type == TYPE_SPRITE ? sprite->Calculate() : hooker->Calculate();
}

/*!
 * 描画
 * @author teco
 * @since 2013.09.25
 */
void TbSpriteScene::Node::Draw( const TbMatrix& mtx , TbSpriteMaterial* material )
{
    type == TYPE_SPRITE ? sprite->Draw(mtx,material) : hooker->Draw(mtx,material);
}


/*!
 * コンストラクタ
 * @author teco
 * @since 2012.01.02
 */
TbSpriteScene::TbSpriteScene( TbSizeT spriteMax )
    : m_Material(nullptr)
    , m_SpriteList(spriteMax)
{
    m_PoseMatrix.Unit();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2012.01.02
 */
TbSpriteScene::~TbSpriteScene()
{
    TB_ASSERT(m_SpriteList.IsEmpty());
}

/*!
 * 最大スプライト数設定
 * @author teco
 * @since 2012.01.02
 */
void TbSpriteScene::SetCapacity( TbSizeT spriteMax )
{
    m_SpriteList.SetCapacity(spriteMax);
}

/*!
 * 全スプライトのフレーム更新
 * @author teco
 * @since 2013.07.04
 */
void TbSpriteScene::Step( TbFloat32 frame )
{
    for( TbArray<Node>::Iterator ite = m_SpriteList.Begin() , endIte = m_SpriteList.End();
         ite != endIte;
         ++ite )
    {
        ite->StepFrame(frame);
    }
}

/*!
 * フレームを進める
 * @author teco
 * @since 2013.07.04
 */
void TbSpriteScene::Calculate()
{
    // ソート
    TbInsertionSort(m_SpriteList.Begin(),m_SpriteList.End(),CompareFunc());
    // 計算
    for( TbArray<Node>::Iterator ite = m_SpriteList.Begin() , endIte = m_SpriteList.End();
         ite != endIte;
         ++ite )
    {
        ite->Calculate();
    }
}

/*!
 * 描画
 * @author teco
 * @since 2012.01.02
 */
void TbSpriteScene::Draw()
{
    if( m_Material ){
        m_Material->BeginDraw();
    }
    for( TbArray<Node>::Iterator ite = m_SpriteList.Begin() , endIte = m_SpriteList.End();
         ite != endIte;
         ++ite )
    {
        ite->Draw(m_PoseMatrix,m_Material);
    }
    if( m_Material ){
        m_Material->EndDraw();
    }
}

/*!
 * スプライト追加
 * @author teco
 * @since 2012.01.02
 */
TbResult TbSpriteScene::addSprite( TbSprite* sprite )
{
    if( !sprite ) {
        return TB_E_FAIL;
    }
    Node node;
    node.type = Node::TYPE_SPRITE;
    node.sprite = sprite;
    m_SpriteList.PushBack(node);
    return TB_S_OK;
}

/*!
 * スプライト除去
 * @author teco
 * @since 2012.01.02
 */
TbResult TbSpriteScene::removeSprite( TbSprite* sprite )
{
    if( !sprite ) {
        return TB_E_FAIL;
    }
    TbArray<Node>::Iterator ite = TbFind(m_SpriteList.Begin(),m_SpriteList.End(),sprite);
    if( ite != m_SpriteList.End() ){
        m_SpriteList.Erase(ite);
    }
    return TB_S_OK;
}

/*!
 * スプライト追加
 * @author teco
 * @since 2012.01.02
 */
TbResult TbSpriteScene::addHooker( TbSpriteSceneHooker* hooker )
{
    if( !hooker ) {
        return TB_E_FAIL;
    }
    Node node;
    node.type = Node::TYPE_HOOKER;
    node.hooker = hooker;
    m_SpriteList.PushBack(node);
    return TB_S_OK;
}

/*!
 * スプライト除去
 * @author teco
 * @since 2012.01.02
 */
TbResult TbSpriteScene::removeHooker( TbSpriteSceneHooker* hooker )
{
    if( !hooker ) {
        return TB_E_FAIL;
    }
    TbArray<Node>::Iterator ite = TbFind(m_SpriteList.Begin(),m_SpriteList.End(),hooker);
    if( ite != m_SpriteList.End() ){
        m_SpriteList.Erase(ite);
    }
    return TB_S_OK;
}

}
