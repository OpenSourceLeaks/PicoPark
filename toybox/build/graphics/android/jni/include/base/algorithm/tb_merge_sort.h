/*!
 * マージソートアルゴリズムテンプレート
 * @author Miyake Shunsuke
 * @since 2011.03.06
 */

#ifndef _INCLUDED_TB_MERGE_SORT_H_
#define _INCLUDED_TB_MERGE_SORT_H_

namespace toybox
{

    /*!
     * マージソート関数
     * 比較関数は第一引数が小さい場合TRUEを返す
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename ValueType>
    TbBool TbMergeSort( T begin , T end , TbBool (*compareFunc)(const ValueType& left,const ValueType& right) )
    {
        if( begin >= end || !compareFunc ){
            return TB_FALSE;
        }
        
        T mid = (begin+end)/2;
        
        T currentBegin = begin+1;
        TbMergeSort(begin,mid);
        TbMergeSort(mid+1,end);
        
        

        while( currentBegin < end ){
            T current = currentBegin;
            ValueType value = *current;
            // 一つ下が大きければ、再挿入
            if( !compareFunc( *(current-1) , value ) ){
                T offset = current;
                do {
                    *offset = *(offset-1);
                    --offset;
                }while( offset > begin && !compareFunc( *(offset-1) , value ) );
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
    template<typename T,typename ValuePtrType>
    TbBool TbMergeSort( T begin , T end , TbBool (*compareFunc)(const ValuePtrType* left,const ValuePtrType* right) )
    {
        if( begin >= end || !compareFunc ){
            return TB_FALSE;
        }
        T currentBegin = begin+1;
        while( currentBegin < end ){
            T current = currentBegin;
            ValuePtrType* value = *current;
            // 一つ下が大きければ、再挿入
            if( !compareFunc( *(current-1) , value ) ){
                T offset = current;
                do {
                    *offset = *(offset-1);
                    --offset;
                }while( offset > begin && !compareFunc( *(offset-1) , value ) );
                *offset = value;
            }
            ++currentBegin;
        }
        return TB_TRUE;
    }

    /*!
     * 挿入ソート関数
     * @author Miyake Shunsuke
     * @since 2011.03.07
     */
    template<typename T,typename ValueType>
    TbBool TbInsertionSort( T begin , T end )
    {
        if( begin >= end ){
            return TB_FALSE;
        }
        T currentBegin = begin+1;
        while( currentBegin < end ){
            T current = currentBegin;
            ValueType value = *current;
            // 一つ下が大きければ、再挿入
            if( *(current-1) > value ){
                T offset = current;
                do {
                    *offset = *(offset-1);
                    --offset;
                }while( offset > begin && *(offset-1) > value );
                *offset = value;
            }
            ++currentBegin;
        }
        return TB_TRUE;
    }
}

#endif
