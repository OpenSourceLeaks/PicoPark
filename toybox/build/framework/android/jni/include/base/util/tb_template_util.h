/*!
 * テンプレート系ユーティリティ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_TEMPLATE_UTIL_H_
#define _INCLUDED_TB_TEMPLATE_UTIL_H_

namespace toybox
{

// 整数定数
template<typename T,T val>
struct TbIntegralConstant
{
    static const T VALUE = val;
    typedef T                         ValueType;    // 値の型
    typedef TbIntegralConstant<T,val> Type;         // 自分自身の型
};
typedef TbIntegralConstant<bool,true>  TbTrueType;  // true型
typedef TbIntegralConstant<bool,false> TbFalseType; // false型

// boolに分岐する型
template<bool>
struct TbBoolType;

template<>
struct TbBoolType <true> 
    : TbTrueType
{};

template<>
struct TbBoolType <false> 
    : TbFalseType
{};

// テンプレートOR
template <bool first,bool second>
struct TbOr;

template<>
struct TbOr<false,false>
    : TbFalseType
{};

template<>
struct TbOr<true,false>
    : TbTrueType
{};

template<>
struct TbOr<false,true>
    : TbTrueType
{};

}

#endif
