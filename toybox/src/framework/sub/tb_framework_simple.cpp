/*!
 * システム
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */

#include "tb_fwd.h"
#include "base/task/tb_task.h"
#include "base/io/tb_file_manager.h"
#include "base/io/resource/tb_resource_manager.h"
#include "base/io/tb_log.h"
#include <base/time/tb_tick.h>
#include <base/time/tb_time.h>
#include "framework/sub/tb_framework_simple.h"
#include "framework/tb_framework.h"
#include "framework/debug/menu/tb_debug_menu_all.h"
#include "framework/debug/viewer/tb_allocator_viewer.h"
#include "framework/debug/viewer/tb_heap_allocator_view.h"
#include "base/memory/tb_allocator_holder.h"
#include "graphics/render/3dapi/tb_3dapi.h"
#include "graphics/window/tb_window_manager.h"
#include "graphics/window/tb_window.h"
#include "graphics/render/tb_render_pass_manager.h"
#include "graphics/math/tb_gr_matrix_util.h"
#include "graphics/display/tb_display_manager.h"
#include "graphics/font/tb_font.h"
#include "graphics/resource/tb_resource_texture.h"
#include "graphics/render/tb_draw_2d.h"
#include "graphics/render/tb_draw_3d.h"
#include "graphics/render/tb_fade.h"
#include "graphics/util/tb_screen_shot.h"
#include "input/tb_keyboard.h"
#include "input/tb_pad.h"
#include "input/tb_mouse.h"
#include "input/tb_touch.h"
#include "base/performance/tb_profiler.h"

namespace toybox
{

class TbFrameworkSimpleImpl
{
    typedef TbFrameworkSimpleImpl Self;
public:

