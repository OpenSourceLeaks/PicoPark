/*!
 * 特定のアロケーターからメモリを確保するためのMix-in
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_ALLOCATBALE_H_
#define _INCLUDED_TB_ALLOCATBALE_H_

#include <base/memory/tb_allocator.h>
#include <base/memory/tb_allocator_holder.h>

namespace toybox
{

template <typename IdType>
class TbAllocatable
{
public:
    //! new演算子
    void* operator new( size_t size ) {
        TbAllocator* allocator = TbAllocatorHolder<IdType>::GetInstance().GetAllocator();
        if( TB_VERIFY(allocator) ){
            return allocator->Allocate(size);
        }
        return NULL;
    }
    //! delete演算子
    void operator delete( void* ptr ) {
        TbAllocator* allocator = TbAllocatorHolder<IdType>::GetInstance().GetAllocator();
        if( TB_VERIFY(allocator) ){
            return allocator->Deallocate(ptr);
        }
    }
    //! new[]演算子
    void* operator new[] ( size_t size ) {
        TbAllocator* allocator = TbAllocatorHolder<IdType>::GetInstance().GetAllocator();
        if( TB_VERIFY(allocator) ){
            return allocator->Allocate(size);
        }
    }
    //! delete[]演算子
    void operator delete[] ( void* ptr ) {
        TbAllocator* allocator = TbAllocatorHolder<IdType>::GetInstance().GetAllocator();
        if( TB_VERIFY(allocator) ){
            return allocator->Deallocate(ptr);
        }
    }
};

}

#endif
