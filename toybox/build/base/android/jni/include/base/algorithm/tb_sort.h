/*!
 * ソートアルゴリズムテンプレート
 * @author Miyake Shunsuke
 * @since 2011.03.06
 */

#ifndef _INCLUDED_TB_SORT_H_
#define _INCLUDED_TB_SORT_H_

#include <base/util/tb_iterator_traits.h>

namespace toybox
{
    
/*!
 * バブルソート関数
 * @author Miyake Shunsuke
 * @since 2011.03.07
 */
template<typename T>
TB_FORCE_INLINE TbBool TbBubbleSort( T begin , T end )
{
    if( begin >= end ){
        return TB_FALSE;
    }
    T currentBegin = begin+1;
    while( currentBegin < end ){
        T current = currentBegin;
        while( current < end ){
            // 一つ下が大きければ交換
            if( *current < *(current-1) ){
                typename TbIteratorTraits<T>::ValueType temp = *(current-1);
                *(current-1) = *current;
                *current = temp;
            }
        }
        // 参照先を変更
        ++currentBegin;
    }
    return TB_TRUE;
}

/*!
 * バブルソート関数( ポインタ )
 * @author Miyake Shunsuke
 * @since 2011.03.07
 */
template<typename T,typename CompareFunc>
TB_FORCE_INLINE TbBool TbBubbleSort( T begin , T end , CompareFunc compareFunc )
{
    if( begin >= end ){
        return TB_FALSE;
    }
    T currentBegin = begin+1;
    while( currentBegin < end ){
        T current = currentBegin;
        while( current < end ){
            // 一つ下が大きければ交換
            if( !compareFunc( *(current-1) , *current ) ){
                typename TbIteratorTraits<T>::ValueType temp = *(current-1);
                *(current-1) = *current;
                *current = temp;
            }
        }
        // 参照先を変更
        ++currentBegin;
    }
    return TB_TRUE;
}


/*!
 * 挿入ソート関数
 * 比較関数は第一引数が小さい場合TRUEを返す
 * @author Miyake Shunsuke
 * @since 2011.03.07
 */
template<typename T>
TB_FORCE_INLINE TbBool TbInsertionSort( T begin , T end )
{
    if( begin >= end ){
        return TB_FALSE;
    }
    T currentBegin = begin+1;
    while( currentBegin < end ){
        T current = currentBegin;
        typename TbIteratorTraits<T>::ValueType value = *current;
        // 一つ下が大きければ、再挿入
        if( value < *(current-1) ){
            T offset = current;
            do {
                *offset = *(offset-1);
                --offset;
            }while( begin < offset && value < *(offset-1) );
            *offset = value;
        }
        ++currentBegin;
    }
    return TB_TRUE;
}

/*!
 * 挿入ソート関数(ポインタ)
 * @author Miyake Shunsuke
 * @since 2011.03.07
 */
template<typename T,typename CompareFunc>
TB_FORCE_INLINE TbBool TbInsertionSort( T begin , T end , CompareFunc compareFunc )
{
    if( begin >= end ){
        return TB_FALSE;
    }
    T currentBegin = begin+1;
    while( currentBegin < end ){
        T current = currentBegin;
        typename TbIteratorTraits<T>::ValueType value = *current;
        // 一つ下が大きければ、再挿入
        if( compareFunc( value , *(current-1) ) ){
            T offset = current;
            do {
                *offset = *(offset-1);
                --offset;
            }while( begin < offset && compareFunc( value , *(offset-1) ) );
            *offset = value;
        }
        ++currentBegin;
    }
    return TB_TRUE;
}

}

#endif
