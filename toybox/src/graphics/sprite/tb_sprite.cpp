/*!
 * スプライトのアニメーション管理クラス
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#include "tb_fwd.h"
#include "graphics/sprite/tb_sprite.h"
#include "graphics/sprite/tb_sprite_material.h"
#include "graphics/sprite/tb_sprite_scene.h"

#define CALL_ANIMATION_FUNC_ARG1(var,func,arg0) \
    var.rect.func(arg0); \
    var.uvOffset.func(arg0); \
    var.translate.func(arg0); \
    var.rot.func(arg0); \
    var.scale.func(arg0); \
    var.color.func(arg0)


namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
TbSprite::TbSprite()
    : m_BitArray()
    , m_NodeCount( 0 )
    , m_NodeInfoList( nullptr )
    , m_NodeTree( nullptr )
    , m_Material( nullptr )
    , m_NodeAnimTree( nullptr )
    , m_Frame(0.0f)
    , m_StepFrameSpeed(1.0f)
    , m_FrameBegin(0.0f)
    , m_FrameEnd(0.0f)
    , m_FrameRateControl(nullptr)
    , m_Z(0.0f)
    , m_Trans()
    , m_Rot(0)
    , m_Scale(1.0f,1.0f)
    , m_Color( TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) )
    , m_Scene(nullptr)
{
    SetVisible(TB_TRUE);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
TbSprite::~TbSprite()
{
    Finalize();
}

/*!
 * ノードツリー設定
 * @param nodeTree ノードツリー
 * @param anim アニメーション
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
TbResult TbSprite::Initialize( TbSpriteNodeTree* nodeTree , TbSpriteNodeAnimTree* anim )
{
    if( !nodeTree ){
        return TB_E_INVALIDARG;
    }
    TbUint32 nodeCount = nodeTree->GetNodeCount();
    if( 0 == nodeCount ){
        return TB_E_INVALIDARG;
    }
    deleteNodeInfoList();

    m_NodeTree = nodeTree;
    m_NodeCount = nodeCount;

    // ノード情報構築
    m_NodeInfoList = TB_NEW Self::NodeInfo[ m_NodeCount ];
    for( TbUint32 i = 0; i < m_NodeCount; ++i ){
        m_NodeInfoList[i].index = i;
    }

    // アニメーション設定
    SetNodeAnimation(anim);

    return TB_S_OK;
}

/*!
 * ノードツリー設定
 * @author Miyake_Shunsuke
 * @since 2014.11.18
 */
TbResult TbSprite::Initialize( const TbRectF32& rect , 
                               const TbRectF32& uvRect )
{
    if( !TB_VERIFY(!m_NodeTree) ){
        return TB_E_DENY;
    }
    m_SingleNodeInfo.rect = rect;
    m_SingleNodeInfo.uvRect = uvRect;
    m_NodeCount = 1;
    m_NodeInfoList = &m_SingleNodeInfo;
    return TB_S_OK;
}

/*!
 * 後始末
 * @author Miyake_Shunsuke
 * @since 2014.11.18
 */
void TbSprite::Finalize()
{
    Exit();
    deleteNodeInfoList();
    m_NodeAnimTree = nullptr;
    m_Frame = 0.0f;
    m_FrameBegin = 0.0f;
    m_FrameEnd = 0.0f;
}

/*!
 * マテリアル設定
 * @param index ノードインデックス
 * @param material マテリアル
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
void TbSprite::SetMaterial( TbUint32 index , TbSpriteMaterial* material )
{
    if( index < m_NodeCount ) {
        m_NodeInfoList[index].material = material;
    }
}

/*!
 * ノードアニメーションツリー設定
 * ツリー構造はノードツリーと同じでなければならない
 * @param anim アニメーションツリー
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
void TbSprite::SetNodeAnimation( TbSpriteNodeAnimTree* anim )
{
    if( !TB_VERIFY(m_NodeInfoList) ){
        return;
    }
    m_NodeAnimTree = anim;
    if( !m_NodeAnimTree ){
        m_Frame = 0.0f;
        m_FrameBegin = 0.0f;
        m_FrameEnd = 0.0f;
    }else if( TB_VERIFY( m_NodeAnimTree->GetNodeCount() == m_NodeTree->GetNodeCount()) ){
        TbSizeT count  = m_NodeAnimTree->GetNodeCount();
        for( TbSizeT i=0;i<count;++i){
            TbSpriteNodeAnimTree::Node* node = m_NodeAnimTree->GetNode(i);
            TbSpriteNodeAnim& animNode = node->GetBody();
            TbUint32 nodeIndex = node->GetIndex();
            m_NodeInfoList[nodeIndex].anim.rect.SetData(&animNode.GetRectData());
            m_NodeInfoList[nodeIndex].anim.uvOffset.SetData(&animNode.GetUVOffsetData());
            m_NodeInfoList[nodeIndex].anim.translate.SetData(&animNode.GetTransData());
            m_NodeInfoList[nodeIndex].anim.rot.SetData(&animNode.GetRotData());
            m_NodeInfoList[nodeIndex].anim.scale.SetData(&animNode.GetScaleData());
            m_NodeInfoList[nodeIndex].anim.color.SetData(&animNode.GetColorData());
#if 0
            m_FrameBegin = TbMin(animNode.GetFrameBegin(),m_FrameBegin);
            m_FrameEnd = TbMax(animNode.GetFrameEnd(),m_FrameEnd);
#else
            m_FrameBegin = animNode.GetFrameBegin();
            m_FrameEnd = animNode.GetFrameEnd();
#endif
        }
        SetFrame(m_FrameBegin);
    }
}

/*!
 * シーンに入る
 * @author Miyake Shunsuke
 * @since 2013.09.08
 */
