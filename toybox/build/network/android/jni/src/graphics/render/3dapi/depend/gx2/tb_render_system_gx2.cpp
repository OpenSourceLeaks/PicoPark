/*!
 * レンダーシステムの共通処理
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_system.h"

namespace toybox
{
    
namespace
{

// 64byteアライメント
static const TbUint32 COMMAND_BUFFER_POOL_SIZE = 1024*1024*sizeof(TbUint32);

static void* allocGraphics(GX2RResourceFlags resourceFlags, u32 byteCount, u32 alignment)
{
    if (GX2TestBitFlagsAny(resourceFlags, GX2R_BIND_COLOR_BUFFER | GX2R_BIND_DEPTH_BUFFER | GX2R_BIND_SCAN_BUFFER | GX2R_BIND_GS_RING | GX2R_USAGE_FORCE_MEM1) &&
        !GX2TestBitFlagsAny(resourceFlags, GX2R_USAGE_FORCE_MEM2))
    {
        return TbGX2AllocMem1(byteCount,alignment);
    }
    return TbGX2Malloc(byteCount);
}

static void freeGraphics(GX2RResourceFlags resourceFlags, void* pMem)
{
    if (GX2TestBitFlagsAny(resourceFlags, GX2R_BIND_COLOR_BUFFER | GX2R_BIND_DEPTH_BUFFER | GX2R_BIND_SCAN_BUFFER | GX2R_BIND_GS_RING | GX2R_USAGE_FORCE_MEM1) &&
        !GX2TestBitFlagsAny(resourceFlags, GX2R_USAGE_FORCE_MEM2))
    {
        TbGX2FreeMem1(pMem);
        return;
    }
    TbGX2Free(pMem);
}

}
    
//////////////////////////
// グローバル関数
//////////////////////////

void* TbGX2Alloc( u32 size , u32 align )
{
    align = TbMax(align,4);
    void* ptr = TbAlloc(size,align);
    GX2NotifyMemAlloc(ptr, size, align);
    return ptr;
}
    
void  TbGX2Free( void* ptr )
{
    TbFree(ptr);
    GX2NotifyMemFree(ptr);
}

void* TbGX2AllocMem1( u32 size , u32 align )
{
    align = TbMax(align,4);
    void* ptr = TbRenderSystem::GetInstance().GetDepend().AllocMem1(size,align);
    return ptr;
}

void  TbGX2FreeMem1( void* ptr )
{
    TbRenderSystem::GetInstance().GetDepend().FreeMem1( ptr );
}

void* TbGX2AllocBucket( u32 size , u32 align )
{
    align = TbMax(align,4);
    void* ptr = TbRenderSystem::GetInstance().GetDepend().AllocBucket(size,align);
    return ptr;
}

void  TbGX2FreeBucket( void* ptr )
{
    TbRenderSystem::GetInstance().GetDepend().FreeBucket( ptr );
}

//////////////////////////
// TbRenderSystemDepend
//////////////////////////

// コンストラクタ
TbRenderSystemDepend::TbRenderSystemDepend()
    : m_IsOnForeground(TB_TRUE)
    , m_HeapHandleMem1(MEM_HEAP_INVALID_HANDLE)
    , m_HeapHandleBucket(MEM_HEAP_INVALID_HANDLE)
{
}

// デストラクタ
TbRenderSystemDepend::~TbRenderSystemDepend()
{
}

// 初期化
void TbRenderSystemDepend::Initialize()
{
    {
        MEMHeapHandle handleMEM1 = MEMGetBaseHeapHandle(MEM_ARENA_1);
        u32 size = MEMGetAllocatableSizeForFrmHeapEx(handleMEM1,4);
        void* ptr = MEMAllocFromFrmHeapEx(handleMEM1, size, 4);
        m_HeapHandleMem1 = MEMCreateExpHeap(ptr, size);
    }
    {
        MEMHeapHandle handleFG = MEMGetBaseHeapHandle(MEM_ARENA_FG);
        u32 size = MEMGetAllocatableSizeForFrmHeapEx(handleFG,4);
        void* ptr = MEMAllocFromFrmHeapEx(handleFG, size, 4);
        m_HeapHandleBucket = MEMCreateExpHeap(ptr, size);
    }
    GX2RSetAllocator(allocGraphics,freeGraphics);

    // ScanBuffer設定
    u32 scanSize = 0;
    GX2Boolean scaleNeeded = false;    
    GX2CalcTVSize(TB_GX2_TV_RENDER_MODE, GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,
                  GX2_BUFFERING_DOUBLE, &scanSize, &scaleNeeded);
    m_ScanBuffer = TbGX2AllocBucket(scanSize, GX2_SCAN_BUFFER_ALIGNMENT);
    GX2Invalidate(GX2_INVALIDATE_CPU, m_ScanBuffer, scanSize);
    GX2SetTVBuffer(m_ScanBuffer, scanSize, TB_GX2_TV_RENDER_MODE,
                   GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM, GX2_BUFFERING_DOUBLE);
}

// 後始末
void TbRenderSystemDepend::Finalize()
{
    if( m_IsOnForeground ){
        GX2DrawDone();
    }

    TbGX2FreeBucket(m_ScanBuffer);
    
    MEMDestroyExpHeap(m_HeapHandleMem1);
    MEMHeapHandle handleMEM1 = MEMGetBaseHeapHandle(MEM_ARENA_1);
    MEMFreeToFrmHeap(handleMEM1, MEM_FRMHEAP_FREE_ALL);

    MEMDestroyExpHeap(m_HeapHandleBucket);
    MEMHeapHandle handleFG = MEMGetBaseHeapHandle(MEM_ARENA_FG);
    MEMFreeToFrmHeap(handleFG, MEM_FRMHEAP_FREE_ALL);
}

// MEM1からのアロケート
void* TbRenderSystemDepend::AllocMem1( u32 size , u32 align )
{
    void* ptr = MEMAllocFromExpHeapEx(m_HeapHandleMem1, size , align);
    GX2NotifyMemAlloc(ptr, size, align);
}

// MEM1のメモリ開放
void TbRenderSystemDepend::FreeMem1( void* ptr )
{
    MEMFreeToExpHeap(m_HeapHandleMem1,ptr);
    GX2NotifyMemFree(ptr);
}

// Bucketからのアロケート
void* TbRenderSystemDepend::AllocBucket( u32 size , u32 align )
{
    void* ptr = MEMAllocFromExpHeapEx(m_HeapHandleBucket, size , align);
    GX2NotifyMemAlloc(ptr, size, align);
}

// Bucket解放
void TbRenderSystemDepend::FreeBucket( void* ptr )
{
    MEMFreeToExpHeap(m_HeapHandleBucket,ptr);
    GX2NotifyMemFree(ptr);
}
    
// フォアグラウンド獲得
u32 TbRenderSystemDepend::acquiredForegroundCallback(void* ptr)
{
    TbRenderSystemDepend* depend = reinterpret_cast<TbRenderSystemDepend*>(ptr);
    depend->m_IsOnForeground = TB_TRUE;
    return 0;
}

// フォアグラウンド解放
u32 TbRenderSystemDepend::releaseForegroundCallback(void* ptr)
{
    TbRenderSystemDepend* depend = reinterpret_cast<TbRenderSystemDepend*>(ptr);
    depend->m_IsOnForeground = TB_FALSE;
    return 0;
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
    TB_SAFE_DELETE_ARRAY(m_CommandBuffer);
}
    
/*!
 * 初期化
 */
