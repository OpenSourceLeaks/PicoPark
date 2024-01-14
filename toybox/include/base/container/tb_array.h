/*!
 * @file
 * @brief 可変長配列クラス
 * @author 三宅俊輔
 */

#ifndef _INCLUDE_TB_ARRAY_H_
#define _INCLUDE_TB_ARRAY_H_

#include <base/memory/tb_template_allocator_std.h>
#include <base/algorithm/tb_algorithm_misc.h>
#include <base/math/tb_math_util.h>
#include <base/container/tb_array_iterator.h>

namespace toybox
{

/**
 * @class TbArray
 * 可変長(内部固定長)配列クラス
 */
template<typename T,typename Allocator = TbTemplateAllocatorStd<T> >
class TbArray
{
public:

    typedef TbArrayIterator<T> Iterator;
    typedef TbArrayConstIterator<T> ConstIterator;

public:

    //! コンストラクタ
    explicit TbArray( TbSint32 capacity = 0 , const Allocator& allocator = Allocator() );
    
    //! デストラクタ
    ~TbArray();
    
public: // 演算子
    
    //! アクセス演算子
    T& operator[]( TbSint32 index ) {
        return GetAt(index);
    }
    
    //! アクセス演算子
    const T& operator[]( TbSint32 index ) const {
        return GetAt(index);
    }

public:

    //! リサイズ
    void SetCapacity( const TbSint32 capacity );

    //! 最大要素数取得
    TbSint32 GetCapacity() const { 
        return m_Capacity; 
    }
    
    //! サイズ取得
    TbSint32 GetCount() const { 
        return m_Count; 
    }
    
    //! 自動キャパシティリサイズ可能設定
    void SetEnableSetCapacityAuto( TbBool isEnableSetCapacityAuto ){
        m_IsEnableSetCapacityAuto = isEnableSetCapacityAuto;
    }
    
    //! 自動キャパシティリサイズ可能設定
    TbBool IsEnableSetCapacityAuto() const{
        return m_IsEnableSetCapacityAuto;
    }

    // 挿入
    TbBool Insert( const TbSint32 index , const T& element );

    // 挿入
    TbBool Insert( Iterator& ite , const T& element );

    // 挿入
    template<typename ComapareFunc>
    TbBool InsertLower( const T& element );

    // 要素の追加
    TbBool PushBack( TbUint32 count , const T& element );

    // 要素の追加
    TbBool PushBack( const T& element );
    
    // デフォルトコンストラクタで追加
    TbBool PushBackDefault( TbUint32 count = 1 );

    // 要素の除去
    TbBool PopBack();
    
    // 要素の除去
    Iterator Erase( const TbSint32 index );

    // 要素の除去
    Iterator Erase( const Iterator& ite );

    //! 先頭要素取得
    T&   GetFront() {
        return GetAt( 0 );
    }

    //! 先頭要素取得
    const T&   GetFront() const {
        return GetAt( 0 );
    }

    //! 最後尾要素取得
    T&   GetBack() {
        return GetAt( m_Count-1 );
    }

    //! 最後尾要素取得
    const T&   GetBack() const {
        return GetAt( m_Count-1 );
    }

    // 要素取得
    T&   GetAt(const TbSint32 index);

    // 要素取得
    const T&   GetAt(const TbSint32 index) const;
    
    // 要素クリアー
    void Clear();

    //! 配列が空かどうか
    TbBool IsEmpty() const {
        return (0==m_Count);
    }

    //! 配列が空かどうか
    TbBool IsFull() const {
        return (m_Capacity==m_Count);
    }
    
    // 先頭イテレータ取得
    Iterator Begin();

    // 先頭イテレータ取得
    ConstIterator Begin() const;

    // 先頭イテレータ取得
    ConstIterator BeginConst() const;

    // 末端イテレータ取得
    Iterator End();
    
    // 末端イテレータ取得
    ConstIterator End() const;

    // 末端イテレータ取得
    ConstIterator EndConst() const;

private:

    TbSint32        m_Capacity;                //!< 最大格納可能数
    TbSint32        m_Count;                   //!< サイズ
    TbBool          m_IsEnableSetCapacityAuto; //!< 自動キャパシティ変更
    T*              m_Array;                   //!< 実配列
    Allocator       m_Allocator;               //!< アロケータ

};

}

#include "./tb_array_inl.h"

#endif
