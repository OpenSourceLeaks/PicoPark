/*!
 * 静的両端キュークラス
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */

#ifndef _INCLUDED_TB_STATIC_DEQUE_H_
#define _INCLUDED_TB_STATIC_DEQUE_H_

namespace toybox
{

/*!
 * 固定長両端キュークラス
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */ 
template<typename T,TbUint32 CAPACITY>
class TbStaticDeque
{
public:

    // コンストラクタ
    TbStaticDeque();
    
    // デストラクタ
    ~TbStaticDeque();

public:

    // アクセス演算子
    T& operator[](const TbUint32 index );
    // アクセス演算子
    const T& operator[](const TbUint32 index ) const;

public:

    // クリア
    void Clear();

    // 要素追加
    TbBool PushFront(const T& value);

    // 要素追加
    TbBool PushBack(const T& value);

    // 戦闘除去
    TbBool PopFront();

    // 最後尾除去
    TbBool PopBack();

    // 先頭取得
    T& GetFront();

    // 最後尾取得
    T& GetBack();

    // 要素取得
    T& GetAt(const TbUint32 index);

    // 要素取得
    const T& GetAt(const TbUint32 index) const;

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

    //! 中身が一杯の時に自動ポップをするかどうか
    void SetEnablePopAuto( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_POP_AUTO ); 
    }

    //! 中身が一杯の時に自動ポップをするかどうか
    TbBool IsEnablePopAuto() const { 
        return m_BitArray.Test( FLAG_POP_AUTO ); 
    }

private:

    enum {
        FLAG_POP_AUTO , // バッファが一杯になるの自動でポップ( PushBack -> PopFront : PushFront -> PopBack )
    };
    static const TbUint32 BUFFER_SIZE = CAPACITY*sizeof(T);

private:

    //! 後ろに挿入するインデックス
    TbUint32 getPushBackIndex() const {
        return ( m_FrontIndex + m_Count ) % CAPACITY;
    }

    //! 前に挿入するインデックス
    TbUint32 getPushFrontIndex() const {
        if( 0 == m_FrontIndex ){
            return CAPACITY-1;
        }
        return m_FrontIndex-1;
    }

    //! indexを内部用indexに変換
    TbUint32 getIndex( TbUint32 index ) const {
        return ( m_FrontIndex + index ) % CAPACITY;
    }

    // 値取得
    T* getValue( TbUint32 index ) {
        return reinterpret_cast<T*>(&m_Array[ index * sizeof(T) ]);
    }

    // 値取得
    const T* getValue( TbUint32 index ) const {
        return reinterpret_cast<const T*>(&m_Array[ index * sizeof(T) ]);
    }

private:

    TbBitArray32    m_BitArray;              //!< ビット配列
    union {
        TbUint8  m_Array[BUFFER_SIZE];  //!< 配列
        T*       m_ArrayPtr;
    };
    TbUint32        m_FrontIndex;            //!< 先頭インデックス
    TbUint32        m_Count;                 //!< サイズ
};

}

#include "tb_static_deque_inl.h" // 実装ファイル

#endif
