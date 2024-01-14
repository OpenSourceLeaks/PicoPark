/*!
 * @file
 * @brief レンダービューのクラス
 * @author teco
 */

#import "graphics/render/3dapi/depend/gl/ios/tb_render_view_gl_ios_impl.h"
#import "graphics/render/3dapi/tb_render_view.h"
#import "graphics/render/3dapi/tb_depth_stencil.h"
#import "graphics/render/3dapi/tb_render_target.h"
#import "graphics/render/3dapi/tb_render_device.h"

namespace toybox
{
    
//////////////////////////////
// TbRenderViewDependImpl
//////////////////////////////

GLKViewController* TbRenderViewDependImpl::s_ViewController = nil;

/*!
 * コンストラクタ
 * @author teco
 */
TbRenderViewDependImpl::TbRenderViewDependImpl()
    : m_Context(nil)
{
    TB_RESQUE(s_ViewController);
    TB_RESQUE(!m_Context);
    m_Context = [[EAGLContext alloc ] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    GLKView* view = (GLKView*)s_ViewController.view;
    view.context = m_Context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    SetCurrent();}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderViewDependImpl::~TbRenderViewDependImpl()
{
    if ([EAGLContext currentContext] == m_Context) {
        [EAGLContext setCurrentContext:nil];
    }
    m_Context = nil;
}

/*!
 * カレント設定
 * @author teco
 */
void TbRenderViewDependImpl::SetCurrent()
{
    [EAGLContext setCurrentContext:m_Context];
}

//////////////////////////////
// TbRenderViewDepend
//////////////////////////////
    
/*!
 * コンストラクタ
 * @author teco
 */
TbRenderViewDepend::TbRenderViewDepend( const TbRenderViewParameter& param )
{
    m_Impl = new TbRenderViewDependImpl();
}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    TB_SAFE_DELETE( m_RenderTarget );
    TB_SAFE_DELETE( m_Impl );
}

/*!
 * カレントビューに設定。設定をコピーするビューの設定可
 * @author teco
 */
void TbRenderViewDepend::SetCurrent( TbRenderView* copySrc )
{
    // マルチウィンドウサポートしているプラットフォームのみ
    if( copySrc && &copySrc->GetDepend() != this ){
        TB_ASSERT(!"unsupported")
    }
    m_Impl->SetCurrent();
}

//////////////////////////////
// TbRenderView
//////////////////////////////
    
/*!
 * コンストラクタ
 * @author teco
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
 * @author teco
 */
TbRenderView::~TbRenderView()
{
    TB_SAFE_DELETE( m_DepthStencil );
}

/*!
 * 画面に表示
 * @author teco
 */
void TbRenderView::Present()
{
    TbRenderDevice::GetInstance().GetViewport(m_PresentRegion);
}

}
