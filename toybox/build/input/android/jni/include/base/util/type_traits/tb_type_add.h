/*!
 * 型から*や&を追加する
 * @author Miyake_Shunsuke
 * @since 2012.05.09
 */

#ifndef _INCLUDED_TB_TYPE_ADD_H_
#define _INCLUDED_TB_TYPE_ADD_H_

namespace toybox
{

// ポインター追加
template <typename T>
struct TbAddPointer
{
    typedef typename TbRemovePointer<T>::Type* Type;
};

// const追加
template<typename T>
struct TbAddConst
{
    typedef T Type;
};

template<typename T>
struct TbAddConst<const T>
{
    typedef const T Type;
};

template<typename T>
struct TbAddConst<T&>
{
    // T& constには意味がないので
    typedef T& Type;
};

// volatile追加
template<typename T>
struct TbAddVolatile
{
    typedef volatile T Type;
};

template<typename T>
struct TbAddVolatile<volatile T>
{
    typedef volatile T Type;
};

template<typename T>
struct TbAddVolatile<T&>
{
    typedef T& Type;
};

// constとvolatile除去
template<typename T>
struct TbAddCV
{
    typedef typename TbAddConst<
                typename TbAddVolatile<T>::Type>::Type Type;
};

// 参照追加
template<typename T>
struct TbAddReference
{
    typedef typename TbRemoveReference<T>::Type& Type;
};

template<>
struct TbAddReference<void>
{
    typedef void Type;
};

template<>
struct TbAddReference<const void>
{
    typedef const void Type;
};

template<>
struct TbAddReference<const volatile void>
{
    typedef const volatile void Type;
};

}

#endif