    // コンストラクタ
    TbFrameworkSimpleImpl( const TbFrameworkSimple::InitParam& initParam )
        :  m_Keyboard(nullptr)
        , m_Pad(nullptr)
        , m_Mouse(nullptr)
        , m_Touch(nullptr)
        , m_DebugMenuManager(nullptr)
        , m_Font(nullptr)
        , m_FontBuffer(nullptr)
        , m_InitParam( initParam )
        , m_ClearColor( TbClearColor::White() )
        , m_UpdateFunc(nullptr)
        , m_DrawFunc(nullptr)
        , m_AllocatorViewer(nullptr)
        , m_HeapAllocatorView()
    {
        switch( initParam.windowSizeType )
        {
        case TbFrameworkSimple::InitParam::WINDOW_SIZE_DISPLAY_ASPECT_SCALE_WIDTH:
            {
                TbDisplayMode mode;
                if( TbDisplayManager::GetInstance().GetCurrentDisplayMode(0,&mode) )
                {
                    TbFloat32 srcRatio = static_cast<TbFloat32>(initParam.windowWidth)/static_cast<TbFloat32>(initParam.windowHeight);
                    TbFloat32 dstRatio = 0.0f;
                    if( mode.width < mode.height ) {
                        dstRatio = static_cast<TbFloat32>(mode.height)/static_cast<TbFloat32>(mode.width);
                    }else{
                        dstRatio = static_cast<TbFloat32>(mode.width)/static_cast<TbFloat32>(mode.height);
                    }
                    if( srcRatio < dstRatio ) {
                        m_InitParam.windowWidth = dstRatio*static_cast<TbFloat32>(initParam.windowHeight);
                    }
                }
            }
            break;
        }

#if TB_PLATFORM_IS_FLASH || TB_PLATFORM_IS_NX
        // 書き換え(ディスプレイサイズが強制ウィンドウサイズ)
        {
            TbDisplayMode displayMode;
            TbDisplayManager::GetInstance().GetCurrentDisplayMode(0,&displayMode);
            m_InitParam.windowWidth = displayMode.width;
            m_InitParam.windowHeight = displayMode.height;
        }
#elif TB_PLATFORM_IS_EMSCRIPTEN
        TbDisplayManagerDepend::Initialize(m_InitParam.windowWidth,
                                           m_InitParam.windowHeight,
                                           TB_COLOR_FORMAT_R5G6B5,60.0f);
#endif

#if (TB_PLATFORM_IS_WIN)
        // ログ設定
        if( initParam.bitArray.Test(TbFrameworkSimple::InitParam::FLAG_USE_LOG) ) {
            m_Log.Start("log.txt",TB_FILE_PATH_ID_NONE);
        }
#endif

        // ウィンドウ生成
        TbWindow* window = nullptr;
        {
            TbWindowParameter windowParam;
            windowParam.rect.x = m_InitParam.windowX;
            windowParam.rect.y = m_InitParam.windowY;
            windowParam.rect.width = m_InitParam.windowWidth;
            windowParam.rect.height = m_InitParam.windowHeight;
            windowParam.name = initParam.windowTitle;
            windowParam.isVisible = TB_TRUE;
            window = TbWindowManager::GetInstance().CreateWindowWithParam( windowParam );
            m_ScreenSize = window->GetSize();
        }
    
        // デバイス作成
        {
            TbRenderDevice& device = TbRenderDevice::GetInstance();
            TbRenderViewParameter param;
            TbMemClear( &param , sizeof( param ) );
            param.backBufferHeight = 0;
            param.backBufferWidth = 0;
            param.backBufferFormat = TB_COLOR_FORMAT_UNKNOWN;
            param.backBufferNum = 0;
            param.enableCreateDepthStencil = TB_TRUE;
#if TB_PLATFORM_IS_EMSCRIPTEN || TB_PLATFORM_IS_IOS
            param.autoDepthStencilsFormat = TB_COLOR_FORMAT_D16;
#else
            param.autoDepthStencilsFormat = TB_COLOR_FORMAT_D24S8;
#endif
            param.isFullscreen = initParam.bitArray.Test(TbFrameworkSimple::InitParam::FLAG_FULLSCREEN);
            param.multiSampleType = TB_MULTISAMPLE_NONE;
            device.Initialize( 0 , window , 0 , param );
        }

#if TB_PLATFORM_IS_ANDROID || TB_PLATFORM_IS_IOS
        // 書き換え
        {
            TbDisplayMode displayMode;
            TbDisplayManager::GetInstance().GetCurrentDisplayMode(0,&displayMode);
            window->SetSize(displayMode.width,displayMode.height);
        }
#endif

        // キーボード設定
        if( initParam.bitArray.Test(TbFrameworkSimple::InitParam::FLAG_USE_KEYBOARD) ){
            TbKeyboardParameter keyboardParam = { window };
            m_Keyboard = TB_NEW TbKeyboard( &keyboardParam , 1 );
        }

        // パッド設定
        if( initParam.bitArray.Test(TbFrameworkSimple::InitParam::FLAG_USE_PAD) ){
            m_Pad = TB_NEW TbPad( window );
        }

        // マウス設定
        if( initParam.bitArray.Test(TbFrameworkSimple::InitParam::FLAG_USE_MOUSE) ){
            TbMouseParameter mouseParam = {window};
            m_Mouse = TB_NEW TbMouse( &mouseParam , 1 );
        }

        // タッチ設定
        if( initParam.bitArray.Test(TbFrameworkSimple::InitParam::FLAG_USE_TOUCH) ){
            TbTouchParameter touchParam = {window};
            m_Touch = TB_NEW TbTouch( &touchParam , 1 );
        }

        // フォント設定(ASCII)
        {
            m_FontTexture.Create("system/font/font_ascii.tga",0,TB_FILE_SYNC);
            m_FontTexture.Activate();
            m_Font = TB_NEW TbFontAscii();
            m_Font->SetTexture(m_FontTexture.GetTexture());
            m_FontBuffer = TB_NEW TbFontBuffer2d(1000,TbFrameworkSimple::RENDER_PASS_2D);
            m_FontBuffer->SetFont(m_Font);
            m_FontBuffer->SetFontSize(16);
            m_FontBuffer->SetColor(TbColor::Cyan());
        }
        // デバッグメニュー
        m_DebugMenuManager = TB_NEW TbDebugMenuManager();
        m_DebugMenuManager->SetFontBuffer( m_FontBuffer );

        TbMatrixSetOrthoOffCenterLH(m_OrthoMatrix,
                                    0.0f,
                                    static_cast<TbFloat32>(m_InitParam.windowWidth),
                                    -static_cast<TbFloat32>(m_InitParam.windowHeight),
                                    0.0f,0.1f,10.0f);

        // ビュー

        // 2Dパス
        TbRenderPassManager& passManager = TbRenderPassManager::GetInstance();
        {
            TbRenderPass* pass = passManager.GetPass(TbFrameworkSimple::RENDER_PASS_2D_BACK);
            pass->SetBeginCallback(TbCreateMemFunc(this,&Self::onBeginRenderPassBack2d));
        }
        // 3Dパス
        {
            TbRenderPass* pass3d = passManager.GetPass(TbFrameworkSimple::RENDER_PASS_3D);
            pass3d->SetBeginCallback(TbCreateMemFunc(this,&Self::onBeginRenderPass3d));
        }
        // 2Dパス
        {
            TbRenderPass* pass2d = passManager.GetPass(TbFrameworkSimple::RENDER_PASS_2D);
            pass2d->SetBeginCallback(TbCreateMemFunc(this,&Self::onBeginRenderPass2d));
        }
        // デバッグ2Dパス
        {
            TbRenderPass* passDebug2d = passManager.GetPass(TbFrameworkSimple::RENDER_PASS_DEBUG_2D);
            passDebug2d->SetBeginCallback(TbCreateMemFunc(this,&Self::onBeginRenderPass2d));
        }
        
        // ユーティリティ系初期設定
        TbDraw2dSetRenderPass(TbFrameworkSimple::RENDER_PASS_2D);
        TbDraw3dSetRenderPass(TbFrameworkSimple::RENDER_PASS_3D);

        // サイズ設定
        m_Fade.SetSize(initParam.windowWidth,initParam.windowHeight);

        // アロケータ情報表示
        {        
            m_HeapAllocatorView.SetAllocator(&TbAllocatorHolderGlobal::GetInstance().GetValue());
            m_HeapAllocatorView.SetName("Global");
            m_AllocatorViewer = TB_NEW TbAllocatorViewer();
            m_AllocatorViewer->AddView(&m_HeapAllocatorView);
            m_AllocatorViewer->SetPos(30,m_InitParam.windowHeight-60);
            m_AllocatorViewer->SetFontBuffer(m_FontBuffer);
        }

        // デバッグメニュー
        {
            TbDebugMenu* menu = m_DebugMenuManager->CreateMenu("TbFrameworkSimple");
            TbDebugMenuItemBool* memoryItem = menu->CreateItemBool("Draw Memory");
            memoryItem->Bind( TbCreateMemFunc(m_AllocatorViewer,&TbAllocatorViewer::SetVisible) ,
                              TbCreateMemFunc(m_AllocatorViewer,&TbAllocatorViewer::IsVisible));
        }
    }

