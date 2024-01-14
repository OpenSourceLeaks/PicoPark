/*!
 * @file
 * @brief 文字列クラステンプレートの実装ファイル
 * @author Miyake Shunsuke
 */

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
TbStaticStringBase<Char,SIZE>::TbStaticStringBase()
{
    Clear();
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
TbStaticStringBase<Char,SIZE>::TbStaticStringBase(const TbStaticStringBase<Char,SIZE>& string)
{
    (*this) = string.m_Buffer;
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
TbStaticStringBase<Char,SIZE>::TbStaticStringBase(const Char* string )
{
    if( string ){
        (*this) = string;
    }
}

/*!
 * 比較演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
bool TbStaticStringBase<Char,SIZE>::operator==(const TbStaticStringBase<Char,SIZE>& string) const
{
    return ( TbStrCmp(m_Buffer,string.m_Buffer) == 0 );
}

/*!
 * 比較演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
bool TbStaticStringBase<Char,SIZE>::operator!=(const TbStaticStringBase<Char,SIZE>& string) const
{
    return TbStrCmp(m_Buffer,string.m_Buffer) != 0;
}

/*!
 * <比較演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
bool TbStaticStringBase<Char,SIZE>::operator<(const TbStaticStringBase<Char,SIZE>& string) const
{
    return TbStrCmp(m_Buffer,string.m_Buffer) < 0;
}

/*!
 * <=比較演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
bool TbStaticStringBase<Char,SIZE>::operator<=(const TbStaticStringBase<Char,SIZE>& string) const
{
    return TbStrCmp(m_Buffer,string.m_Buffer) < 0;
}

/*!
 * >比較演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
bool TbStaticStringBase<Char,SIZE>::operator>(const TbStaticStringBase<Char,SIZE>& string) const
{
    return TbStrCmp(m_Buffer,string.m_Buffer) > 0;
}

/*!
 * >=比較演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
bool TbStaticStringBase<Char,SIZE>::operator>=(const TbStaticStringBase<Char,SIZE>& string) const
{
    return TbStrCmp(m_Buffer,string.m_Buffer) >= 0;
}

/*!
 * 配列演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
const Char& TbStaticStringBase<Char,SIZE>::operator[](TbSint32 index) const
{
    return m_Buffer[index];
}

/*!
 * 配列演算子
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
Char& TbStaticStringBase<Char,SIZE>::operator[](TbSint32 index)
{
    return m_Buffer[index];
}

/*!
 * フォーマット生成
 * @author Miyake Shunsuke
 * @since 2010.06.25
 */
template <typename Char,TbUint32 SIZE>
void  TbStaticStringBase<Char,SIZE>::format( TbStaticStringBase<Char,SIZE>& dst , const char* format , va_list argList )
{
    char    buf[ TB_MAX_PATH ];
    TbVStrPrintf( buf , TB_MAX_PATH , format , argList );
    dst = buf;
}

/*!
 * フォーマット形式で生成
 * @author Miyake Shunsuke
 * @since 2010.06.25
 */
template <typename Char,TbUint32 SIZE>
TbStaticStringBase<Char,SIZE> TbStaticStringBase<Char,SIZE>::Format( const char* fmt , ... )
{
    TbStaticStringBase<Char,SIZE> string;
    va_list ap;
    va_start( ap, fmt );
    format( string , fmt , ap );
    va_end( ap );
    return string;
}

/*!
 * クリア
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
void TbStaticStringBase<Char,SIZE>::Clear()
{
    m_Buffer[0] = 0;
//    TbMemClear(m_Buffer,sizeof(m_Buffer));
}

/*!
 * フォーマットで値設定
 * @author Miyake Shunsuke
 */
template <typename Char,TbUint32 SIZE>
void TbStaticStringBase<Char,SIZE>::SetFormatedString(const char* format, ...)
{
    va_list ap;
    va_start( ap, format );
    TbStaticStringBase<Char,SIZE>::format( (*this) , format , ap );
    va_end( ap );
}


