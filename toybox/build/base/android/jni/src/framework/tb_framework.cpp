/*!
 * システム
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */

#include "tb_fwd.h"
#include "framework/tb_framework.h"
#include "framework/impl/tb_framework_impl.h"

#include "base/time/tb_tick.h"
#include "base/memory/tb_allocator_holder.h"
#include "base/memory/tb_heap_allocator.h"
#include "base/task/tb_task_manager.h"
#include "base/math/tb_random.h"
#include "base/time/tb_time.h"
#include "base/performance/tb_performance_manager.h"
#include "graphics/font/tb_font.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/tb_draw_2d.h"
#include "graphics/camera/tb_camera_manager.h"
#include "graphics/light/tb_light_manager.h"
#include "base/performance/tb_profiler.h"

namespace toybox
{

namespace
{
    TbAllocatorHolderGlobal s_GlobalHeapAllocatorHolder;
    void*                   s_ManagedHeapBuf = nullptr;
    void*                   s_CommandBufferHeapBuf = nullptr;
    TbFrameworkImpl*        s_Impl = nullptr;
}

/*!
 * デフォルトの初期化パラメータ取得
 * @author Miyake Shunsuke
 * @since 2013.03.12
 */
void TbFramework::GetDefaultInitParam( TbFrameworkInitParam& param )
{
    TbUint32 bufSize = 512 * 1024 * 1024;
    TbUint32 commandBufferSize = 5*1024*1024;
    param.allocParam[TbFrameworkInitParam::ALLOC_TYPE_GLOBAL].buf = nullptr; // デフォルト10MB
    param.allocParam[TbFrameworkInitParam::ALLOC_TYPE_GLOBAL].bufSize = bufSize;
    param.allocParam[TbFrameworkInitParam::ALLOC_TYPE_COMMAND_BUFFER].buf = nullptr; // デフォルト10MB
    param.allocParam[TbFrameworkInitParam::ALLOC_TYPE_COMMAND_BUFFER].bufSize = commandBufferSize;
    param.renderViewMax = 1;
    param.renderPassMax = 1;
    param.taskLineMax = 0;
    // ルートパス設定
#if TB_PLATFORM_IS_NX
    param.rootPath = "rom:";
    param.savePath = "save:";
#elif TB_PLATFORM_IS_FLASH || TB_PLATFORM_IS_EMSCRIPTEN
    param.rootPath = "/resource";
    param.savePath = "/save";
#else
    param.rootPath = "./resource"; // binとresourceでフォルダ分け想定
    param.savePath = "./save"; // binとresourceでフォルダ分け想定
#endif
}

/*!
 * 初期化(デフォルト設定)
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFramework::Initialize()
{
    if( s_Impl ) {
        return TB_E_FAIL;
    }
    TbFrameworkInitParam initParam;
    GetDefaultInitParam(initParam);
    return Initialize(initParam);
}

#include <locale.h> // ロケール設定の位置づけを調査する必要あり。まだラッピングしない

/*!
 * 初期化(カスタム設定可能)
 * @retval TB_S_OK 正常完了
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFramework::Initialize( const TbFrameworkInitParam& param )
{
#if 1
    setlocale( LC_CTYPE, "Japanese_Japan.932" );
#endif

    typedef TbFrameworkInitParam InitParam;
    if( !s_Impl )
    {
        void* globalHeap = param.allocParam[InitParam::ALLOC_TYPE_GLOBAL].buf;
        void* commandBufferHeap = param.allocParam[InitParam::ALLOC_TYPE_COMMAND_BUFFER].buf;
        if( !globalHeap ){
            s_ManagedHeapBuf = ::malloc(param.allocParam[InitParam::ALLOC_TYPE_GLOBAL].bufSize);
            globalHeap = s_ManagedHeapBuf;
        }
        if( !commandBufferHeap ){
            s_CommandBufferHeapBuf = ::malloc(param.allocParam[InitParam::ALLOC_TYPE_COMMAND_BUFFER].bufSize);
            commandBufferHeap = s_CommandBufferHeapBuf;
        }

        // アロケータ設定
        s_GlobalHeapAllocatorHolder.GetValue().GetHeap().Initialize( globalHeap ,
                                                         param.allocParam[InitParam::ALLOC_TYPE_GLOBAL].bufSize ,
                                                         64 );
        TbResourceManager::InitParam resourceInitParam;
        resourceInitParam.listTypeCount = 1;
        resourceInitParam.resourceMaxPerList = 40;
        s_Impl = TB_NEW TbFrameworkImpl( param.taskLineMax ,
                                      param.renderViewMax ,
                                      param.renderPassMax , 
                                      resourceInitParam );
        TbRenderPassManager& renderPass = TbRenderPassManager::GetInstance();
        renderPass.GetCommandAllocator()->Initialize(commandBufferHeap,
                                                     param.allocParam[InitParam::ALLOC_TYPE_COMMAND_BUFFER].bufSize);

        // ルートパス設定
        TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_ROOT , param.rootPath );
        if( param.savePath ){
            TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_SAVE , param.savePath );
        }
        TbResourceManager::GetInstance().SetFilePath( 0 , TB_FILE_PATH_ID_PRESET_ROOT );

        {
            TbUint32 seed = 0;
#if 1 || !TB_IS_DEBUG
            TbDate date = TbTime::GetDate();
            seed = date.year + 
                   date.month + 
                   date.day +
                   date.hour + 
                   date.minute + 
                   date.second;
#endif
            TbInitRandom(seed);
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}
    
/*!
 * 初期化済みか
 * @author Miyake Shunsuke
 * @since 2012.07.14
 */
TbBool TbFramework::IsInitialized()
{
    return s_Impl != nullptr;
}

/*!
 * メインループ開始
 * @param begin フレーム開始処理
 * @param main メイン処理
 * @param draw 描画処理
 * @param end  終了処理
 * @author teco
 */
TbResult TbFramework::StartMainLoop( void (*begin)() , 
                                     void (*main)() , 
                                     void (*draw)() , 
                                     void (*end)(), 
                                     void (*drawDirect)())
{
    if( s_Impl ) {
        return s_Impl->StartMainLoop( begin , main , draw , end , drawDirect);
    }
    TB_ASSERT(!"not Initlalized");
    return TB_E_FAIL;
}

/*!
 * メインループ終了(この関数を呼ぶとメインループから抜けます)
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFramework::FinishMainLoop()
{
    if( s_Impl ) {
        s_Impl->FinishMainLoop();
        return TB_S_OK;
    }
    TB_ASSERT(!"not Initlalized");
    return TB_E_FAIL;
}

/*!
 * 終了処理
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFramework::Finalize()
{
    TB_SAFE_DELETE( s_Impl );
    s_GlobalHeapAllocatorHolder.GetValue().GetHeap().Finalize();
    if( s_ManagedHeapBuf ){
        ::free(s_ManagedHeapBuf);
        s_ManagedHeapBuf = nullptr;
    }
    if( s_CommandBufferHeapBuf ){
        ::free(s_CommandBufferHeapBuf);
        s_CommandBufferHeapBuf = nullptr;
    }
    return TB_S_OK;
}

}

// 機種依存の実装コードを同じcppに埋め込む
#include "./depend/tb_framework_impl_depend.h"
