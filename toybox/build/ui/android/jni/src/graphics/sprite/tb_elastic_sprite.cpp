/*!
 * スプライトのアニメーション管理クラス
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#include "tb_fwd.h"
#include "graphics/sprite/tb_elastic_sprite.h"
#include "graphics/sprite/tb_sprite_material.h"
#include "graphics/sprite/tb_sprite_scene.h"

namespace toybox
{

#define NODE_INDEX(place) ((place)+1)

static const TbFloat32 SCALE_MIN = 1.2f;

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.11.25
 */
TbElasticSprite::TbElasticSprite()
    : Super()
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.11.25
 */
TbElasticSprite::~TbElasticSprite()
{
}

/*!
 * ノードツリー設定
 * @author Miyake_Shunsuke
 * @since 2014.11.25
 */
TbResult TbElasticSprite::Initialize( const InitParam& param )
{
    // 横
    if( !TB_VERIFY(param.basePlaceSize.width < param.rect.width) ||
        !TB_VERIFY(param.basePlaceSize.height < param.rect.height) )
    {
        return TB_E_INVALIDARG;
    }

    static const char* NODE_NAME_TABLE[] =
    {
        "lu" , 
        "lc" , 
        "lb" , 

        "up" , 
        "in" , 
        "bt" , 

        "ru" , 
        "rc" ,
        "rb" ,
    };
    m_BasePlaceSize = param.basePlaceSize;
    m_Size = TbDimensionF32( param.rect.width , param.rect.height );
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(NODE_NAME_TABLE)==TbElasticSprite::PLACE_MAX);
    TbSpriteNodeTree::InitNodeParam nodeParam[TbElasticSprite::PLACE_MAX+1];
    nodeParam[0].name = "root";
    nodeParam[0].param.isNull = TB_TRUE;

    for( size_t i = 0; i < TbElasticSprite::PLACE_MAX; ++i ){
        size_t index = i+1;
        nodeParam[index].name = NODE_NAME_TABLE[i];
        nodeParam[index].param.rect = param.rect;
        nodeParam[index].param.rect.width = 1.0f;
        nodeParam[index].param.rect.height = 1.0f;
        nodeParam[index].param.pivot.Set(param.rect.x,param.rect.y);
        nodeParam[index].param.scale.Set(param.basePlaceSize.width,param.basePlaceSize.height);
        nodeParam[index].param.uvRect = param.rectUV[i];
        nodeParam[index].param.priority = 0xFF;
        nodeParam[index].parentIndex = 0;
    }
    nodeParam[TbElasticSprite::PLACE_LC].param.priority = 0;
    nodeParam[TbElasticSprite::PLACE_IN].param.priority = 0;
    nodeParam[TbElasticSprite::PLACE_RC].param.priority = 0;

    m_NodeTree.Initialize(nodeParam);
    TbResult result = Super::Initialize(&m_NodeTree);
    if( TB_SUCCEEDED(result) ){
        updateSize();
    }
    return result;
}

/*!
 * 横幅設定
 * @author Miyake_Shunsuke
 * @since 2014.12.01
 */
void TbElasticSprite::SetWidth( TbFloat32 width )
{
    width = TbMax( width , m_BasePlaceSize.width*SCALE_MIN );
    if( !TB_FLOAT32_EQUAL(m_Size.width,width) ){
        m_Size.width = width;
        updateSize();
    }
}

/*!
 * 高さ設定
 * @author Miyake_Shunsuke
 * @since 2014.12.01
 */
void TbElasticSprite::SetHeight( TbFloat32 height )
{
    height = TbMax( height , m_BasePlaceSize.height*SCALE_MIN );
    if( !TB_FLOAT32_EQUAL(m_Size.height,height) ){
        m_Size.height = height;
        updateSize();
    }
}

/*!
 * サイズ設定
 * @author Miyake_Shunsuke
 * @since 2014.12.04
 */