void TbSprite::Enter( TbSpriteScene* scene )
{
    if( !scene ) {
        return;
    }
    if( m_Scene ) {
        return;
    }
    m_Scene = scene;
    m_Scene->addSprite(this);
}

/*!
 * シーンから解除
 * @author Miyake Shunsuke
 * @since 2013.09.08
 */
void TbSprite::Exit()
{
    if( m_Scene ){
        m_Scene->removeSprite(this);
        m_Scene = nullptr;
    }
}

/*!
 * 回転角度設定
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
void TbSprite::SetRotation( TbAngle32 rot )
{
    m_Rot = rot % (TB_PI_ANG32*2);
}

/*!
 * 直接フレームを進める
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
void TbSprite::StepFrame( TbFloat32 frame )
{
    if( IsEnableStep() ) {
        TbFloat32 step = frame;
        if( m_FrameRateControl ){
            step = m_FrameRateControl->GetStepFrame(frame);
        }
        step *= m_StepFrameSpeed;
        SetFrame(m_Frame + step);
    }
}

/*!
 * 直接フレーム設定
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
void TbSprite::SetFrame( TbFloat32 frame )
{
    if( m_NodeAnimTree ){
        m_Frame = TbMax( frame , m_FrameBegin );
        if( m_Frame > m_FrameEnd ) {
            if( IsEnableLoop() ){
                if( TB_FLOAT32_EQUAL(m_FrameEnd,0.0f) ) {
                    m_Frame = m_FrameBegin;
                }else{
                    // ループ再生
                    while( m_Frame > m_FrameEnd ){
                        m_Frame -= m_FrameEnd;
                    }
                }
            }else{
                m_Frame = m_FrameEnd;
            }
        }
        m_BitArray.SetBit(FLAG_UPDATE_ANIM,TB_TRUE);
    }
    // 座標計算はCalculateで
}

#if 1
/*!
 * 座標計算
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
TbResult TbSprite::Calculate()
{
    TbResult result = TB_E_FAIL;
    // 基準解像度に対してE行う
    TbMatrix rootMatrix;
    rootMatrix.Translate( m_Trans.GetX() , m_Trans.GetY() , 0.0f );
    rootMatrix.RotateZ( m_Rot );
    rootMatrix.Scale( m_Scale.GetX() , m_Scale.GetY() , 1.0f );

    if( m_NodeTree ){
        TbSpriteNodeTree::Node& node = m_NodeTree->GetRootNode();
        result = calcNode( node , rootMatrix );
        m_BitArray.SetBit(FLAG_UPDATE_ANIM,TB_FALSE);
    }else{
        for( TbSizeT i = 0; i < m_NodeCount; ++i ) {
            // 最終的な変換行列を計算
            m_NodeInfoList[i].trsMatrix = rootMatrix;
            m_NodeInfoList[i].color = m_Color;
        }
    }
    return result;
}

/*!
 * 座標計算
 * @author Miyake Shunsuke
 * @since 2012.02.18
 */
