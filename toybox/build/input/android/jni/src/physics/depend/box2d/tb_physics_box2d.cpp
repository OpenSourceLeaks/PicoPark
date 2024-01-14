/*!
 * Box2D固有設定
 * @author Miyake Shunsuke
 * @since 2015.03.06
 */
 
#include <tb_fwd.h>
#include "base/memory/tb_allocator_holder.h"
#include <physics/tb_physics_types.h>

#if TB_PHYSICSAPI_IS_BOX2D

// b2Setting.cppをここで実装

// Memory allocators. Modify these to use your own allocator.
void* b2Alloc(int32 size)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Allocate(size);
}

void b2Free(void* mem)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    allocator.Deallocate(mem);
}

// You can modify this to use your logging facility.
void b2Log(const char* string, ...)
{
    va_list args;
    va_start(args, string);
    vprintf(string, args);
    va_end(args);
}

#endif