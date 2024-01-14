/*!
 * フレームワーク実装部分の共通
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_FRAMEWORK_IMPL_H_
#define _INCLUDED_TB_FRAMEWORK_IMPL_H_

#include <framework/impl/tb_framework_impl_common.h>
#include "base/io/tb_file_manager.h"
#include "base/io/resource/tb_resource_manager.h"
#include "graphics/window/tb_window_manager.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_system.h"
#include "graphics/render/tb_render_pass_manager.h"
#include "graphics/display/tb_display_manager.h"
#include "base/time/tb_tick.h"
#include "base/performance/tb_performance_manager.h"

namespace toybox
{

class TbFrameworkImpl : private TbFrameworkImplCommon , public TbSingletonRef<TbFrameworkImpl>
{
public:
    typedef TbFrameworkImplCommon Super;
public:
    // コンストラクタ
    TbFrameworkImpl( TbUint32 taskLineMax , 
                        TbUint32 renderViewMax , 
                        TbUint32 renderPassMax , 
                        const TbResourceManager::InitParam& resourceParam )
        : Super( taskLineMax )
        , m_DisplayManager()
        , m_WindowManager()
        , m_RenderDevice(renderViewMax)
        , m_RenderPassManager()
        , m_ResourceManager(resourceParam)
        , m_MainLoopDrawFunc(nullptr)
        , m_MainLoopDrawDirectFunc(nullptr)
    {
        m_RenderPassManager.Initialize(renderPassMax);
        m_RenderSystem.Initialize();
    }
    // デストラクタ
    ~TbFrameworkImpl()
    {
        TbPrintf("destructor TbFrameworkImpl");
    }
public:

    // メインループスタート
    TbResult StartMainLoop( MainLoopFunc begin , MainLoopFunc main , MainLoopFunc draw , MainLoopFunc end , MainLoopFunc drawDirect)
    {
        if( TB_VERIFY_MSG(m_RenderDevice.IsInitialized(),"RenderDevice is not Initialized") ){
            m_MainLoopDrawFunc = draw;
            m_MainLoopDrawDirectFunc = drawDirect;
            Super::StartMainLoop(begin,main,end);
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }

    // メインループ終了
    void FinishMainLoop()
    {
        Super::FinishMainLoop();
    }

    // メインループは有効か
    TbBool IsEnableMainLoop() const {
        return Super::IsEnableMainLoop();
    }

    // 更新
    void Update();

    // 描画処理
    void Draw();

    // 後処理
    void Post();

private:
#if TB_IS_DEBUG
    TbStackTrace            m_StackTrace;
    TbPerforcemanceManger   m_PerforcemanceManger;
#endif
    TbResourceManager       m_ResourceManager;
    TbDisplayManager        m_DisplayManager;
    TbWindowManager         m_WindowManager;
    TbRenderSystem          m_RenderSystem;
    TbRenderDevice          m_RenderDevice;
    TbRenderPassManager     m_RenderPassManager;
    MainLoopFunc            m_MainLoopDrawFunc;     // メインループ描画関数
    MainLoopFunc            m_MainLoopDrawDirectFunc;
};

}

#endif
