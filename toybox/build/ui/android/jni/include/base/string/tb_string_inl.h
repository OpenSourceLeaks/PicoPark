/*!
 * @file
 * @brief 文字列クラステンプレートの実装ファイル
 * @author Miyake Shunsuke
 */

/*!
 * コンストラクタ
 * 空生成
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>::TbStringBase()
    : m_Data( NULL )
{
}

/*!
 * コンストラクタ
 * 文字列クラスから生成
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>::TbStringBase(const TbStringBase<CHAR_TYPE>& string)
    : m_Data( NULL )
{
    (*this) = string;
}

/*!
 * コンストラクタ
 * 生文字列ポインタから生成
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>::TbStringBase(const CHAR_TYPE* string )
    : m_Data( NULL )
{
    (*this) = string;
}

/*!
 * コンストラクタ
 * 二つの文字列を結合して生成
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>::TbStringBase(const CHAR_TYPE* str1,const CHAR_TYPE* str2)
    : m_Data( NULL )
{
    createData();
    TbSint32 size = TbStrCharCount( str1 ) + TbStrCharCount( str2 ) + 1;
    m_Data->buffer = createBuffer( size );
    TbStrPrintf( m_Data->buffer , size , "%s%s" , str1 , str2 );
}

/*!
 *デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>::~TbStringBase()
{
    releaseData();
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>& TbStringBase<CHAR_TYPE>::operator=(const TbStringBase<CHAR_TYPE>& string)
{
    releaseData();
    m_Data = string.m_Data;
    if( m_Data ){
        ++m_Data->refCount;
    }
    return (*this);
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE>& TbStringBase<CHAR_TYPE>::operator=(const CHAR_TYPE* string)
{
    releaseData();
    if( string ){
        createData();
        m_Data->buffer = createBuffer( TbStrBufSize(string) );
        TbStrCopy( m_Data->buffer , TbStrCharCount(string)+1 , string );
    }
    return (*this);
}

/*!
 * 加算演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE> TbStringBase<CHAR_TYPE>::operator+(const TbStringBase<CHAR_TYPE>& string)
{
    if( m_Data && string.m_Data ){
        TbStringBase<CHAR_TYPE> result( m_Data->buffer , string.m_Data->buffer );
        return result;
    }
    TbStringBase<CHAR_TYPE> result;
    return result;
}

/*!
 * 加算演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE> TbStringBase<CHAR_TYPE>::operator+(const CHAR_TYPE* string)
{
    if( m_Data ){
        TbStringBase<CHAR_TYPE> result( m_Data->buffer , *string );
        return result;
    }
    TbStringBase<CHAR_TYPE> result;
    return result;
}

/*!
 * 比較演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbBool TbStringBase<CHAR_TYPE>::operator==(const TbStringBase<CHAR_TYPE>& string)
{
    if( m_Data && string.m_Data ){
        return TbStrCmp(m_Data->buffer,string.m_Data->buffer) == 0;
    }
    return TB_FALSE;
}

/*!
 * 比較演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
TbBool TbStringBase<CHAR_TYPE>::operator!=(const TbStringBase<CHAR_TYPE>& string)
{
    if( m_Data && string.m_Data ){
        return TbStrCmp(m_Data->buffer,string.m_Data->buffer) != 0;
    }
    return TB_FALSE;
}

/*!
 * 配列演算子
 * @author Miyake Shunsuke
 */
template <typename CHAR_TYPE>
CHAR_TYPE TbStringBase<CHAR_TYPE>::operator[](TbSint32 index)
{
    if( m_Data && 0 < index && index < TbStrLen( m_Data->buffer ) ){
        return m_Data->buffer[index];
    }
    TB_ASSERT(TB_FALSE);
    return 0;
}

/*!
 * フォーマット生成
 * @author Miyake Shunsuke
 * @since 2010.06.25
 */
template <typename CHAR_TYPE>
void  TbStringBase<CHAR_TYPE>::format( TbStringBase<CHAR_TYPE>& dst , const CHAR_TYPE* format , va_list argList )
{
    CHAR_TYPE    buf[ TB_MAX_PATH ];
    vsprintf( buf , format , argList );
    dst = buf;
}

/*!
 * フォーマット形式で生成
 * @author Miyake Shunsuke
 * @since 2010.06.25
 */
template <typename CHAR_TYPE>
TbStringBase<CHAR_TYPE> TbStringBase<CHAR_TYPE>::Format( const CHAR_TYPE* fmt , ... )
{
    TbStringBase<CHAR_TYPE> string;
    va_list ap;
    va_start( ap, fmt );
    format( string , fmt , ap );
    va_end( ap );
    return string;
}

/*!
 * バッファ取得
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
template <typename CHAR_TYPE>
const CHAR_TYPE* TbStringBase<CHAR_TYPE>::GetBuffer() const
{
    if( m_Data ){
        return m_Data->buffer;
    }
    return TbGetEmptyString<CHAR_TYPE>();
}

/*!
 * 文字数取得
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
template <typename CHAR_TYPE>
TbSint32 TbStringBase<CHAR_TYPE>::GetBufferSize() const
{
    if( m_Data ){
        return (TbStrCharCount( m_Data->buffer)+1) * sizeof(CHAR_TYPE);
    }
    return 0;
}

/*!
 * 文字数取得
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
template <typename CHAR_TYPE>
TbSint32 TbStringBase<CHAR_TYPE>::GetCharCount() const
{
    if( m_Data ){
        return TbStrCharCount( m_Data->buffer );
    }
    return 0;
}

/*!
 * データ生成
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
template <typename CHAR_TYPE>
void TbStringBase<CHAR_TYPE>::createData()
{
    TB_ASSERT( m_Data == NULL );
    m_Data = TB_NEW Data();
}

/*!
 * データ解放
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
template <typename CHAR_TYPE>
void TbStringBase<CHAR_TYPE>::releaseData()
{
    if( m_Data ){
        if( (--m_Data->refCount) <= 0 ){
            TB_SAFE_DELETE_ARRAY( m_Data->buffer );
            TB_SAFE_DELETE( m_Data );
        }
    }
    m_Data = NULL;
}

/*!
 * バッファ生成
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
template <typename CHAR_TYPE>
CHAR_TYPE* TbStringBase<CHAR_TYPE>::createBuffer( TbSint32 size )
{
    CHAR_TYPE* result = TB_NEW CHAR_TYPE[ size ];
    return result;
}
