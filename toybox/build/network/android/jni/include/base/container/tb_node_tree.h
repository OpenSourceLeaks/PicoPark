/*!
 * ノードツリー
 * @author Miyake Shunsuke
 * @since 2012.01.20
 */

#ifndef _INCLUDED_TB_NODE_TREE_H_
#define _INCLUDED_TB_NODE_TREE_H_

#define TB_NODE_TREE_NODE_COUNT_MIN 1

#include <base/string/tb_static_string.h>
#include <base/algorithm/tb_search.h>
#include <base/crypt/tb_crc32.h>

namespace toybox
{

#define TB_NODE_TREE_INDEX_INVALID (0xFFFFFFFF)

template< typename NodeBodyType , 
          typename ConstructParamType >
class TbNodeTree
{
public:
    
    // ノード
    class Node
    {
        friend class TbNodeTree;
    public:
        Node() 
            : m_Index(0)
            , m_Name()
            , m_Hash(0)
            , m_Body()
            , m_Parent(NULL)
            , m_Head(NULL)
            , m_Tail(NULL)
            , m_Next(NULL)
        {}

        ~Node() {}
    public:
        // インデックス
        TbUint32 GetIndex() const {
            return m_Index;
        }
        // 名前
        const TbStaticString16& GetName() const {
            return m_Name;
        }
        // ハッシュ
        const TbUint32 GetHash() const {
            return m_Hash;
        }
        // 実体
        NodeBodyType& GetBody() {
            return m_Body;
        }
        // 実体
        const NodeBodyType& GetBody() const {
            return m_Body;
        }
        // 親
        Node* GetParent() {
            return m_Parent;
        }
        // 長子
        Node* GetHead() {
            return m_Head;
        }
        // 長子
        const Node* GetHead() const{
            return m_Head;
        }
        // 末子
        Node* GetTail() {
            return m_Tail;
        }
        // 末子
        const Node* GetTail() const {
            return m_Tail;
        }
        // 次
        Node* GetNext() {
            return m_Next;
        }
        // 次
        const Node* GetNext() const {
            return m_Next;
        }
    private:
        TbUint32            m_Index;      // インデックス
        TbStaticString16    m_Name;       // 名前
        TbUint32            m_Hash;       // 検索用ハッシュ値
        NodeBodyType        m_Body;       // 中身
        Node*               m_Parent;     // 親
        Node*               m_Head;       // 頭
        Node*               m_Tail;       // 尻尾
        Node*               m_Next;       // 次        
    };
    
    // ノード初期化パラメータ
    struct InitNodeParam
    {
        InitNodeParam() : parentIndex(static_cast<TbUint32>(-1)),name(NULL),param() {}
        TbUint32            parentIndex; // 親インデックス
        const char*         name;        // 名前
        ConstructParamType  param;       // 初期化パラメータ
    };

public:
    
    // コンストラクタ
    TbNodeTree();

    // コンストラクタ
    TbNodeTree( TbUint32 nodeCount , const InitNodeParam* paramList );
    
    // デストラクタ
    ~TbNodeTree();

public:
    
    // 構築完了か
    TbBool IsReady() const {
        return m_BitArray.Test( FLAG_READY );
    }
    
    // 初期化
    template<TbSizeT COUNT>
    TbResult Initialize( const InitNodeParam (&paramList)[COUNT] ) {
        return Initialize( COUNT , paramList );
    }

    // 初期化 ( nodeCount個の配列でなければならない。サイズチェックはしないので注意 )
    TbResult Initialize( TbUint32 nodeCount , const InitNodeParam* paramList );

    // 後始末
    void Finalize();
    
    // ルートノード取得
    const Node& GetRootNode() const {
        return m_RootNode;
    }
    
    // ルートノード取得
    Node& GetRootNode() {
        return m_RootNode;
    }

    // ノード取得
    const Node* GetNode( const char* name ) const;
    
    // ノード取得
    const Node* GetNode( TbUint32 index ) const;

    // ノード取得
    Node* GetNode( const char* name );

    // ノード取得
    Node* GetNode( TbUint32 index );

    // ノードインデックス取得
    TbUint32 GetNodeIndex( const char* name ) const;

    // ノード数取得
    TbUint32 GetNodeCount() const {
        return m_NodeCount;
    }

private:
    
    enum
    {
        FLAG_READY , 
    };

    struct Getter
    {
        TbUint32 operator()( Node* node ){
            return node->GetHash();
        }
    };

private:

    TbBitArray32    m_BitArray;         // ビット配列
    TbUint32        m_NodeCount;        // ノード数
    Node            m_RootNode;         // ルートノード
    Node*           m_ChildNodeBuffer;  // 子ノード用バッファ
    Node**          m_SortedNodeList;   // ソート済みノード配列

};

}

#include "./tb_node_tree_inl.h"

#endif