TbResult TbSprite::calcNode( const TbSpriteNodeTree::Node& node , const TbMatrix& parentMatrix )
{
    NodeInfo& info = m_NodeInfoList[node.GetIndex()];
    const TbSpriteNode& body = node.GetBody();

    info.trsMatrix.Unit();

    // 基準解像度に対してスケールを行う
    TbVector2 trans;
    TbAngle32 rot = 0;
    TbVector2 scale;
    TbColorF128 colorF128 = TbColorF128::White();

    if( m_NodeAnimTree ){
        // アニメーション適用(キーフレームが無いものはデフォルト)
        if( m_BitArray.Test(FLAG_UPDATE_ANIM) ){
            CALL_ANIMATION_FUNC_ARG1(info.anim,SetFrame,m_Frame);
        }
        if( !info.anim.rect.IsEmpty() ){
            info.anim.rect.GetCurrentValue(info.rect);
        }else{
            info.rect = body.rect;
        }
        if( !info.anim.uvOffset.IsEmpty() ){
            info.anim.uvOffset.GetCurrentValue(info.uvRect);
            info.uvRect = info.uvRect + body.uvRect;
        }else{
            info.uvRect = body.uvRect;
        }
        if( !info.anim.translate.IsEmpty() ){
            info.anim.translate.GetCurrentValue(trans);
        }else{
            trans = body.trans;
        }
        TbFloat32 rotF32 = 0.0f;
        if( !info.anim.rot.IsEmpty() ){
            info.anim.rot.GetCurrentValue(rotF32);
            rot = TbDegToAng32(rotF32);
        }else{
            rot = body.rot;
        }
        if( !info.anim.scale.IsEmpty() ){
            info.anim.scale.GetCurrentValue(scale);
        }else{
            scale = body.scale;
        }
        if (!info.anim.color.IsEmpty()) {
            info.anim.color.GetCurrentValue(colorF128);
            colorF128 = colorF128 * body.color;
        } else {
            colorF128 = body.color.GetColorU32();
        }
    }else{
        // 通常
        info.rect = body.rect;
        info.uvRect = body.uvRect;
        trans = body.trans;
        rot = body.rot;
        scale = body.scale;
        colorF128 = body.color.GetColorU32();
        colorF128 = colorF128 * body.color;
    }

    // 最終的な変換行列を計算
    info.trsMatrix = info.trsMatrix * parentMatrix;
    info.trsMatrix.Translate( body.pivot.GetX() , body.pivot.GetY() , 0.0f );
    info.trsMatrix.Translate( trans.GetX() , trans.GetY() , 0.0f );
    info.trsMatrix.RotateZ( body.rot );
    info.trsMatrix.Scale( body.scale.GetX() , body.scale.GetY() , 1.0f );
    info.trsMatrix.Translate( -body.pivot.GetX() , -body.pivot.GetY() , 0.0f );

    // 最終的な色計算
    info.color = m_Color * colorF128;

    // 子ノードも計算
    const TbSpriteNodeTree::Node* child = node.GetHead();
    while( child ){
        calcNode( *child , info.trsMatrix );
        child = child->GetNext();
    }
    return TB_S_OK;
}

/*!
 * 描画
 * @author Miyake Shunsuke
 * @since 2012.02.14
 */
TbResult TbSprite::Draw( const TbMatrix& mtx )
{
    if( !IsVisible() ){
        return TB_S_OK;
    }
    if( 0 == m_NodeCount ){
        return TB_E_FAIL;
    }
    if( m_Material ){
        m_Material->BeginDraw();
    }
    // 描画プライオリテイ順に描画
    for( TbUint32 i = 0; i < m_NodeCount; ++i )
    {
        NodeInfo* nodeInfo = nullptr;
        if( m_NodeTree ){
            const TbSpriteNodeTree::Node* node = m_NodeTree->GetNodeFromIndex(m_NodeTree->GetPriorityNodeIndex(i));
            TB_ASSERT(node);
            if( !node->GetBody().isNull ){
                nodeInfo = &m_NodeInfoList[node->GetIndex()];
            }
        }else{
            nodeInfo = &m_NodeInfoList[i];
        }
        if( nodeInfo ){
            if( nodeInfo->material ){
                // 個別マテリアル
                nodeInfo->material->BeginDraw();
                nodeInfo->material->Draw( nodeInfo->rect , nodeInfo->uvRect , nodeInfo->color , mtx * nodeInfo->trsMatrix );
                nodeInfo->material->EndDraw();
                if( m_Material ){
                    // 全体マテリアルに復帰
                    m_Material->BeginDraw();
                }
            }else if( m_Material ){
                m_Material->Draw( nodeInfo->rect , nodeInfo->uvRect , nodeInfo->color , mtx * nodeInfo->trsMatrix );
            }
        }
    }
    if( m_Material ){
        m_Material->EndDraw();
    }
    return TB_S_OK;
}

/*!
 * 外部から強制マテリアル指定で描画
 * @author Miyake Shunsuke
 * @since 2013.03.03
 */
TbResult TbSprite::Draw( const TbMatrix& mtx , TbSpriteMaterial* defaultMaterial )
{
    if( !IsVisible() ){
        return TB_S_OK;
    }
    if( m_Material ) {
        // 自前指定のマテリアルがあれば自前描画
        if( defaultMaterial ) {
            defaultMaterial->EndDraw();
        }
        Draw(mtx);
        if( defaultMaterial ) {
            defaultMaterial->BeginDraw();
        }
        return TB_S_OK;
    }
    if( !defaultMaterial ) {
        return TB_E_FAIL;
    }
    if( 0 == m_NodeCount ){
        return TB_E_FAIL;
    }
    // 描画プライオリテイ順に描画
    for( TbUint32 i = 0; i < m_NodeCount; ++i )
    {
        NodeInfo* nodeInfo = nullptr;
        if( m_NodeTree ){
            const TbSpriteNodeTree::Node* node = m_NodeTree->GetNodeFromIndex(m_NodeTree->GetPriorityNodeIndex(i));
            TB_ASSERT(node);
            if( !node->GetBody().isNull ){
                nodeInfo = &m_NodeInfoList[node->GetIndex()];
            }
        }else{
            nodeInfo = &m_NodeInfoList[i];
        }
        if( nodeInfo ) {
            defaultMaterial->Draw( nodeInfo->rect , nodeInfo->uvRect , nodeInfo->color , mtx * nodeInfo->trsMatrix );
        }
    }
    return TB_S_OK;
}

#endif

}
