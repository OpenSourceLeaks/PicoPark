/*!
 * プールクラスのヘッダーファイル
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
#ifndef _INCLUDED_TB_POOL_H_
#define _INCLUDED_TB_POOL_H_

#include <base/math/tb_math_util.h>
#include <base/memory/tb_template_allocator_std.h>

namespace toybox
{

typedef TbUint64 TbPoolId;

#define TB_POOL_ID_INVALID 0xFFFFFFFFFFFFFFFFLL
#define TB_POOL_CREATE_ID(count,id) (( static_cast<TbUint64>(count) << 32UL ) | static_cast<TbUint64>(id) )
#define TB_POOL_GET_INDEX_FROM_ID(id) static_cast<TbUint32>( id & 0x00000000FFFFFFFFLL )
#define TB_POOL_GET_COUNT_FROM_ID(id) static_cast<TbUint32>(( id & 0xFFFFFFFF00000000LL ) >> 32UL )

/*!
 * プールクラス
 * 中身はただのポインタ格納配列だが、空いてる場所を自動で見つけて格納してくれる。リサイズも可能。
 */
template<typename T,typename Allocator = TbTemplateAllocatorStd<T> >
class TbPool
{
public:
    
    struct DataBlock
    {
        T value;
        TbUint32 nextIndex;
    };

public:

    class Iterator;
    class ConstIterator;
    typedef typename Allocator::template Rebind<DataBlock>::Other BlockAllocator;

public:

    //コンストラクタ
    TbPool( TbUint32 capacity , 
            const Allocator& allocator = Allocator() ,
            const BlockAllocator& blockAllocator = BlockAllocator() );

    //デストラクタ
    ~TbPool();

public:

    //追加
    TbPoolId    Add( const T& value);

    //除去
    TbBool      Remove(TbPoolId id);

    //取得
    T*          GetAt(TbPoolId id);

    //取得
    const T*    GetAt(TbPoolId id) const;

    //クリアー
    void        Clear();

    // リサイズ
    void        SetCapacity(TbUint32 capacity);

    //最大登録数取得
    TbUint32    GetCapacity() const;

    //! 一杯になった時に最大登録数を設定するか
    void SetEnableSetCapacityAuto( TbBool isEnableSetCapacityAuto ){
        m_IsEnableSetCapacityAuto = isEnableSetCapacityAuto;
    }
    
    //! 一杯になった時に最大登録数を設定するか
    TbBool IsEnableSetCapacityAuto() const{
        return m_IsEnableSetCapacityAuto;
    }

    //登録数取得
    TbUint32    GetCount() const;

    //! 一杯かどうか
    TB_INLINE TbBool    IsFull() const { 
        return m_Capacity == m_Count; 
    }

    //! 空かどうか
    TB_INLINE TbBool    IsEmpty() const { 
        return 0 == m_Count; 
    }

    //! イテレータの前半取得
    Iterator Begin()
    {
        if( m_BeginUsedIndex != INDEX_INVALID ){
            Iterator ite( &m_Pool[ m_BeginUsedIndex ] , this );
            return ite;
        }
        return End();
    }

    //! イテレータの末端取得
    Iterator End() {
        return Iterator(NULL,this); 
    }

private:

    static const TbUint32 INDEX_USED    = 0xFFFFFFFE;
    static const TbUint32 INDEX_INVALID = 0xFFFFFFFF;

private:

    DataBlock*      m_Pool;                     // 格納先
    TbBool          m_IsEnableSetCapacityAuto;  // 自動キャパシティ調整するか
    DataBlock*      m_NextFreeBlock;            // 次
    TbUint32        m_BeginUsedIndex;           // 先頭インデックス
    TbUint32        m_EndUsedIndex;             // 先頭インデックス
    TbUint32        m_Capacity;                 // 最大登録可能数
    TbUint32        m_Count;                    // 登録数
    TbUint32        m_IssueCount;               // 発行ID
    Allocator       m_ValueAllocator;           // アロケータ
    BlockAllocator  m_BlockAllocator;           // アロケータ(ブロック用)

private:

    friend class Iterator;
    friend class ConstIterator;

};

}

#include "tb_pool_inl.h"

#endif