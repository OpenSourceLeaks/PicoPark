/*!
 * 両端キューの実装ファイル
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
TbDeque<T,Allocator>::TbDeque( const Allocator& allocator )
    : m_BitArray()
    , m_Array( NULL )
    , m_Allocator(allocator)
    , m_Count( 0 )
    , m_Capacity( 0U )
    , m_FrontIndex( 0 )
{
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
TbDeque<T,Allocator>::TbDeque( TbUint32 capacity , const Allocator& allocator )
    : m_BitArray()
    , m_Array( NULL )
    , m_Allocator(allocator)
    , m_Count( 0 )
    , m_Capacity( 0U )
    , m_FrontIndex( 0 )
{
    SetCapacity( capacity );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,typename Allocator>
TbDeque<T,Allocator>::~TbDeque()
{
    Clear();
    m_Allocator.Deallocate(m_Array,m_Capacity);
}
    
/*!
 * アクセス演算子
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,typename Allocator>
T& TbDeque<T,Allocator>::operator[]( const TbUint32 index )
{
    return GetAt(index);
}

/*!
 * アクセス演算子
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
const T& TbDeque<T,Allocator>::operator[]( const TbUint32 index ) const
{
    return GetAt(index);
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
void TbDeque<T,Allocator>::Clear()
{
    TbUint32 frontIndex = m_FrontIndex;
    TbUint32 backIndex = getIndex(m_Count-1);
    for( TbUint32 i = frontIndex; i != backIndex; i = ((i+1)%m_Capacity) ){
        m_Allocator.Destroy(&m_Array[i]);
    }
    m_FrontIndex = 0U;
    m_Count = 0U;
}

/*!
 * 後ろから要素追加
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
TbBool TbDeque<T,Allocator>::PushBack( const T& value )
{
    if( IsEnablePopAuto() && IsFull() ){ // 自動ポップ
        PopFront();
    }
    if( m_Count < m_Capacity ){
        TbUint32 index = getPushBackIndex();
        m_Allocator.Construct( &m_Array[index] , value );
        ++m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 前から要素追加
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
TbBool TbDeque<T,Allocator>::PushFront( const T& value )
{
    if( IsEnablePopAuto() && IsFull() ){ // 自動ポップ
        PopBack();
    }
    if( m_Count < m_Capacity ){
        m_FrontIndex = getPushFrontIndex();
        m_Allocator.Construct( m_Array[m_FrontIndex] , value );
        ++m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 最後尾除去
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
TbBool TbDeque<T,Allocator>::PopFront()
{
    if( m_Count > 0 ){
        --m_Count;
        m_Allocator.Destroy( &m_Array[m_FrontIndex] );
        m_FrontIndex = (m_FrontIndex+1)% m_Capacity;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 最後尾除去
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
TbBool TbDeque<T,Allocator>::PopBack()
{
    if( m_Count > 0 ){
        TbUint32 backIndex = getIndex(m_Count-1);
        m_Allocator.Destroy( &m_Array[backIndex] );
        --m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 先頭取得
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
template< typename T,typename Allocator>
T& TbDeque<T,Allocator>::GetFront()
{
    TB_ASSERT(m_Count>0);
    return m_Array[ getIndex(0) ];
}

/*!
 * 最後尾取得
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
template< typename T,typename Allocator>
T& TbDeque<T,Allocator>::GetBack()
{
    TB_ASSERT(m_Count>0);
    return m_Array[ getIndex(m_Count-1) ];    
}

/*!
 * 要素取得
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
T& TbDeque<T,Allocator>::GetAt( const TbUint32 index )
{
    TB_ASSERT_MSG( index < m_Count ,"Out Of Access");
    return m_Array[getIndex(index)];
}

/*!
 * 要素取得
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
const T& TbDeque<T,Allocator>::GetAt( const TbUint32 index ) const
{
    TB_ASSERT_MSG( index < m_Count ,"Out Of Access");
    return m_Array[getIndex(index)];
}

/*!
 * 最大数設定
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */
template< typename T,typename Allocator>
void TbDeque<T,Allocator>::SetCapacity( TbUint32 capacity )
{
    if( capacity <= m_Capacity ){
        return;
    }
    T* array = m_Allocator.Allocate( capacity );
    if( m_Array ){
        TbUint32 frontIndex = m_FrontIndex;
        TbUint32 backIndex = getIndex(m_Count-1);
        TbUint32 destIndex = 0;
        for( TbUint32 i = frontIndex; i != backIndex; i = ((i+1)%m_Capacity) , ++destIndex ){
            m_Allocator.Construct(&array[destIndex],m_Array[i]);
            m_Allocator.Destroy(&m_Array[i]);
        }
        m_Allocator.Deallocate(m_Array,m_Capacity);
    }
    m_Array = array;
    m_Capacity = capacity;
    m_FrontIndex = 0;
}

}