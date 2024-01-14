/*!
 * 静的配列クラス
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */

#ifndef _INCLUDED_TB_STATIC_SET_H_
#define _INCLUDED_TB_STATIC_SET_H_

#include <base/container/tb_array_iterator.h>
#include <base/algorithm/tb_search.h>
#include <base/algorithm/tb_algorithm_misc.h>
#include <base/container/tb_static_array.h>

namespace toybox
{

/*!
 * 固定長セットクラス
 * @author Miyake Shunsuke
 * @since 2014.04.02
 */
template<typename T,TbUint32 CAPACITY>
class TbStaticSet
    : private TbStaticArray<T,CAPACITY>
{
public:
    
    typedef TbStaticArray<T,CAPACITY> Super;
    typedef typename Super::Iterator Iterator;
    typedef typename Super::ConstIterator ConstIterator;

public:

    // コンストラクタ
    TbStaticSet() {}

    // デストラクタ
    ~TbStaticSet() {}

public:

    // クリア
    void Clear() {
        Super::Clear();
    }

    // 要素追加
    TbBool Insert( const T& value ) {
        Iterator ite = TbLowerBound(Begin(),End(),value);
        if( ite == End() ){
            return Super::PushBack(value);
        }else if( *ite != value ){
            Super::Insert( ite , value );
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // 要素除去
    Iterator Erase( Iterator& ite ) {
        return Super::Erase(ite);; 
    }

    //! 最大数取得
    TbUint32 GetCapacity() const { 
        return CAPACITY;
    }

    //! サイズ取得
    TbUint32 GetCount() const { 
        return Super::GetCount();
    }
    
    //! 空かどうか
    TbBool IsEmpty() const { 
        return Super::IsEmpty();
    }

    //! 一杯かどうか
    TbBool IsFull() const { 
        return Super::IsFull();
    }

    // 検索
    Iterator Find( const T& value ) {
        Iterator ite = TbLowerBound(Begin(),End(),value);
        if( *ite == value ) {
            return ite;
        }
        return End();
    }

    //! 先頭イテレータ取得
    Iterator Begin(){
        return Super::Begin();
    }

    //! 末端イテレータ取得
    Iterator End(){
        return Super::End();
    }

    //! 先頭イテレータ取得
    ConstIterator Begin() const {
        return Super::Begin();
    }

    //! 末端イテレータ取得
    ConstIterator End() const {
        return Super::End();
    }
    
};

}

#endif