void TbRenderSystem::Initialize()
{
    m_CommandBuffer = new TbUint8[COMMAND_BUFFER_POOL_SIZE];

    u32 gx2InitAttribs[] =
    {
        GX2_INIT_ATTRIB_CB_BASE, (u32)m_CommandBuffer,          // cb buffer
        GX2_INIT_ATTRIB_CB_SIZE, (u32)COMMAND_BUFFER_POOL_SIZE, // cb size
        GX2_INIT_ATTRIB_NULL                 // terminates the list
    };

    GX2Init(gx2InitAttribs);
    
    m_Depend.Initialize();
        
    GX2SetupContextState(&m_Depend.contextState);
    GX2SetContextState(&m_Depend.contextState);
    GX2SetSwapInterval(1);

    ProcUIRegisterCallback(PROCUI_MESSAGE_ACQUIRE, TbRenderSystemDepend::acquiredForegroundCallback, &m_Depend, 100);
    ProcUIRegisterCallback(PROCUI_MESSAGE_RELEASE, TbRenderSystemDepend::releaseForegroundCallback, &m_Depend, 100);
}

/*!
 * 後始末
 */
void TbRenderSystem::Finalize()
{
    if (m_Depend.m_IsOnForeground && OS_SHUTDOWN_RESTART != OSGetShutdownReason()) {
        GX2SetTVEnable(GX2_FALSE);
    }
    
    if (m_Depend.m_IsOnForeground)
    {
        m_Depend.Finalize();
    }
    
    GX2Shutdown();
    GX2RSetAllocator(nullptr, nullptr);
}

}