    // デストラクタ
    ~TbFrameworkSimpleImpl()
    {
        TB_SAFE_DELETE( m_Pad );
        TB_SAFE_DELETE( m_Keyboard );
        TB_SAFE_DELETE( m_Mouse );
        TB_SAFE_DELETE( m_Touch );
        TB_SAFE_DELETE( m_DebugMenuManager );
        TB_SAFE_DELETE( m_Font );
        TB_SAFE_DELETE( m_FontBuffer );
        m_FontTexture.Destroy();
        TB_SAFE_DELETE(m_AllocatorViewer);
    }

public:

    // 2D画面サイズ取得
    const TbDimensionS32& GetScreenSize()
    {
        return m_ScreenSize;
    }

    // クリアカラー設定
    void SetClearColor( const TbClearColor& color )
    {
        m_ClearColor = color;
    }

    // メインループ用関数設定
    void SetMainLoopFunc( void (*main)() , void (*draw)() )
    {
        m_UpdateFunc = main;
        m_DrawFunc = draw;
    }

    // 画面に文字表示
    void PrintfScreen( TbSint32 x , TbSint32 y , const TbChar8 *text )
    {
        if( !m_FontBuffer ) {
            return;
        }
        m_FontBuffer->Printf( x , y , text );
    }

    // フェードカラー設定 
    void    SetFadeColor( const TbColorU32& color )
    {
        m_Fade.SetColor( color );
    }

