/*!
 * @file
 * @brief 指定クラス専用の固定長ヒープクラス
 * @author Miyake Shunsuke
 * ------------------------------------------------
 */

#ifndef _INCLUDE_TB_TYPE_HEAP_H_
#define _INCLUDE_TB_TYPE_HEAP_H_

namespace toybox
{

/*!
 * @class
 * 仮想的にクラスを動的確保するためのクラス
 * Tで型指定。MAX_SIZEで最大生成数を指定
 */
template<typename T,TbUint32 MAX_SIZE>
class TbTypeHeap{

public:
    
    //内部ノード用
    enum NodeType{
        NODE_TYPE_FREE, //フリー
        NODE_TYPE_USED  //使用中
    };

    static const TbSint32 INDEX_INVALID = -1;
    
private:
    
    /*!
     * @class
     * @brief 内部ノード
     */
    class Node{
    public:
        
        //コンストラクタ
        Node():idx(-1),type(NODE_TYPE_FREE),prev(NULL),next(NULL){}
        
        //デストラクタ
        ~Node(){
            if(NODE_TYPE_USED == type){
                reinterpret_cast<T*>(&value)->~T();
            }
        }
        
    public:
        TbUint8 value[sizeof(T)];   //!< バッファ
        TbSint32 idx;               //!< 配列インデックス番号
        NodeType type;              //!< 状態
        Node *prev;                 //!< 前ノード
        Node *next;                 //!< 次ノード
    };

public:

    /*!
     * コンストラクタ
     */
    TbTypeHeap(): m_Size(0),m_NodeList(),m_BeginFreeNode(NULL){
        if(MAX_SIZE > 0){
            for(TbUint32 i=0;i<MAX_SIZE;++i){
                m_NodeList[i].idx = i;
                m_NodeList[i].prev = &m_NodeList[(i>0) ? i-1 : MAX_SIZE-1];
                m_NodeList[i].next = &m_NodeList[(i<MAX_SIZE-1) ? i+1 : 0];
            }
            m_BeginFreeNode = &m_NodeList[0];
        }
    }
    
public:
    
    /*!
     * 引数無しのコンストラクタ呼び出し
     */
    T *New()
    {
        Node *node = createNode();
        if(node!=NULL){
            return new(&node->value) T();
        }
        return NULL;
    }
    
    /*!
     * 引数1つのコンストラクタ呼び出し
     * @return  
     */
    template<typename T1>
    T *New(T1 t1)
    {
        Node *node = createNode();
        if(node!=NULL){
            return new(&node->value) T(t1);
        }
        return NULL;
    }
    
    /*!
     * 引数2つのコンストラクタ
     */
    template<typename T1,typename T2>
    T *New(T1 t1,T2 t2)
    {
        Node *node = createNode();
        if(node!=NULL){
            return new(&node->value) T(t1,t2);
        }
        return NULL;
    }
    
    /*!
     * 引数3つのコンストラクタ
     */
    template<typename T1,typename T2,typename T3>
    T *New(T1 t1,T2 t2,T3 t3)
    {
        Node *node = createNode();
        if(node!=NULL){
            return new(&node->value) T(t1,t2,t3);
        }
        return NULL;
    }

    /*!
     * 引数4つのコンストラクタ
     */    
    template<typename T1,typename T2,typename T3,typename T4>
    T *New(T1 t1,T2 t2,T3 t3,T4 t4)
    {
        Node *node = createNode();
        if(node!=NULL){
            return new(&node->value) T(t1,t2,t3,t4);
        }
        return NULL;
    }
    
    /*!
     * 引数5つのコンストラクタ
     */
    template<typename T1,typename T2,typename T3,typename T4,typename T5>
    T *New(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5)
    {
        Node *node = createNode();
        if(node!=NULL){
            return new(&node->value) T(t1,t2,t3,t4,t5);
        }
        return NULL;
    }
    
    /*!
     * 削除
     */
    TbBool Delete(void *value)
    {
        Node *node = getNode(value); 
        if(node->idx < 0 || node->idx >= m_Size || node->type != NODE_TYPE_USED){
            return TB_FALSE;
        }
        return deleteNode(node);
    }
    
    /*! 
     * 確保しているバッファの内部配列のインデックス番号取得
     */
    TbSint32 GetIndex(void *value) const
    {
        Node *node = getNode(value);
        if(node->idx < 0 || node->idx >= m_Size || node->type != NODE_TYPE_USED){
            return INDEX_INVALID;
        }
        return node->idx;
    }
    
public:

    /*!
     * 最大要素数取得
     * @retrun 最大要素数
     */
    TbUint32 GetMaxSize() const {return MAX_SIZE;}
        
    /*!
     * 現在生成済みの数
     */
    TbUint32 GetSize() const {return m_Size;}

    /*!
     * インデックス番号から取得
     */
    T* Get(TbUint32 idx)
    {
        if(idx < MAX_SIZE && m_NodeList[idx].type == NODE_TYPE_USED){
            return reinterpret_cast<T*>(&m_NodeList[idx].value);
        }
        return NULL;
    }

private:

    /*!
     * ノード取得
     * @param value 値
     */ 
    Node* getNode(void *value) const 
    {
        if(reinterpret_cast<TbUint32>(&m_NodeList[0]) <= reinterpret_cast<TbUint32>(value) && 
           reinterpret_cast<TbUint32>(&m_NodeList[MAX_SIZE-1]) >= reinterpret_cast<TbUint32>(value))
        {
           return reinterpret_cast<Node*>(value); 
        }
        return NULL;
    }
    
    /*!
     * 空きノードから使用するためのノードを取得
     * この中ではチェインの位置を確定しない
     */
    Node *createNode(){
        Node *result = NULL;
        if(m_BeginFreeNode!=NULL){
            result = m_BeginFreeNode;
            if(m_BeginFreeNode != m_BeginFreeNode->next){
                //チェーンから消す
                Node *next = m_BeginFreeNode->next;
                Node *prev = m_BeginFreeNode->prev;
                next->prev = prev;
                prev->next = next;
                m_BeginFreeNode = next;
            }else{
                m_BeginFreeNode = NULL;
            }
            result->type = NODE_TYPE_USED;
            result->prev = NULL;
            result->next = NULL;
            ++m_Size;
        }
        return result;
    }
    
    /**
     * 使用中ノードを空きノードへ開放する
     */
   TbBool deleteNode(Node *node){
        node->type = NODE_TYPE_FREE;
        reinterpret_cast<T*>(&(node->value))->~T();
        if(m_BeginFreeNode!=NULL){
            //チェーンには先頭の前(最後尾)につなげる
            Node *prev = m_BeginFreeNode->prev;
            m_BeginFreeNode->prev = node;
            prev->next = node;
            node->next = m_BeginFreeNode;
            node->prev = prev;
        }else{
            m_BeginFreeNode = node;
            node->prev = node;
            node->next = node;
        }
        --m_Size;
        return TB_TRUE;
    }

private:

    TbSint32  m_Size;                 //!< 使用済み要素数
    Node      m_NodeList[MAX_SIZE];   //!< 要素リスト
    Node*     m_BeginFreeNode;        //!< 先頭フリーノード

};

}

#endif
