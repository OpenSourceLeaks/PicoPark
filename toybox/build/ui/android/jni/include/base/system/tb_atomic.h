/*! 
 * @file
 * @brief atomic
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_ATOMIC_H_
#define _INCLUDED_TB_ATOMIC_H_

#include <base/system/tb_atomic_op.h>

namespace toybox
{

template <typename T>
class TbAtomic
{
public:

    // コンストラクタ
    TbAtomic();

    // コンストラクタ
    TbAtomic( const T& val ) {
        TbAtomicWrite32( &m_Value , val );
    }

public:

    //! 代入演算子
    TbAtomic<T>& operator=( const T& val ){
        TbAtomicWrite32(&m_Value,val);
        return *this;
    }

    //! 比較演算子
    TbAtomic<T>& operator==( const T& val ){
        T src = static_cast<T>(TbAtomicRead32(&m_Value));
        return src == val;
    }

    //! 比較演算子
    TbAtomic<T>& operator!=( const T& val ){
        T src = static_cast<T>(TbAtomicRead32(&m_Value));
        return src != val;
    }

    //! 加算演算子
    TbAtomic<T>& operator+( const T& val ){
        TbAtomicAdd32(&m_Value,val);
        return *this;
    }

    //! 前置インクリメント
    TbAtomic<T>& operator++(){
        TbAtomicInc32(&m_Value);
        return *this;
    }

    //! 前置デクリメント
    TbAtomic<T>& operator--(){
        TbAtomicDec32(&m_Value);
        return *this;
    }

    //! 取得 (メモリバリア指定できる予定)
    T Load() const {
        T result = static_cast<T>(TbAtomicRead32(&m_Value));
        return result;
    }

    //! 代入(メモリバリア指定できる予定)
    void Store( const T& val ) {
        TbAtomicWrite32(&m_Value ,static_cast<TbUint32>(val));
    }

    //! CAS (m_Value==cmpならm_Vlue=val) (メモリバリアを設定できるようにする予定)
    TbBool CompareAndSwap( const T& val , const T& cmp ) {
        return TbAtomicCompareAndSwap32(&m_Value,val,cmp);
    }

private:
    volatile TbUint32 m_Value; 
};

typedef TbAtomic<TbBool> TbAtomicBool;
typedef TbAtomic<TbUint32> TbAtomicU32;
typedef TbAtomic<TbSint32> TbAtomicS32;

}

#endif
