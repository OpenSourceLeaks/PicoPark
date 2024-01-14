/*!
 * @file
 * @brief 可変長配列クラス
 * @author Miyake Shunsuke
 */

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T,typename Allocator>
TbArray<T,Allocator>::TbArray( TbSint32 capacity , const Allocator& allocator )
    : m_Capacity( 0 )
    , m_Count(0)
    , m_IsEnableSetCapacityAuto( TB_FALSE )
    , m_Array( NULL )
    , m_Allocator(allocator)
{
    SetCapacity( capacity );
}
    
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T,typename Allocator>
TbArray<T,Allocator>::~TbArray() 
{
    for( TbSint32 i = 0; i < m_Count; ++i ){
        m_Allocator.Destroy(&m_Array[i]);
    }
    m_Allocator.Deallocate(m_Array,m_Capacity);
}

/*!
 * リサイズ
 * @author Miyake Shunsuke
 * @since 2011.08.11
 */
template<typename T,typename Allocator>
void TbArray<T,Allocator>::SetCapacity( const TbSint32 capacity )
{
    if( capacity <= m_Capacity ){
        return;
    }
    T* array = m_Allocator.Allocate(capacity);
    if( m_Array ){
        // 未初期化メモリにコピー
        TbUninitializedMove(m_Array,
                            &m_Array[m_Count],
                            array,
                            m_Allocator,
                            TbBoolType<TbIsScalar<T>::VALUE>());
        for( TbSint32 i = 0; i < m_Count; ++i ){
            m_Allocator.Destroy(&m_Array[i]);
        }
        m_Allocator.Deallocate(m_Array,m_Capacity);
    }
    m_Array = array;
    m_Capacity = capacity;
}

/*!
 * 挿入
 * @author Miyake Shunsuke
 * @since 2011.07.26
 */
template<typename T,typename Allocator>
TB_FORCE_INLINE TbBool TbArray<T,Allocator>::Insert( const TbSint32 index , const T& element )
{
    if( index == m_Count ){
        return PushBack( element );
    }
    if( m_Count >= m_Capacity ){
        if( !IsEnableSetCapacityAuto() ){
            return TB_FALSE;
        }
        SetCapacity( m_Capacity + 1 );
    }
    if( index >= m_Capacity ){
        return TB_FALSE;
    }

    // 一度最後尾に突っ込んだ後、順番にコピー
    m_Allocator.Construct( &m_Array[m_Count] , m_Array[m_Count-1] );
    for( TbSint32 i = static_cast<TbSint32>(m_Count-1); i > index; --i ){
        m_Array[i] = m_Array[i-1];
    }
    m_Allocator.Destroy(&m_Array[index]);
    m_Allocator.Construct( &m_Array[index] , element );

    ++m_Count;
    return TB_TRUE;        
}

/*!
 * 挿入
 * @author Miyake Shunsuke
 * @since 2011.07.26
 */
template<typename T,typename Allocator>
TB_FORCE_INLINE TbBool TbArray<T,Allocator>::Insert( Iterator& ite , const T& element )
{
    TbSint32 index = ite - Begin();
    return Insert(index,element);
}

/*!
 * 要素の追加
 * 配列の最後尾に追加される。
 * @return 追加の是非
 * @param count 要素数
 * @param element 要素
 * @author Miyake Shunsuke
 * @since 2011.10.9
 */
template<typename T,typename Allocator>
TbBool TbArray<T,Allocator>::PushBack( TbUint32 count , const T& element )
{
    if( m_Count+static_cast<TbSint32>(count) > m_Capacity ){
        if( !IsEnableSetCapacityAuto() ){
            return TB_FALSE;
        }
        TbSizeT capacity = TbMax<TbSizeT>(m_Count+count,m_Count*2);
        SetCapacity( capacity );
    }
    for( TbUint32 i = 0; i < count; ++i ){
        m_Allocator.Construct( &m_Array[m_Count] , element );
        ++m_Count;
    }
    return TB_TRUE;
}

/*!
 * 要素の追加
 * 配列の最後尾に追加される。
 * @return 追加の是非
 * @param element 要素
 * @author Miyake Shunsuke
 * @since 2011.06.12
 */
template<typename T,typename Allocator>
TbBool TbArray<T,Allocator>::PushBack( const T& element )
{
    if( m_Count >= m_Capacity ){
        if( !IsEnableSetCapacityAuto() ){
            return TB_FALSE;
        }
        SetCapacity( m_Capacity + 1 );
    }
    m_Allocator.Construct( &m_Array[m_Count] , element );
    ++m_Count;
    return TB_TRUE;
}

