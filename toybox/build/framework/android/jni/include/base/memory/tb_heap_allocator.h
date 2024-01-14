/*!
 * ヒープからメモリ確保するアロケータ(TbHeapとTbAllocatorをつなぐアダプタークラス)
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */

#ifndef _INCLUDED_TB_HEAP_ALLOCATOR_H_
#define _INCLUDED_TB_HEAP_ALLOCATOR_H_

#include <base/memory/tb_allocator.h>
#include <base/memory/tb_heap.h>

namespace toybox
{

class TbHeapAllocator : public TbAllocator
{
public:

    // コンストラクタ
    TbHeapAllocator() : m_Heap() {}

    // デストラクタ
    ~TbHeapAllocator() {}

public:

    // ヒープ取得
    TbHeap& GetHeap() {
        return m_Heap;
    }

    // ヒープ取得
    const TbHeap& GetHeap() const {
        return m_Heap;
    }

public:

    //メモリ確保
    virtual void* Allocate( TbUint32 size ) {
        return m_Heap.AllocateForward(size);
    }

    //メモリ確保
    virtual void* Allocate(TbUint32 size,TbUint32 align) {
        return m_Heap.AllocateForward(size,align);
    }

    //開放
    virtual void Deallocate(void* ptr) {
        m_Heap.Deallocate(ptr);
    }

private:
    TbHeap m_Heap;
};

}

#endif
