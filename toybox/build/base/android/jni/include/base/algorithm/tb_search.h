/*!
 * 検索アルゴリズムテンプレート
 * @author Miyake Shunsuke
 * @since 2011.03.06
 */

#ifndef _INCLUDED_TB_SEARCH_H_
#define _INCLUDED_TB_SEARCH_H_

namespace toybox
{

    template<typename T, TbSint32 N>
    T* TbBegin(T(&ary)[N])
    {
        return &ary[0];
    }

    template<typename T, TbSint32 N>
    const T* TbBegin(const T(&ary)[N])
    {
        return &ary[0];
    }

    template<typename T, TbSint32 N>
    T* TbEnd(T(&ary)[N])
    {
        return &ary[N];
    }
    template<typename T, TbSint32 N>
    const T* TbEnd(const T(&ary)[N])
    {
        return &ary[N];
    }

    template <typename T>
    struct TbLess {
        TbBool operator() ( const T& left , const T& right ) const {
            return left < right;
        }
    };

    //! findテンプレート関数(コンテナ専用)
    template <typename Container,typename T>
    TB_FORCE_INLINE typename Container::Iterator TbFind(Container& container,T value){
        typename Container::Iterator begin = container.Begin();
        typename Container::Iterator end = container.End();
        for(; begin != end; ++begin){
            if( *begin == value ){
                return begin;
            }
        }
        return end;
    }

    //! findテンプレート関数
    template <typename Iterator,typename T>
    TB_FORCE_INLINE Iterator TbFind(Iterator begin,Iterator end,T value){
        for(; begin != end; ++begin){
            if( *begin == value ){
                return begin;
            }
        }
        return end;
    }

    //! findテンプレート関数(配列用
    template <typename T,size_t N>
    TB_FORCE_INLINE T* TbFind(T (&container)[N],T value){
        T* begin = &container[0];
        T* end = &container[N];
        for (; begin != end; ++begin) {
            if (*begin == value) {
                return begin;
            }
        }
        return end;
    }

    //! findテンプレート関数(配列用(const用
    template <typename T, size_t N>
    TB_FORCE_INLINE const T* TbFind(const T(&container)[N], T value) {
        const T* begin = &container[0];
        const T* end = &container[N];
        for (; begin != end; ++begin) {
            if (*begin == value) {
                return begin;
            }
        }
        return end;
    }

    //! findテンプレート関数
    template <typename Iterator,typename F>
    TB_FORCE_INLINE Iterator TbFindIf(Iterator begin,Iterator end,F func){
        for(; begin != end; ++begin){
            if( func(*begin) ){
                return begin;
            }
        }
        return end;
    }

    //! findテンプレート関数
    template <typename Container,typename F>
    TB_FORCE_INLINE typename Container::Iterator TbFindIf(Container& container,F func){
        typename Container::Iterator begin = container.Begin();
        typename Container::Iterator end = container.End();
        for(; begin != end; ++begin){
            if( func(*begin) ){
                return begin;
            }
        }
        return end;
    }

    //! findテンプレート関数(配列用
    template <typename T,size_t N,typename F>
    TB_FORCE_INLINE T* TbFindIf(T (&container)[N],F func){
        return TbFindIf( &container[0] , &container[N] , func );
    }

    /*!
     * ソート済み配列で、指定した値以上の要素が現れる位置を返す( ValueTypeのゲッター関数指定 )
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE T TbLowerBound( T begin , T end , const CompareValueType& value )
    {
        T low = begin;
        T high = end;
        while( low < high ){
            T middle = low + ( high - low ) / 2;
            if ( *middle < value ){
                low = middle + 1;
            }else{
                high = middle;
            }
        }
        return low;
    }

    /*!
     * ソート済み配列で、指定した値以上の要素が現れる位置を返す( ValueTypeのゲッター関数指定 )
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE T TbLowerBound( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T low = begin;
        T high = end;
        while( low < high ){
            T middle = low + ( high - low ) / 2;
            if ( func(*middle,value) ){
                low = middle + 1;
            }else{
                high = middle;
            }
        }
        return low;
    }

    /*!
     * ソート済み配列で、指定した値以上の要素が現れる位置を返す( ValueTypeのゲッター関数指定 )
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE TbSint32 TbLowerBoundIndex( T begin , T end , const CompareValueType& value )
    {
        T result = TbLowerBound(begin,end,value);
        return result - begin;
    }

    /*!
     * ソート済み配列で、指定した値以上の要素が現れる位置を返す( ValueTypeのゲッター関数指定 )
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE TbSint32 TbLowerBoundIndex( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T result = TbLowerBound(begin,end,value,func);
        return result - begin;
    }

    /*!
     * ソート済み配列で、指定した値より"大きい"要素が現れる位置を返す
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE T TbUpperBound( T begin , T end , const CompareValueType& value )
    {
        T low = begin;
        T high = end;
        while( low < high ){
            T middle = low + ( high - low ) / 2;
            if ( !(value < *middle) ){
                low = middle + 1;
            }else{
                high = middle;
            }
        }
        return low;
    }

    /*!
     * ソート済み配列で、指定した値より"大きい"要素が現れる位置を返す( 比較関数指定 )
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE T TbUpperBound( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T low = begin;
        T high = end;
        while( low < high ){
            T middle = low + ( high - low ) / 2;
            if ( !func(value,*middle) ){
                low = middle + 1;
            }else{
                high = middle;
            }
        }
        return low;
    }

    /*!
     * ソート済み配列で、指定した値より"大きい"要素が現れる位置を返す
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE TbSint32 TbUpperBoundIndex( T begin , T end , const CompareValueType& value )
    {
        T result = TbUpperBound(begin,end,value);
        return result - begin;
    }

    /*!
     * ソート済み配列で、指定した値より"大きい"要素が現れる位置を返す( 比較関数指定 )
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE TbSint32 TbUpperBoundIndex( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T result = TbUpperBound(begin,end,value,func);
        return result - begin;
    }

    /*!
     * ソート済み配列を二分探索で検索
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE TbBool TbBinarySearch( T begin , T end , const CompareValueType& value )
    {
        T result = TbLowerBound(begin,end,value);
        return (result != end) && *result == value;
    }

    /*!
     * ソート済み配列を二分探索で検索
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE TbBool TbBinarySearch( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T result = TbLowerBound(begin,end,value);
        return (result != end) && !func(value,*result);
    }

    /*!
     * ソート済み配列を二分探索で検索
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE T TbBinarySearchIte( T begin , T end , const CompareValueType& value )
    {
        T result = TbLowerBound(begin,end,value);
        if( (result != end) && *result == value ) {
            return result;
        }
        return end;
    }

    /*!
     * ソート済み配列を二分探索で検索
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE T TbBinarySearchIte( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T result = TbLowerBound(begin,end,value,func);
        if( (result != end) && !func(value,*result) ){
            return result;
        }
        return end;
    }

    /*!
     * ソート済み配列を二分探索で検索
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType>
    TB_FORCE_INLINE TbSint32 TbBinarySearchIndex( T begin , T end , const CompareValueType& value )
    {
        T result = TbLowerBound(begin,end,value);
        if( (result != end) && *result == value ) {
            return (result - begin);
        }
        return -1;
    }

    /*!
     * ソート済み配列を二分探索で検索
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename CompareValueType,typename CompareFunc>
    TB_FORCE_INLINE TbSint32 TbBinarySearchIndex( T begin , T end , const CompareValueType& value , CompareFunc func )
    {
        T result = TbLowerBound(begin,end,value,func);
        if( (result != end) && !func(value,*result) ){
            return (result - begin);
        }
        return -1;
    }

}

#endif
