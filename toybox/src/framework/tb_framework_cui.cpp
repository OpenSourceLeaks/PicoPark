/*!
 * システム
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */

#include "tb_fwd.h"
#include "framework/tb_framework_cui.h"
#include "framework/impl/tb_framework_cui_impl.h"
#include "base/time/tb_tick.h"
#include "base/system/tb_console.h"
#include "base/memory/tb_allocator_holder.h"
#include "base/memory/tb_heap_allocator.h"
#include "base/io/tb_file_manager.h"
#include "base/io/resource/tb_resource_manager.h"
#include "base/task/tb_task_manager.h"


namespace toybox
{

namespace
{

    TbAllocatorHolderGlobal s_GlobalHeapAllocatorHolder;
    void*                   s_ManagedHeapBuf = NULL;
    TbFrameworkCuiImpl*        s_Impl = NULL;

}

/*!
 * デフォルトの初期化パラメータ取得
 * @author Miyake_Shunsuke
 * @since 2015.01.09
 */
void TbFrameworkCui::GetDefaultInitParam( TbFrameworkCuiInitParam& initParam )
{
    TbUint32 bufSize = 128*1024*1024;
    initParam.allocParam[TbFrameworkInitParam::ALLOC_TYPE_GLOBAL].buf = nullptr;
    initParam.allocParam[TbFrameworkInitParam::ALLOC_TYPE_GLOBAL].bufSize = bufSize;
    initParam.taskLineMax = 0;
    // ルートパス設定
#if TB_PLATFORM_IS_NX
    initParam.rootPath = "rom:";
#elif TB_PLATFORM_IS_ANDROID
    initParam.rootPath = "/sdcard/resource";
#elif TB_PLATFORM_IS_FLASH || TB_PLATFORM_IS_EMSCRIPTEN
    initParam.rootPath = "/resource";
#else
    initParam.rootPath = "resource"; // binとresourceでフォルダ分け想定
#endif
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2011.11.23
 */
TbResult TbFrameworkCui::Initialize()
{
    if( s_Impl ) {
        return TB_E_FAIL;
    }
    TbFrameworkCuiInitParam initParam;
    GetDefaultInitParam(initParam);
    return Initialize(initParam);
}

/*!
 * 初期化(カスタム設定可能)
 * @retval TB_S_OK 正常完了
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkCui::Initialize( const TbFrameworkCuiInitParam& param )
{
    typedef TbFrameworkCuiInitParam InitParam;
    if( s_Impl ) {
        return TB_E_FAIL;
    }
    if( TB_FAILED( TbConsole::Create() ) ){
        return TB_E_FAIL;
    }

    void* heapBuf = param.allocParam[TbFrameworkCuiInitParam::ALLOC_TYPE_GLOBAL].buf;
    if( !heapBuf ){
        s_ManagedHeapBuf = ::malloc(param.allocParam[TbFrameworkCuiInitParam::ALLOC_TYPE_GLOBAL].bufSize);
        heapBuf = s_ManagedHeapBuf;
    }
    TB_RESQUE_RET(heapBuf,TB_E_FAIL);

    // アロケータ設定
    s_GlobalHeapAllocatorHolder.GetValue().GetHeap().Initialize( heapBuf ,
                                                                 param.allocParam[InitParam::ALLOC_TYPE_GLOBAL].bufSize ,
                                                                 64 );
    TbResourceManager::InitParam resourceInitParam;
    resourceInitParam.listTypeCount = 1;
    resourceInitParam.resourceMaxPerList = 10;
    s_Impl = TB_NEW TbFrameworkCuiImpl(param.taskLineMax,resourceInitParam);

    // ルートパス設定
    TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_ROOT , param.rootPath );
    TbResourceManager::GetInstance().SetFilePath( 0 , TB_FILE_PATH_ID_PRESET_ROOT );

    return TB_S_OK;
}

/*!
 * メインループ
 * @param begin フレーム開始処理
 * @param main メイン処理
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkCui::StartMainLoop( void (*begin)() , void (*main)() , void (*end)() )
{
    if( !s_Impl ) {
        return TB_E_FAIL;
    }
    s_Impl->StartMainLoop(begin,main,end);
    return TB_S_OK;
}

/*!
 * メインループ終了(この関数を呼ぶとメインループから抜けます)
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkCui::FinishMainLoop()
{
    if( !s_Impl ) {
        return TB_E_FAIL;
    }
    s_Impl->FinishMainLoop();
    return TB_S_OK;
}

/*!
 * 終了処理
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkCui::Finalize()
{
    if( s_Impl ) {
        TB_SAFE_DELETE( s_Impl );
        TbConsole::Delete();
    }
    return TB_S_OK;
}

}

#include "./depend/tb_framework_cui_impl_depend.h"