void TbElasticSprite::SetSize( TbFloat32 width , TbFloat32 height )
{
    width = TbMax( width , m_BasePlaceSize.width*SCALE_MIN );
    height = TbMax( height , m_BasePlaceSize.height*SCALE_MIN );
    if( !TB_FLOAT32_EQUAL(m_Size.width,width) || !TB_FLOAT32_EQUAL(m_Size.height,height) ){
        m_Size.width = width;
        m_Size.height = height;
        updateSize();
    }
}

/*!
 * サイズ更新
 * @author Miyake_Shunsuke
 * @since 2014.12.02
 */
void TbElasticSprite::updateSize()
{
    if( !m_NodeTree.IsInitialized() ){
        return;
    }

    TbVector2 center;
    center.Set( TbMin(m_BasePlaceSize.width,m_Size.width-m_BasePlaceSize.width) ,
                TbMin(m_BasePlaceSize.height,m_Size.height-m_BasePlaceSize.height) );
    TbVector2 rb;
    rb.Set( m_Size.width  - m_BasePlaceSize.width  ,
            m_Size.height - m_BasePlaceSize.height );

    TbSpriteNodeTree::Node* nodeLU = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_LU));
    TbSpriteNodeTree::Node* nodeLC = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_LC));
    TbSpriteNodeTree::Node* nodeLB = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_LB));

    TbSpriteNodeTree::Node* nodeUP = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_UP));
    TbSpriteNodeTree::Node* nodeIN = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_IN));
    TbSpriteNodeTree::Node* nodeBT = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_BT));

    TbSpriteNodeTree::Node* nodeRU = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_RU));
    TbSpriteNodeTree::Node* nodeRC = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_RC));
    TbSpriteNodeTree::Node* nodeRB = m_NodeTree.GetNodeFromIndex(NODE_INDEX(PLACE_RB));

    static TbFloat32 CENTER_SCALE_OFS = 2.0f;
    TbFloat32 centerOffset = CENTER_SCALE_OFS/2.0f;

    // 左側
    nodeLC->GetBody().trans.SetY( center.GetY()-(centerOffset));
    nodeLC->GetBody().scale.SetY( (rb.GetY() - center.GetY()) + CENTER_SCALE_OFS);
    nodeLB->GetBody().trans.SetY( rb.GetY() );

    // 真ん中
    nodeUP->GetBody().trans.SetX(  center.GetX() );
    nodeUP->GetBody().scale.SetX( rb.GetX() - center.GetX() );
    nodeIN->GetBody().trans.Set(  center.GetX() , center.GetY() - centerOffset);
    nodeIN->GetBody().scale.Set( rb.GetX() - center.GetX() , 
                                 (rb.GetY() - center.GetY()) + CENTER_SCALE_OFS);
    nodeBT->GetBody().trans.Set( center.GetX() , rb.GetY() );
    nodeBT->GetBody().scale.SetX( rb.GetX() - center.GetX() );

    // 右側
    nodeRU->GetBody().trans.SetX( rb.GetX() );
    nodeRC->GetBody().trans.Set( rb.GetX() , center.GetY() - centerOffset);
    nodeRC->GetBody().scale.SetY( rb.GetY() - center.GetY() + CENTER_SCALE_OFS);
    nodeRB->GetBody().trans.Set( rb.GetX() , rb.GetY() );
}

/*!
 * UV座標のみ設定
 * @author Miyake_Shunsuke
 * @since 2014.12.15
 */
void TbElasticSprite::SetUV( const UvParam& param )
{
    if( !m_NodeTree.IsInitialized() ){
        return;
    }
    for( TbSint32 i = 0; i < PLACE_MAX; ++i ) {
        TbSpriteNodeTree::Node* node = m_NodeTree.GetNodeFromIndex(NODE_INDEX(i));
        if( TB_VERIFY(node) ) {
            node->GetBody().uvRect = param.rectUV[i];
        }
    }
}

}

