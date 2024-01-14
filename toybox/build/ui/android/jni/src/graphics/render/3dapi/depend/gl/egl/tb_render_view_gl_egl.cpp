/*!
 * @file
 * @brief レンダービュークラス（Windows用)
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_view.h"

#if TB_3DAPI_IS_GL && TB_GLAPI_IS_GLES

#include "base/util/tb_depend_cast.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/window/tb_window.h"
#include "graphics/display/tb_display_manager.h"

namespace toybox
{

/////////////////////////////////////////////////////////////////////
// 依存コード

/*!
 * コンストラクタ
 * @parma device 対応デバイス
 * @param param 生成パラメータ
 * @param index 既に生成済みのSwapChainがあれば、そのインデックス
 * @author teco
 */
TbRenderViewDepend::TbRenderViewDepend( const TbRenderViewParameter& param )
    : m_Display(nullptr)
    , m_Config(nullptr)
    , m_Surface(nullptr)
    , m_Context(nullptr)
    , m_RenderTarget(nullptr)
{
#if (TB_PLATFORM_IS_WIN)
    HWND hWnd = param.window->GetDepend().GetWindowHandle();
#elif TB_PLATFORM_IS_EMSCRIPTEN
    Window& hWnd = param.window->GetDepend().window;
#endif
    
    if( hWnd )
    {
        EGLint major, minor;
        EGLint attr[20];
        m_Display = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);
        
         //ディスプレイの初期化
        :: eglInitialize(m_Display, &major, &minor);
            
        //属性の設定
        TbColorFormat colorFormat = ( param.backBufferFormat != TB_COLOR_FORMAT_UNKNOWN ) ? param.backBufferFormat : TB_COLOR_FORMAT_R5G6B5;
         attr[0] = EGL_BUFFER_SIZE;
         attr[1] = TbGetColorFormatBitSize( colorFormat );
         attr[2] = EGL_RENDERABLE_TYPE;
#if TB_GLAPI_IS_GLES2
         attr[3] = EGL_OPENGL_ES2_BIT;
#elif TB_GLAPI_IS_GLES1
         attr[3] = EGL_OPENGL_ES_BIT;
#endif
         attr[4] = EGL_DEPTH_SIZE;
         attr[5] = param.enableCreateDepthStencil ? TbGetColorFormatDepthBitSize( param.autoDepthStencilsFormat ) : 0;
         attr[6] = EGL_SURFACE_TYPE;
         attr[7] = EGL_WINDOW_BIT;
         attr[8] = EGL_NONE;

         //コンフィグの選択
         EGLint numConfigs;
         ::eglChooseConfig(m_Display, attr, &m_Config, 1, &numConfigs);
         TB_ASSERT(1==numConfigs);

         //サーフィスの取得(hWndはプラットフォームにより異なる)
         m_Surface = eglCreateWindowSurface(m_Display, m_Config, hWnd, NULL);

         //コンテキストの取得
         EGLint ctxattr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 
#if TB_GLAPI_IS_GLES2
            2,
#elif TB_GLAPI_IS_GLES1
            1,
#endif
            EGL_NONE,
         };

        TbRenderDevice& device = TbRenderDevice::GetInstance();
        TbRenderView* master = device.GetView(0);
        EGLContext sharedContext = EGL_NO_CONTEXT;
        if( master ) {
            sharedContext = master->GetDepend().GetRenderingContext();
        }
         m_Context = eglCreateContext(m_Display, m_Config, sharedContext, ctxattr );

         //コンテキストをカレントにする
         eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);    
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    if( nullptr != m_Context ){
        eglMakeCurrent(m_Display,nullptr,nullptr,nullptr);
        eglDestroyContext(m_Display,m_Context);
        eglDestroySurface(m_Display,m_Surface);
        eglTerminate(m_Display);
    }
}

/*!
 * カレントビューに設定
 * @param copySrc コピー元
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */
void TbRenderViewDepend::SetCurrent( TbRenderView* copySrc )
{
    // マルチウィンドウサポートしているプラットフォームのみ
    if( copySrc && &copySrc->GetDepend() != this ){
        TB_ASSERT(!"unsupported")
    }
    eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);    
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderView::TbRenderView( const TbRenderViewParameter& param , TbSint32 index )
    : m_BitArray()
    , m_Depend( param )
    , m_Param( param )
    , m_DepthStencil( nullptr )
    , m_FrontBufferFormat( TB_COLOR_FORMAT_UNKNOWN )
{
    m_Depend.m_RenderTarget = TB_NEW TbRenderTarget( this , 1 );
    m_DepthStencil = TB_NEW TbDepthStencil( this );
    m_FrontBufferFormat = TB_COLOR_FORMAT_X8R8G8B8;
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderView::~TbRenderView()
{
    TB_SAFE_DELETE( m_DepthStencil );
}

/*!
 * 画面に表示
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */
void TbRenderView::Present()
{
    eglSwapBuffers( m_Depend.m_Display , m_Depend.m_Surface );
}

}

#endif
