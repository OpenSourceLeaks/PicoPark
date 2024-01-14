/*!
 * 固定長配列クラスの実装ファイル
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
 
namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsu    
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
TbStaticArray<T,CAPACITY>::TbStaticArray()
    : m_Count( 0 )
{
    m_ArrayPtr = reinterpret_cast<T*>(m_Array);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,TbUint32 CAPACITY >
TbStaticArray<T,CAPACITY>::~TbStaticArray()
{
    Clear();
}

/*!
 * アクセス演算子
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,TbUint32 CAPACITY >
T& TbStaticArray< T , CAPACITY >::operator[]( const TbUint32 index )
{
    return GetAt(index);
}

/*!
 * アクセス演算子
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,TbUint32 CAPACITY >
const T& TbStaticArray< T , CAPACITY >::operator[]( const TbUint32 index ) const
{
    return GetAt(index);
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
void TbStaticArray<T,CAPACITY>::Clear()
{
    for( TbUint32 i = 0; i < m_Count; ++i ){
        TbDestroy(getValue(i));
    }
    m_Count = 0;
}

/*!
 * 挿入
 * @author Miyake_Shunsuke
 * @since 2014.04.02
 */
template< typename T , TbUint32 CAPACITY >
TbBool TbStaticArray<T,CAPACITY>::Insert( const TbSint32 index , const T& element )
{
    if( index == m_Count ){
        return PushBack( element );
    }
    if( index >= CAPACITY ){
        return TB_FALSE;
    }
    // 一度最後尾に突っ込んだ後、順番にコピー
    PushBack( *getValue(m_Count-1) );
    for( TbSint32 i = static_cast<TbSint32>(m_Count-1); i > index; --i ){
        *getValue(i) = *getValue(i-1);
    }
    TbDestroy(getValue(index));
    new(reinterpret_cast<void*>(getValue(index))) T(element);
    return TB_TRUE;
}

/*!
 * 挿入
 * @author Miyake_Shunsuke
 * @since 2014.04.02
 */
template< typename T , TbUint32 CAPACITY >
TbBool TbStaticArray<T,CAPACITY>::Insert( const ConstIterator& ite , const T& value )
{
    TbSint32 index = ite - BeginConst();
    return Insert(index,value);
}

/*!
 * 同一要素を複数個追加
 * @author Miyake Shunsuke
 * @since 2011.10.10
 */
template< typename T , TbUint32 CAPACITY >
TbBool TbStaticArray<T,CAPACITY>::PushBack( const TbUint32 count , const T& value )
{
    if( m_Count+count > CAPACITY ){
        return TB_FALSE;
    }
    for( TbUint32 i = 0; i < count; ++i ){
        new(reinterpret_cast<void*>(getValue(m_Count))) T(value);
        ++m_Count;
    }
    return TB_TRUE;
}

/*!
 * 複数個要素配列
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
TbBool TbStaticArray<T,CAPACITY>::PushBackArray( const TbUint32 count , const T* value )
{
    if( m_Count+count > CAPACITY ){
        return TB_FALSE;
    }
    for( TbUint32 i = 0; i < count; ++i ){
        new(reinterpret_cast<void*>(getValue(m_Count))) T(value[i]);
        ++m_Count;
    }
    return TB_TRUE;
}

/*!
 * 要素追加
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
TbBool TbStaticArray<T,CAPACITY>::PushBack( const T& value )
{
    if( m_Count < CAPACITY ){
        new(reinterpret_cast<void*>(getValue(m_Count))) T(value);
        ++m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 要素の追加
 * 配列の最後尾に追加される。
 * @return 追加の是非
 * @param count 要素数
 * @author Miyake Shunsuke
 * @since 2011.10.9
 */
template<typename T, TbUint32 CAPACITY>
TbBool TbStaticArray<T,CAPACITY>::PushBackDefault( TbUint32 count )
{
    if( m_Count+static_cast<TbSint32>(count) > CAPACITY ){
        return TB_FALSE;
    }
    for( TbUint32 i = 0; i < count; ++i ){
        new(reinterpret_cast<void*>(getValue(m_Count+i))) T();
    }
    m_Count += count;
    return TB_TRUE;
}

/*!
 * 最後尾除去
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
TbBool TbStaticArray<T,CAPACITY>::PopBack()
{
    if( m_Count > 0 ){
        TbDestroy(getValue(m_Count-1));
        --m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 要素取得
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
T& TbStaticArray<T,CAPACITY>::GetAt( const TbUint32 index )
{
    TB_ASSERT_MSG( index < m_Count ,"Out Of Access");
    return *getValue(index);
}

/*!
 * 要素取得
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
const T& TbStaticArray<T,CAPACITY>::GetAt( const TbUint32 index ) const
{
    TB_ASSERT_MSG( index < m_Count ,"Out Of Access");
    return *getValue(index);
}

/*!
 * 要素除去
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
typename TbStaticArray<T,CAPACITY>::Iterator TbStaticArray<T,CAPACITY>::Erase( const TbUint32 index )
{
    if( index < m_Count ){
        if( index < m_Count - 1 ){
            for( TbUint32 currentIndex = index; currentIndex < m_Count-1; ++currentIndex ){
                *getValue( currentIndex ) = *getValue( currentIndex+1 );
            }
        }
        TbDestroy( getValue(m_Count-1) );
        --m_Count;
        return Iterator(getValue(index));
    }
    return Begin();
}

/*!
 * 要素除去
 * @author Miyake_Shunsuke
 * @since 2012.06.12
 */
template<typename T,TbUint32 CAPACITY>
typename TbStaticArray<T,CAPACITY>::Iterator TbStaticArray<T,CAPACITY>::Erase( Iterator& ite )
{
    TbSint32 index = ite - Begin();
    return Erase(index);
}


}