/*!
 * アロケータをシングルトンで保持するためのクラス
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ALLOCATOR_HOLDER_H_
#define _INCLUDED_ALLOCATOR_HOLDER_H_

#include <base/memory/tb_heap_allocator.h>
#include <base/util/tb_holder_singleton.h>

namespace toybox
{

class TbAllocator;

struct TbAllocatorHolderIdGlobal;
typedef TbHolderSingleton<TbHeapAllocator,TbAllocatorHolderIdGlobal> TbAllocatorHolderGlobal;

};

#endif
