/*!
 * @file
 * @brief 文字列関数郡
 * @author Miyake Shunsuke
 */

namespace toybox
{

/*!
 * 文字列コピー
 * @param dst コピー先
 * @param dst コピー先サイズ
 * @param src コピー元
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbStrCopy(TbChar8* dst,TbSizeT dstCount,const TbChar8* src)
{
#if (TB_PLATFORM_IS_WIN)
    strcpy_s(dst,dstCount,src);
#else
    strncpy(dst,src,dstCount);
#endif
}

// 文字列コピー
TB_FORCE_INLINE void TbStrCopy(TbChar8* dst , TbSizeT dstCount,const TbChar8* src,TbUint32 srcCount)
{
#if (TB_PLATFORM_IS_WIN)
    strncpy_s(dst,dstCount,src,srcCount);
#else
    strncpy(dst,src,srcCount);
#endif
}

#if TB_CHAR16_IS_WCHAR
/*!
 * 文字列コピー
 * @param dst コピー先
 * @param dst コピー先サイズ
 * @param src コピー元
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbStrCopy( TbChar16* dst , TbSizeT dstCount , const TbChar8* src )
{
#if (TB_PLATFORM_IS_WIN)
    size_t result = 0;
    mbstowcs_s( &result , dst , dstCount , src , _TRUNCATE );
#else
    mbstowcs( dst , src , dstCount );
#endif
}

/*!
 * 文字列コピー
 * @param dst コピー先
 * @param dst コピー先サイズ
 * @param src コピー元
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbStrCopy( TbChar8* dst , TbSizeT dstCount , const TbChar16* src )
{
#if (TB_PLATFORM_IS_WIN)
    size_t result = 0;
    errno_t error = wcstombs_s( &result , dst , dstCount , src , _TRUNCATE );
#else
    wcstombs( dst , src , dstCount );
#endif
}
#endif

/*!
 * 文字列比較
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE TbSint32 TbStrCmp(const TbChar8* str1,const TbChar8* str2)
{
    return strcmp(str1,str2);
}

/*!
 * 文字列比較(文字数指定)
 */
TB_FORCE_INLINE TbSint32 TbStrCmp(const TbChar8* str1,const TbChar8* str2, TbSizeT count)
{
    return strncmp(str1,str2,count);
}

/*!
 * 文字列サイズ
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE TbSint32 TbStrCharCount(const TbChar8* str)
{
    return static_cast<TbSint32>(strlen(str));
}

/*!
 * 文字列サイズ
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE TbSint32 TbStrBufSize(const TbChar8* str)
{
    return static_cast<TbSint32>(strlen(str) + 1);
}

/*!
 * 文字列検索(逆から)
 * @parma str 元データ
 * @param search 検索文字列
 * @author Miyake Shunsuke
 * @since 2011.07.13
 */
TB_FORCE_INLINE TbSint32 TbStrFind( const char* str , const char* search )
{
    TbSint32 result = -1;
    if( !str || '\0' == str[0] || !search || '\0' == search[0] ){
        return result;
    }
    const char* candidate = NULL;
    const char* p = str;
    const char* pattern = search;
    do{
        if( *p == *pattern ){
            if( !candidate ){
                candidate = p;
            }
            ++pattern;
            if( '\0' == *pattern ){
                result = static_cast<TbSint32>(( candidate - str ) / sizeof( *p ));
                break;
            }
        }else{
            candidate = NULL;
            pattern = search;
        }
        ++p;
    }while( *p != '\0' );
    return result;
}

/*!
 * 文字列検索(逆から)
 * @parma str 元データ
 * @param search 検索文字列
 * @author Miyake Shunsuke
 * @since 2011.07.13
 */
TB_FORCE_INLINE TbSint32 TbStrReverseFind( const char* str , const char* search )
{
    TbSint32 result = -1;
    if( !str || '\0' == str[0] || !search || '\0' == search[0] ){
        return result;
    }
    TbUint32 count = TbStrCharCount(str);
    const char* candidate = NULL;
    const char* base = &str[count-1];
    const char* p = &str[count-1];
    const char* pattern = search;
    while( p != str ){
        do{
            if( *p == *pattern ){
                if( !candidate ){
                    candidate = p;
                }
                ++pattern;
                if( '\0' == *pattern ){
                    result = static_cast<TbSint32>(( candidate - str ) / sizeof( *p ));
                    break;
                }
            }else{
                candidate = NULL;
                pattern = search;
                break;
            }
            ++p;
        }while( *p != '\0' );
        if( result >= 0 ){
            break;
        }
        --base;
        p = base;
    }
    return result;
}

/*!
 * 文字列設定
 * @author Miyake Shunsuke
 * @since 2011.11.24
 */
TB_FORCE_INLINE TbSint32 TbStrPrintf( TbChar8* buffer , TbUint32 count , const TbChar8 *format , ... )
{
    va_list ap;
    va_start( ap, format );
    TbSint32 result = TbVStrPrintf( buffer , count , format , ap );
    va_end( ap );
    return result;
}

/*!
 * 引数リストから文字列書き込み
 * @author Miyake Shunsuke
 * @since 2011.08.17
 */
TB_FORCE_INLINE TbSint32 TbVStrPrintf( TbChar8 *buffer , TbUint32 numberOfElements , const TbChar8 *format , va_list argptr )
{
#if (TB_PLATFORM_IS_WIN)
    return vsprintf_s( buffer , numberOfElements , format , argptr );
#else
    return vsnprintf( buffer , numberOfElements , format , argptr );
#endif
}

#if TB_CHAR16_IS_WCHAR
/*!
 * ワイド文字列コピー
 * @param dst コピー先
 * @param dst コピー先サイズ
 * @param src コピー元
 */
TB_FORCE_INLINE void TbStrCopy(TbChar16* dst,TbUint32 dstCount,const TbChar16* src)
{
#if (TB_PLATFORM_IS_WIN)
    wcscpy_s(dst,dstCount,src);
#else
    wcsncpy(dst,src,dstCount);
#endif
}

/*!
 * ワイド文字列比較
 */
TB_FORCE_INLINE TbSint32 TbStrCmp(const TbChar16* str1,const TbChar16* str2)
{
    return wcscmp(str1,str2);
}

/*!
 * ワイド文字列サイズ
 */
TB_FORCE_INLINE TbSint32 TbStrCharCount(const TbChar16* str)
{
    return wcslen(str);
}

/*!
 * ワイド文字列サイズ
 */
TB_FORCE_INLINE TbSint32 TbStrBufSize(const TbChar16* str)
{
    return (wcslen(str) + 1)*sizeof(TbChar16);
}

/*!
 * 引数リストから文字列書き込み
 * @author Miyake Shunsuke
 * @since 2011.08.17
 */
TB_FORCE_INLINE TbSint32 TbVStrPrintf( TbChar16 *buffer , TbUint32 numberOfElements , const TbChar16 *format , va_list argptr )
{
#if (TB_PLATFORM_IS_WIN)
    return vswprintf_s( buffer , numberOfElements , format , argptr );
#else
    return vswprintf( buffer , numberOfElements , format , argptr );
#endif
}
#endif

}