    // フェードカラー取得
    TbColorU32    GetFadeColor()
    {
        return m_Fade.GetColor();
    }

    // フェードイン
    void     FadeIn( TbFloat32 sec )
    {
        m_Fade.FadeIn(sec);
    }

    // フェードアウト
    void     FadeOut( TbFloat32 sec )
    {
        m_Fade.FadeOut(sec);
    }

    // フェード中か
    TbBool IsFading() const {
        return m_Fade.IsFading();
    }

    // 更新
    void Update()
    {
        if( m_Pad ) {
            m_Pad->Update();
        }
        if( m_Keyboard ) {
            m_Keyboard->Update();
            if( m_Keyboard->IsPress(TB_KEY_ESCAPE) ){
                TbFramework::FinishMainLoop();
            }
        }
        if( m_Mouse ){
            m_Mouse->Update();
        }
        if( m_Touch ){
            m_Touch->Update();
        }
        if( m_DebugMenuManager ) {
            m_DebugMenuManager->Update();
        }
        if( m_UpdateFunc ){
            m_UpdateFunc();
        }
#if TB_IS_DEBUG && TB_PLATFORM_IS_WIN
        if( TbKeyboard::IsOn( TB_KEY_CONTROL_L ) && TbKeyboard::IsPress( TB_KEY_P ) ){
            const TbDate& date = TbTime::GetDate();
            const TbStaticString64& filePath = TbStaticString64::Format("capture%d%d%d%d%d.tga",date.month,date.day,date.hour,date.minute,date.second);
            TbScreenShot::CaptureAndExportSync( filePath.GetBuffer() , TbScreenShot::FILE_TGA );
        }
#endif
        m_Fade.Update();
    }

    // 描画
    void Draw()
    {
        if( m_DrawFunc ){
            m_DrawFunc();
        }
        if( m_DebugMenuManager ) {
            m_DebugMenuManager->Draw();
        }
        m_AllocatorViewer->Draw();
        if( m_FontBuffer ) {
            m_FontBuffer->Flush();
        }
        m_Fade.Draw();
    }


private:

    // 2Dパス用デフォルト設定
    void onBeginRenderPass2d( TbRenderPass& pass ) 
    {
        pass.SetWorldViewProjectionMatrix(TbMatrix::IDENTITY,
                                          TbMatrix::IDENTITY,
                                          m_OrthoMatrix);
        pass.SetFixedShaderEnableLighting(TB_FALSE);
        pass.SetDepthStencilState( TbRenderStatePreset::DEPTH_STENCIL_2D );
    }

    // 先頭パス用デフォルト設定
    void onBeginRenderPassBack2d( TbRenderPass& pass ) 
    {
        TbUint32 clearTargetBit = TB_RENDER_BUFFER_TYPE_COLOR_BIT;
        if( !m_InitParam.disableRenderPass[TbFrameworkSimple::RENDER_PASS_3D] ){
            // 3Dを利用しない場合、深度とステンシルを無効か
            clearTargetBit |= TB_RENDER_BUFFER_TYPE_DEPTH_BIT|
                              TB_RENDER_BUFFER_TYPE_STENCIL_BIT;
        }
        pass.Clear(clearTargetBit,
                   m_ClearColor ,
                   1.0f,
                   0);
        onBeginRenderPass2d(pass);
    }