/*!
 * 要素の追加
 * 配列の最後尾に追加される。
 * @return 追加の是非
 * @param count 要素数
 * @param element 要素
 * @author Miyake Shunsuke
 * @since 2011.10.9
 */
template<typename T,typename Allocator>
TbBool TbArray<T,Allocator>::PushBackDefault( TbUint32 count )
{
    if( m_Count+static_cast<TbSint32>(count) > m_Capacity ){
        if( !IsEnableSetCapacityAuto() ){
            return TB_FALSE;
        }
        TbSizeT capacity = TbMax<TbSizeT>(m_Count+count,m_Count*2);
        SetCapacity( capacity );
    }
    for( TbUint32 i = 0; i < count; ++i ){
        m_Allocator.Construct( &m_Array[m_Count] );
        ++m_Count;
    }
    return TB_TRUE;
}

/*!
 * 要素の除去
 * 配列の最後尾除去
 * @retval TB_TRUE 成功
 * @retval TB_FALSE 失敗
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T,typename Allocator>
TbBool TbArray<T,Allocator>::PopBack()
{
    if( m_Count > 0 ){
        m_Allocator.Destroy( &m_Array[m_Count-1] );
        --m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 要素の除去
 * @param index
 * @retval TB_TRUE 成功
 * @retval TB_FALSE 失敗
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T,typename Allocator>
typename TbArray<T,Allocator>::Iterator TbArray<T,Allocator>::Erase( const TbSint32 index )
{
    if( index < m_Count ){
        if( index < m_Count - 1 ){
            for( TbSint32 currentIndex = index; currentIndex < m_Count-1; ++currentIndex ){
                m_Array[ currentIndex ] = m_Array[ currentIndex+1 ];
            }
        }
        m_Allocator.Destroy( &m_Array[m_Count-1] );
        --m_Count;
        return Iterator(&m_Array[index]);
    }
    return Begin();
}

/*!
 * 要素の除去
 * @param index
 * @retval TB_TRUE 成功
 * @retval TB_FALSE 失敗
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T,typename Allocator>
typename TbArray<T,Allocator>::Iterator TbArray<T,Allocator>::Erase( const Iterator& ite )
{
    TbSint32 index = ite - Begin();
    return Erase(index);
}

/*!
 * 要素取得
 * @return　要素
 * @param index インデックス番号
 */
template<typename T,typename Allocator>
T& TbArray<T,Allocator>::GetAt(const TbSint32 index)
{
    TB_ASSERT(0 <= index && index < m_Count);
    return m_Array[ index ];
}

/*!
 * 要素取得
 * @return　要素
 * @param index インデックス番号
 */
template<typename T,typename Allocator>
const T& TbArray<T,Allocator>::GetAt(const TbSint32 index) const
{
    TB_ASSERT(0 <= index && index < m_Count);
    return m_Array[ index ];
}
    
/*!
 * 要素クリアー
 */
template<typename T,typename Allocator>
void TbArray<T,Allocator>::Clear()
{
    for( TbSint32 i = 0; i < m_Count; ++i ){
        m_Allocator.Destroy( &m_Array[i] );
    }
    m_Count = 0;
}

/*!
 * 先頭イテレータ取得
 * @return 先頭イテレータ
 */
template<typename T,typename Allocator>
typename TbArray<T,Allocator>::Iterator TbArray<T,Allocator>::Begin() 
{
    return Iterator(&(m_Array[0]));
}

/*!
 * 先頭イテレータ取得
 * @return 先頭イテレータ
 */
template<typename T,typename Allocator>
typename TbArray<T,Allocator>::ConstIterator TbArray<T,Allocator>::Begin() const
{
    return ConstIterator(&(m_Array[0]));
}

/*! 
 * 末端イテレータ取得
 * @return 末端イテレータ
 */
template<typename T,typename Allocator>
typename TbArray<T,Allocator>::Iterator TbArray<T,Allocator>::End() 
{
    return Iterator(&(m_Array[ m_Count ]));
}
    
/*! 
 * 末端イテレータ取得
 * @return 末端イテレータ
 */
template<typename T,typename Allocator>
typename TbArray<T,Allocator>::ConstIterator TbArray<T,Allocator>::End() const
{
    return ConstIterator(&(m_Array[ m_Count ]));
}

}
