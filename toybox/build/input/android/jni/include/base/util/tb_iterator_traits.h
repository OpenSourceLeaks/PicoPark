/*!
 * Iterator特性取得
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_ITERATOR_TRAITS_H_
#define _INCLUDED_TB_ITERATOR_TRAITS_H_

namespace toybox
{
    
template<typename Ite>
struct TbIteratorTraits
{
    typedef typename Ite::ValueType ValueType;
};

template<typename T>
struct TbIteratorTraits<T*>
{
    typedef T ValueType;
};

template<typename T>
struct TbIteratorTraits<const T*>
{
    typedef T ValueType;
};

// Iteratorの格納型をポインターで返す
template<typename Ite> inline
typename TbIteratorTraits<Ite>::ValueType* TbGetIteValue(Ite)
{
    return (NULL);
}
    
}

#endif
