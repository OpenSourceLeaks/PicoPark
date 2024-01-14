/*!
 * ポインタプールクラスの実装ファイル
 * @author Miyake Shunsuke
 */

namespace toybox
{

/*!
 * イテレータ
 */
template<typename T,typename Allocator>
class TbPool<T,Allocator>::Iterator
{
public:
    typedef T ValueType;
public:

    //! コンストラクタ
    Iterator(const Iterator& ite)
        : m_Current(ite.m_Current)
        , m_Pool(ite.m_Pool){}

    //! デストラクタ
    ~Iterator(){}

private:

    //! TbPoolのみから呼ばれるコンストラクタ
    Iterator( DataBlock* current , TbPool<T,Allocator>* pool)
        : m_Current(current)
        , m_Pool(pool)
    {}

public:

    //! 代入演算子
    Iterator& operator=(const Iterator& ite) { 
        (m_Current = ite.m_Current); return (*this); 
    }

    //! 参照演算子
    T& operator*() {
        return m_Current->value;
    }

    //! 参照演算子
    const T& operator*() const {
        return m_Current->value;
    }

    //! インクリメント演算子
    Iterator& operator++()
    {
        // 自身がNULLなら既に終了済み
        if( !m_Current ){
            return *this;
        }
        // 最後尾ならもうない
        if( static_cast<TbUint32>(m_Current-m_Pool->m_Pool) == m_Pool->m_EndUsedIndex ){
            m_Current = NULL;
            return *this;
        }
        do{
            ++m_Current;
        }while( INDEX_USED != m_Current->nextIndex );
        return *this;
    }

    //! 比較演算子
    TbBool operator==(const Iterator& ite) const { 
        return m_Current == ite.m_Current; 
    }

    //! 比較演算子
    TbBool operator!=(const Iterator& ite) const { 
        return m_Current != ite.m_Current; 
    }

private:

    DataBlock* m_Current;
    TbPool<T>* m_Pool;

private:

    friend class TbPool<T>;
};

/*!
 * イテレータ
 */
template<typename T,typename Allocator>
class TbPool<T,Allocator>::ConstIterator
{
public:
    typedef T ValueType;
public:

    //! コンストラクタ
    ConstIterator(const ConstIterator& ite)
        : m_Current(ite.m_Current)
        , m_Pool(ite.m_Pool){}

    //! デストラクタ
    ~ConstIterator(){}

private:

    //! TbPoolのみから呼ばれるコンストラクタ
    ConstIterator( const DataBlock* current , const TbPool<T,Allocator>* pool)
        : m_Current(current)
        , m_Pool(pool)
    {}

public:

    //! 代入演算子
    ConstIterator& operator=(const Iterator& ite) { 
        (m_Current = ite.m_Current); 
        return (*this); 
    }

    //! 参照演算子
    const T& operator*() const {
        return m_Current->value;
    }

    //! インクリメント演算子
    ConstIterator& operator++()
    {
        // 自身がNULLなら既に終了済み
        if( !m_Current ){
            return *this;
        }
        // 最後尾ならもうない
        if( static_cast<TbUint32>(m_Current-m_Pool->m_Pool) == m_Pool->m_EndUsedIndex ){
            m_Current = NULL;
            return *this;
        }
        do{
            ++m_Current;
        }while( INDEX_USED != m_Current->nextIndex );
        return *this;
    }

    //! 比較演算子
    TbBool operator==(const ConstIterator& ite) const { 
        return m_Current == ite.m_Current; 
    }

    //! 比較演算子
    TbBool operator!=(const ConstIterator& ite) const { 
        return m_Current != ite.m_Current; 
    }

private:

    const DataBlock* m_Current;
    const TbPool<T>* m_Pool;

private:

