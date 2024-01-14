/*!
 * @file
 * @brief メモリ関連関数郡の実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/memory/tb_memory.h"
#include "base/memory/tb_allocator_holder.h"

namespace toybox
{

/*!
 * メモリアロケーション
 * 16バイトアライメントされます
 * @author Miyake Shunsuke
 */
void* TbMallocKernel(TbUint32 size)
{
#if TB_PLATOFORM_IS_WIN
    return _aligned_offset_malloc( size, 16, 0);
#elif TB_PLATOFORM_IS_WIIU
    return MEMAllocFromDefaultHeapEx( size , 16 );
#else
    return malloc( size );
#endif
}

/*!
 * メモリの解放
 * param ptr ブロックアドレス
 * @author Miyake Shunsuke
 */
void TbFreeKernel(void* ptr)
{
    if(ptr)
    {
#if TB_PLATOFORM_IS_WIN
        _aligned_free(ptr);
#elif TB_PLATOFORM_IS_WIIU
        return MEMFreeToDefaultHeap(ptr);
#else
        free(ptr);
#endif
    }
}

/*!
 *メモリアロケーション
 */
void* TbMallocDefaultHeap(TbUint32 size, TbUint32 align)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Allocate(size,align);
}

/*!
* メモリの解放
*/
void TbFreeDefaultHeap(void* ptr)
{
    using namespace toybox;
    TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
    return allocator.Deallocate(ptr);
}

}
