/*!
 * �^����*��&����������
 * @author Miyake_Shunsuke
 * @since 2012.05.09
 */

#ifndef _INCLUDED_TB_TYPE_REMOVE_H_
#define _INCLUDED_TB_TYPE_REMOVE_H_

namespace toybox
{

// �|�C���^�[����
template <typename T>
struct TbRemovePointer
{
    typedef T Type;
};
    
template <typename T>
struct TbRemovePointer<T*>
{
    typedef T Type;
};
    
template <typename T>
struct TbRemovePointer<T* const>
{
    typedef T Type;
};

template <typename T>
struct TbRemovePointer<T* volatile>
{
    typedef T Type;
};

template <typename T>
struct TbRemovePointer<T* const volatile>
{
    typedef T Type;
};

// const����
template<typename T>
struct TbRemoveConst
{
    typedef T Type;
};

template<typename T>
struct TbRemoveConst<const T>
{
    typedef T Type;
};

template<typename T>
struct TbRemoveConst<const T[]>
{
    typedef T Type[];
};

template<typename T,TbUint32 N>
struct TbRemoveConst<const T[N]>
{
    typedef T Type[N];
};

// volatile����
template<typename T>
struct TbRemoveVolatile
{
    typedef T Type;
};

template<typename T>
struct TbRemoveVolatile<volatile T>
{
    typedef T Type;
};

template<typename T>
struct TbRemoveVolatile<volatile T[]>
{
    typedef T Type[];
};

template<typename T,TbUint32 N>
struct TbRemoveVolatile<volatile T[N]>
{
    typedef T Type[N];
};

// const��volatile����
template<typename T>
struct TbRemoveCV
{
    typedef typename TbRemoveConst<
                typename TbRemoveVolatile<T>::Type>::Type Type;
};

// �Q�Ə���
template<typename T>
struct TbRemoveReference
{
    typedef T Type;
};

template<typename T>
struct TbRemoveReference<T&>
{
    typedef T Type;
};

// �z�񕔕�����
template<typename T>
struct TbRemoveExtent
{
    typedef T type;
};

template<typename T, unsigned int N>
struct TbRemoveExtent<T[N]>
{
    typedef T type;
};

template<typename T>
struct TbRemoveExtent<T[]>
{
    typedef T type;
};

template<typename T>
struct TbRemoveAllExtents
{
    typedef T type;
};

template<typename T, unsigned int N>
struct TbRemoveAllExtents<T[N]>
{
    typedef typename TbRemoveAllExtents<T>::type type;
};

template<class T>
struct TbRemoveAllExtents<T[]>
{
    typedef typename TbRemoveAllExtents<T>::type type;
};

}

#endif
