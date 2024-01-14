/*!
 * 型特性クラス
 * 右辺値参照は非対応
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TYPE_TRAITS_H_
#define _INCLUDED_TYPE_TRAITS_H_

#include <base/util/tb_template_util.h>
#include <base/util/type_traits/tb_type_remove.h>
#include <base/util/type_traits/tb_type_add.h>

// enum型識別を利用可能かどうか
#if TB_PLATFORM_IS_FLASH
#   define TB_IS_ENABLE_ENUM_TYPE_TRAITS 0
#else
#   define TB_IS_ENABLE_ENUM_TYPE_TRAITS 1
#endif

namespace toybox
{

// 型分岐
template<bool test,typename T1,typename T2>
struct TbTypeConditional
{
    typedef T2 Type;
};

template<typename T1,typename T2>
struct TbTypeConditional<true,T1,T2>
{
    typedef T1 Type;
};

// 整数かどうか
template<class T>
struct TbIsIntegralBase : TbFalseType
{};

template<>
struct TbIsIntegralBase<bool> : TbTrueType
{};

template<>
struct TbIsIntegralBase<signed char> : TbTrueType
{};

template<>
struct TbIsIntegralBase<unsigned char> : TbTrueType
{};

template<>
struct TbIsIntegralBase<signed short> : TbTrueType
{};

template<>
struct TbIsIntegralBase<unsigned short> : TbTrueType
{};

template<>
struct TbIsIntegralBase<signed int> : TbTrueType
{};

template<>
struct TbIsIntegralBase<unsigned int> : TbTrueType
{};

template<>
struct TbIsIntegralBase<signed long> : TbTrueType
{};

template<>
struct TbIsIntegralBase<unsigned long> : TbTrueType
{};

template<>
struct TbIsIntegralBase<signed long long> : TbTrueType
{};

template<>
struct TbIsIntegralBase<unsigned long long> : TbTrueType
{};

template<typename T>
struct TbIsIntegral
    : TbIsIntegralBase<typename TbRemoveCV<T>::Type>
{};

// 符号付か
template<class T>
struct TbIsSignedIntegralBase : TbFalseType
{};

template<>
struct TbIsSignedIntegralBase<signed char> : TbTrueType
{};

template<>
struct TbIsSignedIntegralBase<signed short> : TbTrueType
{};

template<>
struct TbIsSignedIntegralBase<signed int> : TbTrueType
{};

template<>
struct TbIsSignedIntegralBase<signed long> : TbTrueType
{};

template<>
struct TbIsSignedIntegralBase<signed long long> : TbTrueType
{};

template<typename T>
struct TbIsSignedIntegral
    : TbIsSignedIntegralBase<typename TbRemoveCV<T>::Type>
{};

// 浮動少数か
template<class T>
struct TbIsFloatingPointBase : TbFalseType
{};

template<>
struct TbIsFloatingPointBase<float> : TbTrueType
{};

template<>
struct TbIsFloatingPointBase<double> : TbTrueType
{};

template<typename T>
struct TbIsFloatingPoint
    : TbIsFloatingPointBase<typename TbRemoveCV<T>::Type>
{};

// voidかどうか
template<typename T>
struct TbIsVoidBase
    : TbFalseType
{};

template<>
struct TbIsVoidBase<void>
    : TbTrueType
{};

template<typename T>
struct TbIsVoid
    : TbIsVoidBase<typename TbRemoveCV<T>::Type>
{};

// 配列かどうか
template<typename T>
struct TbIsArray : TbFalseType 
{};

template<typename T,size_t N>
struct TbIsArray<T[N]> : TbTrueType 
{};

template<typename T>
struct TbIsArray<T[]> : TbTrueType 
{};

// 参照かどうか
template<typename T>
struct TbIsReference : TbFalseType 
{};

template<typename T>
struct TbIsReference<T&> : TbTrueType 
{};

// 数字か
template<typename T>
struct TbIsArithmetic
    : TbBoolType<TbIsIntegral<T>::VALUE || TbIsFloatingPoint<T>::VALUE >
{};

// enumか
#if TB_IS_ENABLE_ENUM_TYPE_TRAITS
template<typename T>
struct TbIsEnum
    : TbBoolType<__is_enum(T)>
{};
#endif

// 関数ポインターかどうか
template<typename T>
struct TbIsFunctionPointerBase
    : TbFalseType
{};

template<typename R>
struct TbIsFunctionPointerBase<R (*)()>
    : TbTrueType
{};

template<typename R,typename A0>
struct TbIsFunctionPointerBase<R (*)(A0)>
    : TbTrueType
{};

template<typename R,typename A0,typename A1>
struct TbIsFunctionPointerBase<R (*)(A0,A1)>
    : TbTrueType
{};

template<typename R,typename A0,typename A1,typename A2>
struct TbIsFunctionPointerBase<R (*)(A0,A1,A2)>
    : TbTrueType
{};

template<typename R,typename A0,typename A1,typename A2,typename A3>
struct TbIsFunctionPointerBase<R (*)(A0,A1,A2,A3)>
    : TbTrueType
{};

template<typename R,typename A0,typename A1,typename A2,typename A3,typename A4>
struct TbIsFunctionPointerBase<R (*)(A0,A1,A2,A3,A4)>
    : TbTrueType
{};

template<typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5>
struct TbIsFunctionPointerBase<R (*)(A0,A1,A2,A3,A4,A5)>
    : TbTrueType
{};

template<typename T>
struct TbIsFunctionPointer
    : TbIsFunctionPointerBase<typename TbRemoveCV<T>::Type>
{};

// メンバ関数ポインターかどうか
template<typename T>
struct TbIsMemberFunctionPointerBase
    : TbFalseType
{};

template<typename R,typename T>
struct TbIsMemberFunctionPointerBase<R (T::*)()>
    : TbTrueType
{};

template<typename R,typename T,typename A0>
struct TbIsMemberFunctionPointerBase<R (T::*)(A0)>
    : TbTrueType
{};

template<typename R,typename T,typename A0,typename A1>
struct TbIsMemberFunctionPointerBase<R (T::*)(A0,A1)>
    : TbTrueType
{};

template<typename R,typename T,typename A0,typename A1,typename A2>
struct TbIsMemberFunctionPointerBase<R (T::*)(A0,A1,A2)>
    : TbTrueType
{};

template<typename R,typename T,typename A0,typename A1,typename A2,typename A3>
struct TbIsMemberFunctionPointerBase<R (T::*)(A0,A1,A2,A3)>
    : TbTrueType
{};

template<typename R,typename T,typename A0,typename A1,typename A2,typename A3,typename A4>
struct TbIsMemberFunctionPointerBase<R (T::*)(A0,A1,A2,A3,A4)>
    : TbTrueType
{};

template<typename R,typename T,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5>
struct TbIsMemberFunctionPointerBase<R (T::*)(A0,A1,A2,A3,A4,A5)>
    : TbTrueType
{};

template<typename T>
struct TbIsMemberFunctionPointer
    : TbIsMemberFunctionPointerBase<typename TbRemoveCV<T>::Type>
{};

// メンバオブジェクトポインターかどうか
template<typename T>
struct TbIsMemberObjectPointerBase
    : TbFalseType
{};

template<typename T1, typename T2>
struct TbIsMemberObjectPointerBase<T1 T2::*>
    : TbBoolType<!TbIsMemberFunctionPointer<T1 T2::*>::VALUE>
{};

template<typename T>
struct TbIsMemberObjectPointer
    : TbIsMemberObjectPointerBase<typename TbRemoveCV<T>::Type>
{};

// メンバーポインターかどうか
template<typename T>
struct TbIsMemberPointer
    : TbBoolType<TbIsMemberObjectPointer<T>::VALUE || 
                 TbIsMemberFunctionPointer<T>::VALUE>
{};

// ポインターかどうか
template<typename T>
struct TbIsPointerBase
    : TbFalseType
{};

template<typename T>
struct TbIsPointerBase<T*>
    : TbBoolType<!TbIsMemberObjectPointer<T*>::VALUE &&
                 !TbIsMemberFunctionPointer<T*>::VALUE>
{};

template<typename T>
struct TbIsPointer
    : TbIsPointerBase<typename TbRemoveCV<T>::Type>
{};

// スカラー値かどうか
template<typename T>
struct TbIsScalar
    : TbBoolType<TbIsArithmetic<T>::VALUE || 
#if TB_IS_ENABLE_ENUM_TYPE_TRAITS
                 TbIsEnum<T>::VALUE || 
#endif
                 TbIsPointer<T>::VALUE ||
                 TbIsMemberPointer<T>::VALUE>
{};

template<typename T>
struct TbIsScalar<T&>
    : TbFalseType
{};

}

#endif
