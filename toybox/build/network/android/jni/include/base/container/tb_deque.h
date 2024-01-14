/*!
 * 両端キュークラス
 * @author Miyake Shunsuke
 * @since 2010.06.24
 */

#ifndef _INCLUDED_TB_DEQUE_H_
#define _INCLUDED_TB_DEQUE_H_

#include <base/memory/tb_template_allocator_std.h>

namespace toybox
{

/*!
 * 端キュークラス
 * @author Miyake Shunsuke
 * @since 2012.04.29
 */ 
template<typename T,typename Allocator = TbTemplateAllocatorStd<T> >
class TbDeque
{
public:

    // コンストラクタ
    TbDeque( const Allocator& allocator = Allocator() );

    // コンストラクタ
    TbDeque( TbUint32 capacity , const Allocator& allocator = Allocator() );

    // デストラクタ
    ~TbDeque();

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

    // 最大数設定
    void SetCapacity( TbUint32 capacity );

    //! 最大数取得
    TbUint32 GetCapacity() const { 
        return m_Capacity; 
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
        return m_Count == m_Capacity; 
    }

    //! 自動ポップをするかどうか
    void SetEnablePopAuto( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_POP_AUTO ); 
    }

    //! 自動ポップをするかどうか
    TbBool IsEnablePopAuto() const { 
        return m_BitArray.Test( FLAG_POP_AUTO ); 
    }

private:

    enum {
        FLAG_POP_AUTO , // バッファが一杯になると自動でポップ( PushBack -> PopFront : PushFront -> PopBack )
    };

private:

    //! 後ろに挿入するインデックス
    TbUint32 getPushBackIndex() const {
        return ( m_FrontIndex + m_Count ) % m_Capacity;
    }

    //! 前に挿入するインデックス
    TbUint32 getPushFrontIndex() const {
        if( 0 == m_FrontIndex ){
            return m_Capacity-1;
        }
        return m_FrontIndex-1;
    }

    //! indexを内部用indexに変換
    TbUint32 getIndex( TbUint32 index ) const {
        return ( m_FrontIndex + index ) % m_Capacity;
    }

private:

    TbBitArray32    m_BitArray;         //!< ビット配列
    T*              m_Array;            //!< 生サイズ
    Allocator       m_Allocator;        //!< アロケータ
    TbUint32        m_FrontIndex;       //!< 先頭インデックス
    TbUint32        m_Count;            //!< サイズ
    TbUint32        m_Capacity;         //!< キャパシティ

};

}

#include "tb_deque_inl.h" // 実装ファイル

#endif
