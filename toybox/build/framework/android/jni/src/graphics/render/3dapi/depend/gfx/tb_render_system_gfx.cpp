/*!
 * レンダーシステムの共通処理
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_system.h"

namespace toybox
{
    
namespace
{

#if defined( NN_BUILD_TARGET_PLATFORM_OS_NN ) && defined( NN_BUILD_APISET_NX )

void* Allocate(size_t size, size_t alignment, void*)
{
    return aligned_alloc(alignment, size);
}

void Free(void* addr, void*)
{
    free(addr);
}

void* Reallocate(void* addr, size_t newSize, void*)
{
    return realloc(addr, newSize);
}

#endif

}
   
//////////////////////////
// TbRenderSystem
//////////////////////////

/*!
 * コンストラクタ
 */
TbRenderSystem::TbRenderSystem()
    : m_Depend()
{
}

/*!
 * デストラクタ
 */
TbRenderSystem::~TbRenderSystem()
{
}
    
/*!
 * 初期化
 */
void TbRenderSystem::Initialize()
{
#if defined( NN_BUILD_TARGET_PLATFORM_OS_NN ) && defined( NN_BUILD_APISET_NX )
    // グラフィックスシステムのためのメモリ周りの初期化を行います。
    {
        const size_t GraphicsSystemMemorySize = 8 * 1024 * 1024;
        nv::SetGraphicsAllocator(Allocate, Free, Reallocate, NULL);
        nv::InitializeGraphics(malloc(GraphicsSystemMemorySize), GraphicsSystemMemorySize);
    }
    // グラフィックス開発者向けツールおよびデバッグレイヤのためのメモリアロケータを設定します。
    nv::SetGraphicsDevtoolsAllocator(Allocate, Free, Reallocate, NULL);
    // GLSLC のためのメモリアロケータを設定します。
    glslcSetAllocator(Allocate, Free, Reallocate, NULL);
#endif

    nn::gfx::Initialize();
}

/*!
 * 後始末
 */
void TbRenderSystem::Finalize()
{
    nn::gfx::Finalize();
}

}
