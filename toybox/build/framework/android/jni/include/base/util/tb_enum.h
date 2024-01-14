/*!
 * 任意型でenumを管理するためのクラス
 * @author Miyake Shunsuke
 * @since 2012.02.28
 */

#ifndef _INCLUDED_TB_ENUM_H_
#define _INCLUDED_TB_ENUM_H_

namespace toybox
{

template<typename EnumType,typename InnerType>
class TbEnum
{
    typedef TbEnum Self;
public:

    // コンストラクタ
    TbEnum(EnumType value) {
        m_Value = static_cast<InnerType>(value);
    }

    // デストラクタ
    ~TbEnum(){}

public:

    //! 代入インクリメント
    template <typename T>
    TbEnum& operator=(T value){
        m_Value = value;
        return *this;
    }

    //! 比較演算子
    template <typename T>
    TbBool operator==(T value){
        return m_Value == value;
    }

    //! 比較演算子
    template <typename T>
    TbBool operator!=(T value){
        return m_Value != value;
    }

    //! 加算
    template <typename T>
    TbEnum& operator+=(T value){
        m_Value += value;
        return *this;
    }

    //! 引き算
    template <typename T>
    TbEnum& operator-=(T value){
        m_Value -= value;
        return *this;
    }

    //! 乗算
    template <typename T>
    TbEnum& operator*=(T value){
        m_Value *= value;
        return *this;
    }

    //! 割算
    template <typename T>
    TbEnum& operator/=(T value){
        m_Value /= value;
        return *this;
    }

    //! 前置インクリメント
    TbEnum& operator++(){
        ++m_Value;
        return *this;
    }

    //! 後置インクリメント 
    TbEnum operator++(int){
        m_Value++;
        return TbEnum(static_cast<EnumType>(m_Value-1));
    }

private:
    
    InnerType m_Value;

public: // friend関数

    template <typename T,typename EnumType2,typename InnerType2>
    friend TbBool operator==(const T& src , const TbEnum<EnumType2,InnerType2>& dst);

    template <typename T,typename EnumType2,typename InnerType2>
    friend TbBool operator!=(const T& src , const TbEnum<EnumType2,InnerType2>& dst);

};

}

#include "./enum/tb_enum_inl.h"

#endif
