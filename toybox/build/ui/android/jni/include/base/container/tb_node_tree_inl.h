/*!
 * ノードツリー実装部分
 * @author Miyake Shunsuke
 * @since 2012.01.15
 */

#include <base/util/tb_adapter_iterator.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.01.17
 */
template< typename NodeBodyType , typename ConstructParamType >
TbNodeTree<NodeBodyType,ConstructParamType>::TbNodeTree()
    : m_NodeCount(0U)
    , m_RootNode()
    , m_ChildNodeBuffer(nullptr)
    , m_SortedNodeList(nullptr)
{
}

/*!
 * コンストラクタ
 * @param nodeCount ノード数
 * @param paramList ノード別初期化パラメータ
 * @author Miyake Shunsuke
 * @since 2012.01.17
 */
template< typename NodeBodyType , typename ConstructParamType >
TbNodeTree<NodeBodyType,ConstructParamType>::TbNodeTree( TbUint32 nodeCount , const InitNodeParam* paramList )
    : m_NodeCount(0U)
    , m_RootNode()
    , m_ChildNodeBuffer(nullptr)
    , m_SortedNodeList(nullptr)
{
    Initialize(nodeCount,paramList);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.01.17
 */
template< typename NodeBodyType , typename ConstructParamType >
TbNodeTree<NodeBodyType,ConstructParamType>::~TbNodeTree()
{
    Finalize();
}

/*!
 * 初期化
 * @param nodeCount ノード数
 * @param paramList ノード別初期化パラメータ(( サイズはnodeCountと同じでなければならない)
 * @author Miyake Shunsuke
 * @since 2012.01.17
 */
template< typename NodeBodyType , typename ConstructParamType >
TbResult TbNodeTree<NodeBodyType,ConstructParamType>::Initialize( TbUint32 nodeCount , const InitNodeParam* paramList )
{
    if( IsReady() ){
        return TB_E_FAIL;
    }
    if( !TB_VERIFY(nodeCount>0) ){
        return TB_E_FAIL;
    }
    if( nodeCount > 1 ){
        m_ChildNodeBuffer = TB_NEW Node[nodeCount-1];
        m_SortedNodeList = TB_NEW Node*[nodeCount];
    }
    m_NodeCount = nodeCount;

    // ルート設定 ( ルートは親を持たない)
    m_RootNode.m_Index = 0;
    m_RootNode.m_Name = paramList[0].name;
    m_RootNode.m_Hash = TbCrc32::GetHash( paramList[0].name , 
                                        TbStrCharCount(paramList[0].name) );
    m_RootNode.m_Body.Initialize(paramList[0].param);
    if( m_SortedNodeList ){
        m_SortedNodeList[0] = &m_RootNode;
    }

    // 子ノード設定
    Node* parent = NULL;
    for( TbUint32 i = 1; i < m_NodeCount; ++i ){
        
        Node& node = m_ChildNodeBuffer[i-1];
        node.m_Index = i;
        node.m_Name = paramList[i].name;
        node.m_Body.Initialize(paramList[i].param);
        node.m_Hash = TbCrc32::GetHash( paramList[i].name , TbStrCharCount(paramList[i].name) );

        // 親ノード設定
        if( TB_VERIFY( paramList[i].parentIndex < m_NodeCount ) ){
            parent = GetNode( paramList[i].parentIndex );
            node.m_Parent = parent;
            if( parent->m_Head ){
                parent->m_Tail->m_Next = &node;
                parent->m_Tail = &node;
            }else{
                parent->m_Head = &node;
                parent->m_Tail = &node;
            }
        }

        // 検索用配列に格納
        Getter getter;
        TbSint32 insertIndex = TbUpperBoundIndex( TbGetAdapterIterator<TbUint32>(&m_SortedNodeList[0],getter) , 
                                                  TbGetAdapterIterator<TbUint32>(&m_SortedNodeList[i],getter) , 
                                                  node.m_Hash );
        for( TbSint32 index = static_cast<TbSint32>(i-1); index >= insertIndex; --index ){
            m_SortedNodeList[index+1] = m_SortedNodeList[index];
        }
        m_SortedNodeList[insertIndex] = &node;
    }
    
    m_BitArray.SetBit( FLAG_READY , TB_TRUE );
    return TB_S_OK;
}

/*!
 * 後始末
 * @author teco
 */
template< typename NodeBodyType , typename ConstructParamType >
void TbNodeTree<NodeBodyType,ConstructParamType>::Finalize()
{
    TB_SAFE_DELETE_ARRAY(m_ChildNodeBuffer);
    TB_SAFE_DELETE_ARRAY(m_SortedNodeList);
    m_NodeCount = 0;
    m_BitArray.Clear();
}

/*!
 * ノード取得
 * @author Miyake Shunsuke
 * @since 2012.01.17
 */
template< typename NodeBodyType , typename ConstructParamType >
const typename TbNodeTree<NodeBodyType,ConstructParamType>::Node* TbNodeTree<NodeBodyType,ConstructParamType>::GetNode( const char* name ) const
{
    return GetNode(GetNodeIndex(name));
}

/*!
 * ノード取得
 * @author Miyake Shunsuke
 * @since 2012.01.17
 */
template< typename NodeBodyType , typename ConstructParamType >
const typename TbNodeTree<NodeBodyType,ConstructParamType>::Node* TbNodeTree<NodeBodyType,ConstructParamType>::GetNode( TbUint32 index ) const
{
    if( index >= m_NodeCount ){
        return NULL;
    }
    return ( 0 == index ) ? &m_RootNode : &m_ChildNodeBuffer[index-1];
}

/*!
 * ノード取得
 * @author Miyake Shunsuke
 * @since 2012.02.16
 */
template< typename NodeBodyType , typename ConstructParamType >
typename TbNodeTree<NodeBodyType,ConstructParamType>::Node* TbNodeTree<NodeBodyType,ConstructParamType>::GetNode( const char* name )
{
    return GetNode(GetNodeIndex(name));
}

/*!
 * ノード取得
 * @author Miyake Shunsuke
 * @since 2012.02.16
 */
template< typename NodeBodyType , typename ConstructParamType >
typename TbNodeTree<NodeBodyType,ConstructParamType>::Node* TbNodeTree<NodeBodyType,ConstructParamType>::GetNode( TbUint32 index )
{
    if( index >= m_NodeCount ){
        return NULL;
    }
    return ( 0 == index ) ? &m_RootNode : &m_ChildNodeBuffer[index-1];
}

/*!
 * ノードインデックス取得
 * @author Miyake Shunsuke
 * @since 2012.02.16
 */
template< typename NodeBodyType , typename ConstructParamType >
TbUint32 TbNodeTree<NodeBodyType,ConstructParamType>::GetNodeIndex( const char* name ) const
{
    TbUint32 result = TB_NODE_TREE_INDEX_INVALID;
    TbUint32 hash = TbCrc32::GetHash( name , TbStrCharCount(name) );
    // ハッシュ値に変えて二部探索で検索
    if( m_SortedNodeList ){
        Getter getter;
        TbSint32 index = TbBinarySearchIndex( TbGetAdapterIterator<TbUint32>(&m_SortedNodeList[0],getter) , 
                                              TbGetAdapterIterator<TbUint32>(&m_SortedNodeList[m_NodeCount],getter) , 
                                              hash );
        if( index >= 0 ){
            result = static_cast<TbUint32>(index);
        }
    }else{
        if( m_RootNode.m_Hash == hash ){
            result = 0U;
        }
    }
    return result;
}

}