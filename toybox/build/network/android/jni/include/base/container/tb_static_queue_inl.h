/*!
 * 固定長キュークラスの実装ファイル
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
TbStaticQueue<T,CAPACITY>::TbStaticQueue()
    : m_BitArray()
    , m_Count( 0 )
    , m_FrontIndex( 0 )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,TbUint32 CAPACITY >
TbStaticQueue<T,CAPACITY>::~TbStaticQueue()
{
    Clear();
}
    
/*!
 * アクセス演算子
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,TbUint32 CAPACITY >
T& TbStaticQueue< T , CAPACITY >::operator[]( const TbUint32 index )
{
    return GetAt(index);
}

/*!
 * アクセス演算子
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T,TbUint32 CAPACITY >
const T& TbStaticQueue< T , CAPACITY >::operator[]( const TbUint32 index ) const
{
    return GetAt(index);
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
void TbStaticQueue<T,CAPACITY>::Clear()
{
    for( TbUint32 i = 0; i < m_Count; ++i ){
        TbDestroy( getValue(getIndex(i)) );
    }
    m_FrontIndex = 0U;
    m_Count = 0U;
}

/*!
 * 後ろから要素追加
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template< typename T , TbUint32 CAPACITY >
TbBool TbStaticQueue<T,CAPACITY>::PushBack( const T& value )
{
    if( IsEnablePopAuto() && IsFull() ){ // 自動ポップ
        PopFront();
    }
    if( m_Count < CAPACITY ){
        TbUint32 index = getPushBackIndex();
        new(reinterpret_cast<void*>(getValue(index)) T(value);
        ++m_Count;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 最後尾除去
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
TbBool TbStaticQueue<T,CAPACITY>::PopFront()
{
    if( m_Count > 0 ){
        TbDestroy(getValue(m_FrontIndex));
        --m_Count;
        m_FrontIndex = (m_FrontIndex+1)%CAPACITY;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 先頭取得
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
template<typename T,TbUint32 CAPACITY>
T& TbStaticQueue<T,CAPACITY>::GetFront()
{
    TB_ASSERT(m_Count>0);
    return getValue( getIndex(0) );
}

/*!
 * 最後尾取得
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
template<typename T,TbUint32 CAPACITY>
T& TbStaticQueue<T,CAPACITY>::GetBack()
{
    TB_ASSERT(m_Count>0);
    return getValue( getIndex(m_Count-1) );    
}

/*!
 * 要素取得
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
T& TbStaticQueue<T,CAPACITY>::GetAt( const TbUint32 index )
{
    TB_ASSERT_MSG( index < m_Count ,"Out Of Access");
    return *getValue(getIndex(index));
}

/*!
 * 要素取得
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
const T& TbStaticQueue<T,CAPACITY>::GetAt( const TbUint32 index ) const
{
    TB_ASSERT_MSG( index < m_Count ,"Out Of Access");
    return *getValue(getIndex(index));
}

}