/*!
 * �^����*��&��ǉ�����
 * @author Miyake_Shunsuke
 * @since 2012.05.09
 */

#ifndef _INCLUDED_TB_TYPE_ADD_H_
#define _INCLUDED_TB_TYPE_ADD_H_

namespace toybox
{

// �|�C���^�[�ǉ�
template <typename T>
struct TbAddPointer
{
    typedef typename TbRemovePointer<T>::Type* Type;
};

// const�ǉ�
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
    // T& const�ɂ͈Ӗ����Ȃ��̂�
    typedef T& Type;
};

// volatile�ǉ�
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

// const��volatile����
template<typename T>
struct TbAddCV
{
    typedef typename TbAddConst<
                typename TbAddVolatile<T>::Type>::Type Type;
};

// �Q�ƒǉ�
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
