/*!
 * スプライト
 * @author Miyake Shunsuke
 * @since 2012.01.15
 */

#ifndef _INCLUDED_TB_SPRITE_NODE_TREE_H_
#define _INCLUDED_TB_SPRITE_NODE_TREE_H_

#include <base/container/tb_node_tree.h>
#include <base/algorithm/tb_sort.h>
#include <graphics/sprite/tb_sprite_types.h>

namespace toybox
{

struct TbSpriteNode
{

    // 初期化パラメータ
    struct InitParam
    {
        InitParam()
            : isNull(TB_FALSE)
            , rect(0.0f,0.0f,0.0f,0.0f)
            , uvRect(0.0f,0.0f,0.0f,0.0f)
            , color(TbColor::White())
            , pivot()
            , trans()
            , rot(0)
            , scale(1.0f,1.0f)
            , priority(0)
        {}
        ~InitParam(){}
        TbBool          isNull;      // nullptrノードか
        TbRectF32       rect;        // 矩形
        TbRectF32       uvRect;      // UV矩形
        TbColor         color;       // 色
        TbVector2       pivot;       // ピボット
        TbVector2       trans;       // 位置
        TbAngle32       rot;         // 回転
        TbVector2       scale;       // スケール
        TbUint32        priority;
    };

    // コンストラクタ
    TbSpriteNode()
        : isNull(TB_FALSE) 
        , rect() 
        , uvRect() 
        , color() 
        , pivot() 
        , trans() 
        , rot() 
        , scale()
        , priority(0)
    {}

    // デストラクタ
    ~TbSpriteNode()
    {}

    // 初期化
    void Initialize( const InitParam& param )
    {
        isNull  = param.isNull;
        rect    = param.rect;
        uvRect  = param.uvRect;
        color   = param.color;
        pivot   = param.pivot;
        trans   = param.trans;
        rot     = param.rot;
        scale   = param.scale;
        priority = param.priority;
    }

    TbBool        isNull;     // nullptrノードか
    TbRectF32     rect;       // 矩形
    TbRectF32     uvRect;     // UV矩形
    TbColor       color;      // カラー
    TbVector2     pivot;      // ピボット
    TbVector2     trans;      // 位置
    TbAngle32     rot;        // 回転
    TbVector2     scale;      // スケール
    TbSint32      priority;   // 描画プライオリティ(高い方が手前)
};

// スプライトノードツリー
typedef TbNodeTree<TbSpriteNode,TbSpriteNode::InitParam> TbSpriteNodeTreeContainer;

class TbSpriteNodeTree
{
public:
    
    typedef TbSpriteNodeTreeContainer Container;
    typedef Container::InitNodeParam  InitNodeParam;
    typedef Container::Node           Node;
    static const TbUint32 PRIORITY_INDEX_INVALID = 0xFFFFFFFF;

public:

    // コンストラクタ
    TbSpriteNodeTree()
        : m_Container()
        , m_PriorityIndex2NodeIndex(nullptr)
    {}

    // デストラクタ
    ~TbSpriteNodeTree()
    {
        Finalize();
    }

public:

    // 初期化
    TbResult Initialize( const InitNodeParam& param ) {
        return Initialize( 1 , &param );
    }

    // 初期化
    template<TbSizeT COUNT>
    TbResult Initialize( const InitNodeParam (&paramList)[COUNT] ) {
        return Initialize( COUNT , paramList );
    }

    // 初期化 ( nodeCount個の配列でなければならない。サイズチェックはしないので注意 )
    TbResult Initialize( TbUint32 nodeCount , const InitNodeParam* paramList ) {
        TbResult result = m_Container.Initialize(nodeCount,paramList);
        if( nodeCount > 1 ){
            m_PriorityIndex2NodeIndex = TB_NEW TbUint32[nodeCount];
            for( TbUint32 i = 0; i < nodeCount; ++i ){
                m_PriorityIndex2NodeIndex[i] = m_Container.GetNode(i)->GetIndex();
            }
            TbInsertionSort(m_PriorityIndex2NodeIndex,m_PriorityIndex2NodeIndex+nodeCount,CompareFunc(m_Container));
        }
        return result;
    }

    // 後始末
    void Finalize()
    {
        m_Container.Finalize();
        TB_SAFE_DELETE_ARRAY(m_PriorityIndex2NodeIndex);
    }

    // 初期化済み
    TbBool IsInitialized() const {
        return m_Container.IsReady();
    }

private:

    // 比較関数
    struct CompareFunc
    {
        CompareFunc( Container& src )
            : container(src)
        {}
        TbBool operator()( TbUint32 leftIndex , TbUint32 rightIndex ) const {
            Container::Node* leftNode = container.GetNode(leftIndex);
            Container::Node* rightNode = container.GetNode(rightIndex);
            if( TB_VERIFY( leftNode && rightNode ) ){
                TbSpriteNode& left = leftNode->GetBody();
                TbSpriteNode& right = rightNode->GetBody();
                return left.priority < right.priority;
            }
            return TB_FALSE;
        }
        Container& container;
    };

public:

    // ルートノード取得
    const Node& GetRootNode() const {
        return m_Container.GetRootNode();
    }
    
    // ルートノード取得
    Node& GetRootNode() {
        return m_Container.GetRootNode();
    }

    // ノード取得
    const Node* GetNode( const char* name ) const {
        return m_Container.GetNode(name);
    }
    
    // ノード取得
    const Node* GetNodeFromIndex( TbUint32 index ) const {
        return m_Container.GetNode(index);
    }

    // ノード取得
    Node* GetNode( const char* name ) {
        return m_Container.GetNode(name);
    }

    // ノード取得
    Node* GetNodeFromIndex( TbUint32 index ) {
        return m_Container.GetNode(index);
    }

    // ノードインデックス取得
    TbUint32 GetNodeIndex( const char* name ) const {
        return m_Container.GetNodeIndex(name);
    }

    // ノード数取得
    TbUint32 GetNodeCount() const {
        return m_Container.GetNodeCount();
    }

    // 優先度が低い順のノード番号
    TbUint32 GetPriorityNodeIndex( TbUint32 index ) {
        if( index < m_Container.GetNodeCount() ) {
            if( 1 == m_Container.GetNodeCount() ){
                return 0;
            }else{
                return m_PriorityIndex2NodeIndex[index];
            }
        }
        return PRIORITY_INDEX_INVALID;
    }

private:

    Container   m_Container;
    TbUint32*   m_PriorityIndex2NodeIndex; // 優先度番号からノード番号への変換配列

};

}

#endif