    friend class TbPool<T>;
};

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
TbPool<T,Allocator>::TbPool(TbUint32 capacity , const Allocator& allocator , const BlockAllocator& blockAllocator)
    : m_Pool(NULL)
    , m_NextFreeBlock(NULL)
    , m_BeginUsedIndex(INDEX_INVALID)
    , m_EndUsedIndex(INDEX_INVALID)
    , m_Capacity(capacity)
    , m_Count(0)
    , m_IssueCount(0)
    , m_ValueAllocator(allocator)
    , m_BlockAllocator(blockAllocator)
{
    SetCapacity(capacity);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
TbPool<T,Allocator>::~TbPool()
{
    Clear();
    m_BlockAllocator.Deallocate(m_Pool,m_Capacity);
}

/*!
 * 追加
 * @param value 値
 * @return 登録インデックス番号
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
TbPoolId TbPool<T,Allocator>::Add(const T& value)
{
    TbPoolId result = TB_POOL_ID_INVALID;
    if( IsFull() && IsEnableSetCapacityAuto() ){
        SetCapacity( GetCapacity() * 2 );
    }
    if( !IsFull() )
    {
        ++m_IssueCount;
        TbUint32 index = static_cast<TbUint32>(m_NextFreeBlock-m_Pool);
        m_ValueAllocator.Construct(&m_NextFreeBlock->value,value);
        if( INDEX_INVALID != m_NextFreeBlock->nextIndex ){
            DataBlock* next = &m_Pool[m_NextFreeBlock->nextIndex];
            m_NextFreeBlock->nextIndex = INDEX_USED;
            m_NextFreeBlock = next;
        }else{
            m_NextFreeBlock->nextIndex = INDEX_USED;
            m_NextFreeBlock = NULL;
        }

        // 一番低い、高い利用インデックス値更新
        if( INDEX_INVALID == m_BeginUsedIndex || m_BeginUsedIndex > index ){
            m_BeginUsedIndex = index;
        }
        if( INDEX_INVALID == m_EndUsedIndex || m_EndUsedIndex < index ){
            m_EndUsedIndex = index;
        }

        result = TB_POOL_CREATE_ID( m_IssueCount , index );
        ++m_Count;
    }
    return result;
}

/*!
 * 取得
 * @param index インデックス番号
 * @return 値
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
T*        TbPool<T,Allocator>::GetAt(TbPoolId id)
{
    TbUint32 index = TB_POOL_GET_INDEX_FROM_ID(id);
    if(index < m_Capacity && INDEX_USED == m_Pool[index].nextIndex ){
        return &m_Pool[index].value;
    }
    return NULL;
}

/*!
 * 取得
 * @param index インデックス番号
 * @return 値
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
const T*        TbPool<T,Allocator>::GetAt(TbPoolId id) const 
{
    TbUint32 index = TB_POOL_GET_INDEX_FROM_ID(id);
    if(index < m_Capacity && INDEX_USED == m_Pool[index].nextIndex ){
        return &m_Pool[index].value;
    }
    return NULL;
}

/*!
 * クリアー
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
void TbPool<T,Allocator>::Clear()
{
    m_Count = 0;
    m_BeginUsedIndex = INDEX_INVALID;
    m_EndUsedIndex = INDEX_INVALID;
    if( m_Capacity > 0 ){
        m_NextFreeBlock = &m_Pool[0];
        for( TbUint32 i = 0; i < m_Capacity-1; ++i ){
            if( INDEX_USED == m_Pool[i].nextIndex ){
                m_ValueAllocator.Destroy(&m_Pool[i].value);
            }
            m_NextFreeBlock->nextIndex = i+1;
        }
        DataBlock& last = m_Pool[m_Capacity-1];
        if( INDEX_USED == last.nextIndex ){
            m_ValueAllocator.Destroy(&last.value);
        }
        last.nextIndex = INDEX_INVALID;
    }
}


/*!
 * 除去
 * @param index インデックス番号
 * @retval TB_TRUE 除去成功
 * @retval TB_FALSE 除去失敗
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
TbBool TbPool<T,Allocator>::Remove(TbPoolId id)
{
    TbUint32 index = TB_POOL_GET_INDEX_FROM_ID(id);
    if(index < m_Capacity && INDEX_USED == m_Pool[index].nextIndex )
    {
        m_ValueAllocator.Destroy(&m_Pool[index].value);
        m_Pool[index].nextIndex = static_cast<TbUint32>(m_NextFreeBlock-m_Pool);
        m_NextFreeBlock = &m_Pool[index];
        --m_Count;

        // 一番低い、高いインデックス値の更新
        if( index == m_BeginUsedIndex && index == m_EndUsedIndex ){
            m_BeginUsedIndex = INDEX_INVALID;
            m_EndUsedIndex   = INDEX_INVALID;
        }else if( index == m_BeginUsedIndex ){
            TB_ASSERT(index<m_Capacity-1);
            TbUint32 targetIndex = index+1;
            while( targetIndex < m_Capacity ){
                if( INDEX_USED == m_Pool[targetIndex].nextIndex ){
                    m_BeginUsedIndex = targetIndex;
                    break;
                }
                ++targetIndex;
            }
        }else if( index == m_EndUsedIndex ){
            TB_ASSERT(index>0);
            TbUint32 targetIndex = index-1;
            while( targetIndex > 0 ){
                if( INDEX_USED == m_Pool[targetIndex].nextIndex ){
                    m_EndUsedIndex = targetIndex;
                    break;
                }
                --targetIndex;
            }
        }

        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 最大登録数取得
 * @return 最大登録数
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
TbUint32    TbPool<T,Allocator>::GetCapacity() const
{
    return m_Capacity;
}

/*!
 * リサイズ
 * @param capacity 登録可能最大数
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
void        TbPool<T,Allocator>::SetCapacity(TbUint32 capacity)
{
    TbUint32 count = 0;
    m_BeginUsedIndex = INDEX_INVALID;
    m_EndUsedIndex   = INDEX_INVALID;
    m_NextFreeBlock = NULL;
    DataBlock* tmpPool = capacity > 0 ? m_BlockAllocator.Allocate(capacity) : NULL;
    if(m_Pool){
        
        // コピー
        DataBlock* tmpNext = NULL;
        for( TbUint32 i = 0; i < capacity; ++i ){
            if( i < m_Capacity && INDEX_USED == m_Pool[i].nextIndex ){
                m_ValueAllocator.Construct(&tmpPool[i].value,m_Pool[i].value);
                tmpPool[i].nextIndex = INDEX_USED;

                // 一番低い、高いインデックス値の更新
                if( INDEX_INVALID == m_BeginUsedIndex || m_BeginUsedIndex > i ){
                    m_BeginUsedIndex = i;
                }
                if( INDEX_INVALID == m_EndUsedIndex || m_EndUsedIndex < i ){
                    m_EndUsedIndex = i;
                }
                ++count;
            }else{
                if( !m_NextFreeBlock ){
                    m_NextFreeBlock = &tmpPool[i];
                    tmpNext= m_NextFreeBlock;
                }else{
                    tmpNext->nextIndex = i;
                    tmpNext = &tmpPool[i];
                }
            }
        }
        if( tmpNext ){
            tmpNext->nextIndex = INDEX_INVALID;
        }

        // 後始末
        for( TbUint32 i = 0; i < m_Capacity; ++i ){
            if( INDEX_USED == m_Pool[i].nextIndex ){
                m_ValueAllocator.Destroy(&m_Pool[i].value);
            }
        }
        m_BlockAllocator.Deallocate(m_Pool,m_Capacity);
    }else if( capacity > 0 ){
        // 初期化
        m_NextFreeBlock = &tmpPool[0];
        for( TbUint32 i = 0; i < capacity-1; ++i ){
            tmpPool[i].nextIndex = i+1;
        }
        tmpPool[capacity-1].nextIndex = INDEX_INVALID;
    }
    m_Pool = tmpPool;
    m_Capacity = capacity;
    m_Count = count;
}

/*!
 * 登録数取得
 * @return 登録数
 * @author Miyake Shunsuke
 */
template< typename T,typename Allocator>
TbUint32    TbPool<T,Allocator>::GetCount() const
{
    return m_Count;
}

}