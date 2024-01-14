/*!
 * 静的配列クラス
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */

#ifndef _INCLUDED_TB_STATIC_ARRAY_H_
#define _INCLUDED_TB_STATIC_ARRAY_H_

#include <base/container/tb_array_iterator.h>

namespace toybox
{

/*!
 * 固定長配列クラス
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */
template<typename T,TbUint32 CAPACITY>
class TbStaticArray
{
public:

    typedef TbArrayIterator<T> Iterator;
    typedef TbArrayConstIterator<T> ConstIterator;

public:

    // コンストラクタ
    TbStaticArray();

    // デストラクタ
    ~TbStaticArray();

public:

    // アクセス演算子
    T& operator[](const TbUint32 index );

    // アクセス演算子
    const T& operator[](const TbUint32 index ) const;

public:

    // クリア
    void Clear();

    // 挿入
    TbBool Insert( const TbSint32 index , const T& value );

    // 挿入
    TbBool Insert( const ConstIterator& ite , const T& value );

    // 同一要素を複数個追加
    TbBool PushBack( const TbUint32 count , const T& value );

    // 複数個要素配列
    TbBool PushBackArray( const TbUint32 count , const T* valueArray );

    // 要素追加
    TbBool PushBack( const T& value );

    // デフォルトコンストラクタで追加
    TbBool PushBackDefault( TbUint32 count = 1 );

    // 最後尾除去
    TbBool PopBack();

    // 先頭取得
    T& GetFront() {
        return *getValue(0);
    }

    // 先頭取得
    const T& GetFront() const {
        return *getValue(0);
    }

    // 最後尾取得
    T& GetBack() {
        return *getValue(m_Count-1);
    }

    // 最後尾取得
    T& GetBack() const {
        return *getValue(m_Count-1);
    }

    // 要素取得
    T& GetAt(const TbUint32 index);

    // 要素取得
    const T& GetAt(const TbUint32 index) const;
    
    // 要素除去
    Iterator Erase(const TbUint32 index);
    
    // 要素除去
    Iterator Erase( Iterator& ite );

    //! 最大数取得
    TbUint32 GetCapacity() const { 
        return CAPACITY; 
    }

    //! サイズ取得
    TbUint32 GetCount() const { 
        return m_Count; 
    }
    
    //! 空かどうか
    TbBool IsEmpty() const { 
        return 0 == m_Count; 
    }

    //! 一杯かどうか
    TbBool IsFull() const { 
        return m_Count == CAPACITY; 
    }

    //! 先頭イテレータ取得
    Iterator Begin(){
        return Iterator(getValue(0));
    }

    //! 末端イテレータ取得
    Iterator End(){
        return Iterator(getValue(m_Count));
    }

    //! 先頭イテレータ取得
    ConstIterator Begin() const {
        return ConstIterator(getValue(0));
    }

    //! 末端イテレータ取得
    ConstIterator End() const {
        return ConstIterator(getValue(m_Count));
    }


    //! 先頭イテレータ取得
    ConstIterator BeginConst() const {
        return ConstIterator(getValue(0));
    }

    //! 末端イテレータ取得
    ConstIterator EndConst() const {
        return ConstIterator(getValue(m_Count));
    }

public:

    // 生の配列を返す
    const T* GetRawArray() const {
        return reinterpret_cast<const T*>(m_Array);
    }

private:

    static const TbUint32 BUFFER_SIZE = CAPACITY*sizeof(T);

private:

    // 値取得
    T* getValue( TbUint32 index ) {
        return reinterpret_cast<T*>(&m_Array[ index * sizeof(T) ]);
    }

    // 値取得
    const T* getValue( TbUint32 index ) const {
        return reinterpret_cast<const T*>(&m_Array[ index * sizeof(T) ]);
    }

private:

    TbUint8  m_Array[BUFFER_SIZE];  //!< 配列
    T*       m_ArrayPtr;
    TbUint32 m_Count;               //!< サイズ

};

}

#include "tb_static_array_inl.h" // 実装ファイル

#endif
