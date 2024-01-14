/*!
 * 
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#include "tb_fwd.h"
#include "base/memory/tb_new_delete.h"
#include "base/memory/tb_memory.h"
#include "base/memory/tb_allocator.h"
#include "base/memory/tb_allocator_holder.h"

namespace toybox
{
    const TbNewDeleteType TB_NEW_DELETE_TYPE_VAL = {};
}

#if TB_PLATFORM_IS_APPLE || TB_PLATFORM_IS_NX

/*!
 * toybox用new
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */
void* operator new(size_t size,const toybox::TbNewDeleteType&)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Allocate(size);
}

/*!
 * toybox用delete
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */
void operator delete(void* pv,const toybox::TbNewDeleteType&)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    allocator.Deallocate(pv);
}

/*!
 * toybox用new[]
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */
void* operator new[](size_t size,const toybox::TbNewDeleteType&)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Allocate(size);
}

// toybox用delete[]
void operator delete[](void* pv,const toybox::TbNewDeleteType&)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Deallocate(pv);
}


#else

/*!
 * toybox用new
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */
void* operator new(size_t size/*,const toybox::TbNewDeleteType&*/)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Allocate(size);
}

/*!
 * toybox用delete
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */
void operator delete(void* pv/*,const toybox::TbNewDeleteType&*/) TB_THROW
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    allocator.Deallocate(pv);
}

/*!
 * toybox用new[]
 * @author Miyake_Shunsuke
 * @since 2012.05.26
 */
void* operator new[](size_t size/*,const toybox::TbNewDeleteType&*/)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Allocate(size);
}

// toybox用delete[]
void operator delete[](void* pv/*,const toybox::TbNewDeleteType&*/) TB_THROW
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Deallocate(pv);
}

#endif