    // 3Dパス用デフォルト設定
    void onBeginRenderPass3d( TbRenderPass& pass ) 
    {
        if( !m_InitParam.disableRenderPass[TbFrameworkSimple::RENDER_PASS_3D] ){
            pass.SetDepthStencilState(TbRenderStatePreset::DEPTH_STENCIL_3D_NORMAL);
        }
    }

private:

    TbKeyboard*                  m_Keyboard;            // キーボード
    TbPad*                       m_Pad;                 // パッド
    TbMouse*                     m_Mouse;               // マウス
    TbTouch*                     m_Touch;               // タッチ
    TbDebugMenuManager*          m_DebugMenuManager;    // デバッグメニュー管理クラス
    TbFont*                      m_Font;                // ASCIIフォント
    TbFontBuffer2d*              m_FontBuffer;
    TbResourceHandleTexture      m_FontTexture;
    TbMatrix                     m_OrthoMatrix;
    TbDimensionS32               m_ScreenSize;
    TbFrameworkSimple::InitParam m_InitParam;
    TbClearColor                 m_ClearColor;
    void                         (*m_UpdateFunc)();
    void                         (*m_DrawFunc)();
    TbAllocatorViewer*           m_AllocatorViewer;
    TbHeapAllocatorView          m_HeapAllocatorView;
    TbFade                       m_Fade;
    TbLog                        m_Log;
};

static TbFrameworkSimpleImpl* s_Impl = nullptr;

void updateImpl()
{
    TB_PROFILER_SCOPE("updateImpl");
    if( s_Impl ){
        s_Impl->Update();
    }
}

void drawImpl()
{
    TB_PROFILER_SCOPE("drawImpl");
    if( s_Impl ){
        s_Impl->Draw();
    }
}


/*!
 * デフォルトパラメータを取得
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
void TbFrameworkSimple::GetDefaultInitParam( TbFrameworkSimple::InitParam& param )
{
    // 共通フレームワークの初期値
    {
        TbFrameworkInitParam initParam;
        TbFramework::GetDefaultInitParam(initParam);
        TbMemCopy(param.allocParam,initParam.allocParam,sizeof(param.allocParam));
        param.renderViewMax = initParam.renderViewMax;
        param.taskLineMax = initParam.taskLineMax;
    }
    TbMemClear(param.disableRenderPass);
    param.windowX = 0;
    param.windowY = 0;
    param.windowWidth = 640;
    param.windowHeight = 480;
    param.windowSizeType = InitParam::WINDOW_SIZE_DIRECT;
    param.windowTitle = "";
    param.projectName = nullptr;
    param.rootPath = nullptr;
    param.savePath = nullptr;
    
    param.bitArray.SetBit( InitParam::FLAG_USE_KEYBOARD );
    param.bitArray.SetBit( InitParam::FLAG_USE_PAD );
    param.bitArray.SetBit( InitParam::FLAG_USE_MOUSE );
}

/*!
 * 初期化(デフォルト設定)
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkSimple::Initialize()
{
    InitParam param;
    GetDefaultInitParam(param);
    return Initialize(param);
}

/*!
 * 初期化(カスタム設定可能)
 * @retval TB_S_OK 正常完了
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkSimple::Initialize( const InitParam& initParam )
{
    if( !s_Impl )
    {
        // 共通フレームワーク初期化
        {
            TbStaticString128 rootPath;
            TbStaticString128 savePath;
            TbFrameworkInitParam frameworkParam;
            TbFramework::GetDefaultInitParam(frameworkParam);
            TbMemCopy(frameworkParam.allocParam,initParam.allocParam,sizeof(frameworkParam.allocParam));
            frameworkParam.renderViewMax = initParam.renderViewMax;
            frameworkParam.taskLineMax = initParam.taskLineMax;
            frameworkParam.renderPassMax = TbFrameworkSimple::RENDER_PASS_MAX;
            if( initParam.projectName ){
#if TB_PLATFORM_IS_EMSCRIPTEN // webサーバー上ではプロジェクト名のフォルダ以下にデータを配置
                rootPath.SetFormatedString("/%s%s",initParam.projectName,frameworkParam.rootPath);
                savePath.SetFormatedString("/%s%s",initParam.projectName,frameworkParam.savePath);
                frameworkParam.rootPath = rootPath.GetBuffer();
                frameworkParam.savePath = savePath.GetBuffer();
#elif !TB_PLATFORM_IS_NX
                if( initParam.rootPath ){
                    frameworkParam.rootPath = initParam.rootPath;
                }
                if( initParam.savePath ){
                    frameworkParam.savePath = initParam.savePath;
                }
#endif
            }
            TbFramework::Initialize(frameworkParam);
        }

        s_Impl = new TbFrameworkSimpleImpl(initParam);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 終了処理
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */
TbResult TbFrameworkSimple::Finalize()
{
    TB_SAFE_DELETE(s_Impl);
    TbFramework::Finalize();
    return TB_S_OK;
}

/*!
 * メインループ開始
 * @author Miyake Shunsuke
 * @since 2013.03.12
 */
TbResult TbFrameworkSimple::StartMainLoop( void (*begin)() , void (*main)() , void (*draw)() , void (*end)() , void(*drawDirect)())
{
    if( !s_Impl ) {
        return TB_E_FAIL;
    }
    s_Impl->SetMainLoopFunc( main , draw );
    return TbFramework::StartMainLoop(begin,updateImpl,drawImpl,end, drawDirect);
}

/*!
 * 2D画面サイズ取得
 * @author Miyake Shunsuke
 * @since 2013.03.12
 */
TbDimensionS32 TbFrameworkSimple::GetScreenSize()
{
    if( !s_Impl ) {
        return TbDimensionS32();
    }
    return s_Impl->GetScreenSize();
}

/*!
 * クリアカラー設定
 * @author Miyake Shunsuke
 * @since 2013.07.15
 */
void    TbFrameworkSimple::SetClearColor( const TbClearColor& color )
{
    if( !s_Impl ){
        return;
    }
    s_Impl->SetClearColor( color );
}

/*!
 * 画面に文字表示
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
void     TbFrameworkSimple::PrintfScreen( TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... )
{
    if( !s_Impl ){
        return;
    }
    va_list ap;
    TbChar8 text[512];

    va_start( ap, fmt );
    TbVStrPrintf( text , TB_ARRAY_COUNT_OF(text) , fmt, ap );
    va_end( ap );

    s_Impl->PrintfScreen( x , y , text );
}

/*!
 * フェードカラー設定
 * @author Miyake_Shunsuke
 */
void     TbFrameworkSimple::SetFadeColor( const TbColorU32& color )
{
    if( s_Impl ){
        s_Impl->SetFadeColor( color );
    }
}

/*!
 * フェードカラー取得
 * @author Miyake_Shunsuke
 */
TbColorU32  TbFrameworkSimple::GetFadeColor()
{
    if (s_Impl) {
        s_Impl->GetFadeColor();
    }
    return TbColorU32::White();
}

/*!
 * フェードイン
 * @author Miyake_Shunsuke
 */
void     TbFrameworkSimple::FadeIn( TbFloat32 sec )
{
    if( s_Impl ){
        s_Impl->FadeIn( sec );
    }
}

/*!
 * フェードアウト
 * @author Miyake_Shunsuke
 */
void     TbFrameworkSimple::FadeOut( TbFloat32 sec )
{
    if( s_Impl ){
        s_Impl->FadeOut(sec);
    }
}

/*!
 * フェード中か
 * @author teco
 */
TbBool     TbFrameworkSimple::IsFading()
{
    if (s_Impl) {
        return s_Impl->IsFading();
    }
    return TB_FALSE;
}

}
