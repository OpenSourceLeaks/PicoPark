/*!
 * 描画パネルクラスの実装ファイル
 * @author teco
 * @since 2010.05.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_device.h"

#if TB_3DAPI_IS_GX2

#include "base/util/tb_depend_cast.h"
#include "graphics/render/3dapi/tb_3dapi_util.h"
#include "graphics/render/3dapi/tb_render_view.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"
#include "graphics/render/3dapi/tb_index_buffer.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/tb_texture_sampler.h"
#include "graphics/window/tb_window.h"
#include "graphics/display/tb_display_manager.h"

namespace
{
    static const GLenum PRIM_TYPE[ toybox::TB_PT_MAX ] = 
    {
        GX2_PRIMITIVE_POINTS , 
        GX2_PRIMITIVE_LINES  , 
        GX2_PRIMITIVE_LINE_STRIP , 
        GX2_PRIMITIVE_TRIANGLES , 
        GL_TRIANGLE_STRIP ,
        GX2_PRIMITIVE_TRIANGLE_FAN , 
    };
}

namespace toybox
{

namespace
{
    // テクスチャに対してサンプラーステートを設定する
    TbBool setTextureSampler( TbUint32 slot , TbTexture* texture , const TbTextureSampler* sampler )
    {
        GLenum minFilterMode;
        GLenum magFilterMode;
        GLenum addressModeU;
        GLenum addressModeV;
        // パラメータチェック
        if( !TbTextureFilterModeToGLenum( &minFilterMode , sampler->GetMinFilterMode() ) ||
            !TbTextureFilterModeToGLenum( &magFilterMode , sampler->GetMagFilterMode() ) ||
            !TbTextureAddressModeToGLenum( &addressModeU , sampler->GetAddressModeU() ) || 
            !TbTextureAddressModeToGLenum( &addressModeV , sampler->GetAddressModeV() ) ){
            return TB_FALSE;
        }
        if( texture ){
            // glActiveTexture( GL_TEXTURE0 ):
            // glBindTexture( GL_TEXTURE_2D , texture->GetDepend().GetTexture() );
            glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , minFilterMode );
            glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , magFilterMode );
            glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , addressModeU );
            glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , addressModeV );
        }
        return TB_TRUE;
    }

    // ビューポート更新
    void updateViewport( TbRenderDevice* device , TbRenderTarget* renderTarget )
    {
        //TB_ASSERT(!"Support device->GetPresentType()");
        if( renderTarget )
        {
            switch( renderTarget->GetType() )
            {
            case TB_RENDER_TARGET_TYPE_BUFFER:
            case TB_RENDER_TARGET_TYPE_TEXTURE:
                {
                    // ビューポート自動切換え(解像度に合わせる)
                    device->SetViewport( 0 , 0 , renderTarget->GetWidth() , 
                                                 renderTarget->GetHeight() );
                }
                break;
            case TB_RENDER_TARGET_TYPE_VIEW:
                {
                    TbWindow* window = renderTarget->GetDepend().view->GetWindow();
                    toybox::TbRectS32 rect = device->GetPresentRect(device->GetPresentType(),renderTarget,window);
                    // ビューポート自動切換え(ウィンドウのサイズに合わせる)
                    device->SetViewport( rect.x , rect.y , rect.width , rect.height );
//                    device->SetViewport( 0 , 0 , renderTarget->GetDepend().view->GetWindow()->GetWidth() , 
//                                                 renderTarget->GetDepend().view->GetWindow()->GetHeight() );
                }
                break;
            }
        }
    }
}

// 依存コード

/*!
 * コンストラクタ
 * @author teco
 * @since 2010.07.13
 */
TB_INLINE TbRenderDeviceDepend::TbRenderDeviceDepend()
    : m_BitArray()
    , m_RenderView( nullptr )
    , m_FboId( 0 )
    , m_DefaultDepthStencil( nullptr )
    , m_DefaultTextureSampler()
    , m_CurrentTextureStage( 0 )
{
    m_DefaultTextureSampler.SetMinFilterMode( TB_TEXTURE_FILTER_MODE_POINT );
    m_DefaultTextureSampler.SetMagFilterMode( TB_TEXTURE_FILTER_MODE_POINT );
    m_DefaultTextureSampler.SetAddressModeU( TB_TEXTURE_ADDRESS_MODE_CLAMP );
    m_DefaultTextureSampler.SetAddressModeV( TB_TEXTURE_ADDRESS_MODE_CLAMP );

    TbMemClear( m_VertexStream , sizeof(m_VertexStream) );
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(PRIM_TYPE) == TB_PT_MAX );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2010.07.13
 */
TB_INLINE TbRenderDeviceDepend::~TbRenderDeviceDepend()
{
    TB_SAFE_DELETE(m_DefaultDepthStencil);
}

/*!
 * セットアップ
 * @param view ベースビュー
 * @author teco
 * @since 2010.07.13
 */
TB_INLINE TbBool TbRenderDeviceDepend::Initialize( TbRenderView* view )
{
    if( view ){
        m_RenderView = view;
        m_RenderView->GetDepend().SetCurrent();
        TbGlGenFramebuffers( 1 , &m_FboId );
        m_DefaultDepthStencil = TB_NEW TbDepthStencil( 512 , 512 , TB_COLOR_FORMAT_D16 );
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 頂点ストリーム設定
 * @param stream ストリーム番号
 * @param offset オフセット
 * @param stride ストライド
 * @author teco
 * @since 2010.08.12
 */
void TbRenderDeviceDepend::SetVertexStream( TbSint32 stream , TbVertexBuffer* vertexBuffer , TbUint32 offset , TbUint32 stride )
{
    if( 0 <= stream && stream < TB_VERTEX_STREAM_MAX ){
        m_VertexStream[ stream ].vertexBuffer = vertexBuffer;
        m_VertexStream[ stream ].offset = offset;
        m_VertexStream[ stream ].stride = stride;
    }
}

/*!
 * アクティブなテクスチャステージの取得
 * @author teco
 * @since 2011.09.23
 */
TbUint32 TbRenderDeviceDepend::GetCurrentTextureStage() const
{
    return m_CurrentTextureStage;
}

/*!
 * コンストラクタ
 * @author teco
 * @since 2010.05.10
 */
TbRenderDevice::TbRenderDevice( TbSizeT viewMax )
    : m_Window( nullptr )
    , m_AdapterIndex(0)
    , m_IndexBuffer( nullptr )
    , m_FixedShader()
    , m_PresentType( TB_RENDER_PRESENT_DEFAULT )
    , m_ViewList( viewMax )
    , m_ActiveTextureCount( 0U )
    , m_DepthStencil( nullptr )
    , m_DepthStencilState()
    , m_BlendState()
    , m_Depend()
{
    TbMemClear( m_Texture , sizeof(m_Texture) );
    TbMemClear( m_TextureSampler , sizeof( m_TextureSampler ) );
    TbMemClear( m_RenderTarget , sizeof(m_RenderTarget) );
    initDepthStencilState();
    initBlendState();
    initAlphaTestState();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2010.05.10
 */
TbRenderDevice::~TbRenderDevice()
{
    TbRenderView* view = nullptr;
    TbForIterator( it , m_ViewList ) {
        view = *it;
        TB_SAFE_DELETE( view );
    }
}

/*!
 * セットアップ
 * @param window 対応ウィンドウ
 * @param parameter パラメータ
 * @author teco
 * @since 2010.05.10
 */
TbBool TbRenderDevice::Initialize( TbUint32 adapterIndex , TbWindow* focusWindow , TbUint32 flags , const TbRenderViewParameter& parameter )
{
    if( focusWindow )
    {
        m_AdapterIndex = adapterIndex;
        m_Window = focusWindow;
        TbRenderViewParameter localParam;
        TbMemCopy( &localParam , &parameter , sizeof(localParam) );
        localParam.window = m_Window;

        TbRenderView* view = CreateView( localParam );
        m_RenderTarget[0] = view->GetRenderTarget(0); // デフォルトレンダーターゲット

        m_BitArray.SetBit( FLAG_FULLSCREEN , TbDisplayManager::GetInstance().IsFullscreen( m_AdapterIndex ) );
        if( m_Depend.Initialize( GetView(0) ) ){
//            SetDepthStencil( m_Depend.GetDefaultDepthStencil() );
            if( parameter.enableCreateDepthStencil ){
                SetDepthWriteMask( TB_DEPTH_MASK_ALL );
                SetEnableDepthTest( TB_TRUE );
            }
            initCommon();
            m_BitArray.SetBit(FLAG_INITIALIZE);
            return TB_TRUE;
        }
        return TB_FALSE;
    }
    return TB_FALSE;
}

/*!
 * 性能チェック
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbRenderDevice::GetCaps( Caps& caps )
{
    // 未実装
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
 * @since 2011.01.22
 */
TbBool TbRenderDevice::GetCurrentDisplayMode( TbUint32 index , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetCurrentDisplayMode( index , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
 * @since 2011.01.22
 */
TbBool TbRenderDevice::GetDisplayMode( TbUint32 index , TbSint32 mode , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetDisplayMode( index , mode , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
 */
TbBool TbRenderDevice::GetDisplayMode( TbUint32 index , TbSint32 mode , TbColorFormat format , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetDisplayMode( index , mode , format , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author teco
 */
TbSint32 TbRenderDevice::GetDisplayModeNum( TbUint32 index ) const
{
    return TbDisplayManager::GetInstance().GetDisplayModeNum( index );
}

/*!
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author teco
 */
TbSint32 TbRenderDevice::GetDisplayModeNum( TbUint32 index , TbColorFormat format ) const
{
    return TbDisplayManager::GetInstance().GetDisplayModeNum( index , format );
}


/*!
 * ビュー生成
 * @author teco
 */
TbRenderView* TbRenderDevice::CreateView( const TbRenderViewParameter& parameter )
{
    TbRenderView* result = nullptr;
    result = TB_NEW TbRenderView( parameter , -1 );
    if( result->IsError() ){
        TB_SAFE_DELETE( result );
    }else{
        m_ViewList.PushBack( result );
    }
    m_FixedShader.GetDepend().Setup(); // 固定シェーダのセットアップ
    return result;
}

/*!
 * ビューポート設定
 * @author teco
 */
TbResult TbRenderDevice::SetViewport( TbUint32 x , TbUint32 y , TbUint32 width , TbUint32 height )
{
    //if( m_Cache.SetViewPort(x,y,width,height) )
    {
        GX2SetViewport( x , y , width , height , 0.0f , 1.0f );
    }
    return TB_S_OK;
}

/*!
 * ビューポート取得
 * @author teco
 */
TbResult TbRenderDevice::GetViewport( TbRectS32& rect ) const
{
    GLint rectGL[4];
    glGetIntegerv( GL_VIEWPORT , rectGL );
    rect.x = rectGL[0];
    rect.y = rectGL[1];
    rect.width = rectGL[2];
    rect.height = rectGL[3];
    return TB_E_FAIL;
}

/*!
 * レンダーターゲット設定
 * @author teco
 */
TbResult TbRenderDevice::SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget )
{
    if( index >= RENDER_TARGET_MAX ){
        return TB_E_FAIL;
    }
    if( m_RenderTarget[index] == renderTarget ){
        return TB_S_OK;
    }

    if( renderTarget )
    {
        switch( renderTarget->GetType() )
        {
        case TB_RENDER_TARGET_TYPE_BUFFER: // バッファ
            if( TB_VERIFY(index < TB_ARRAY_COUNT_OF(TB_GL_COLOR_ATTACKMENT_TYPE) ) ){
                TbGlBindFramebuffer( TB_GL_FRAMEBUFFER , m_Depend.GetFboId());
                TbGlFramebufferTexture2D( TB_GL_FRAMEBUFFER , 
                                           TB_GL_COLOR_ATTACKMENT_TYPE[index] ,
                                           GL_TEXTURE_2D ,
                                           renderTarget->GetDepend().rboId ,
                                           0 );
            }else{
                TB_ASSERT(!"Unsupported Target");
            }
            break;
        case TB_RENDER_TARGET_TYPE_VIEW: // ビュー
            {
                TbGlBindFramebuffer( TB_GL_FRAMEBUFFER , 0 );
                TbRenderView* view = renderTarget->GetDepend().view;
                view->GetDepend().SetCurrent(m_Depend.GetRenderView());
                // 深度ステンシル自動切換え
                m_DepthStencil = view->GetDepthStencil();

            }
            break;
        case TB_RENDER_TARGET_TYPE_TEXTURE: // テクスチャ
            if( TB_VERIFY(index < TB_ARRAY_COUNT_OF(TB_GL_COLOR_ATTACKMENT_TYPE) ) ){
                TbGlBindFramebuffer( TB_GL_FRAMEBUFFER , m_Depend.GetFboId() );
                TbGlFramebufferTexture2D( TB_GL_FRAMEBUFFER , 
                                        TB_GL_COLOR_ATTACKMENT_TYPE[index] ,
                                        GL_TEXTURE_2D ,
                                        renderTarget->GetDepend().texture->GetDepend().GetTextureId() ,
                                        0 );
            }else{
                TB_ASSERT(!"Unsupported Target");
            }
            break;
        }
    }
    m_RenderTarget[index] = renderTarget;
    // 0番のレンダーターゲットが更新された場合、ビューポートも更新する
    if( 0 == index ){
        updateViewport( this , m_RenderTarget[index] );
    }
    return TB_S_OK;
}

/*!
 * 深度ステンシル設定
 * @author teco
 */
TbResult TbRenderDevice::SetDepthStencil( TbDepthStencil* depthStencil )
{
    if( m_DepthStencil == depthStencil ){
        return TB_S_OK;
    }

    if( depthStencil ){
        switch( depthStencil->GetType() )
        {
        case TB_DEPTH_STENCIL_TYPE_BUFFER:
            {
                TbGlBindFramebuffer( TB_GL_FRAMEBUFFER , m_Depend.GetFboId() );
                TbGlFramebufferRenderbuffer( TB_GL_FRAMEBUFFER , 
                                           TB_GL_DEPTH_ATTACHMENT ,
                                           TB_GL_RENDERBUFFER ,
                                           depthStencil->GetDepend().rboId );
            }
            break;
        case TB_DEPTH_STENCIL_TYPE_TEXTURE:
            {
                TbGlBindFramebuffer( TB_GL_FRAMEBUFFER , m_Depend.GetFboId() );
                TbGlFramebufferTexture2D( TB_GL_FRAMEBUFFER , 
                                        TB_GL_DEPTH_ATTACHMENT ,
                                        GL_TEXTURE_2D ,
                                        depthStencil->GetDepend().textureId ,
                                        0 );
            }
            break;
        case TB_DEPTH_STENCIL_TYPE_VIEW:
            if( !m_RenderTarget[0] || m_RenderTarget[0]->GetType() != TB_RENDER_TARGET_TYPE_VIEW ){
                // WIndows版では、WindowSystemが作成したレンダーターゲットと深度ステンシルは
                // 必ずセットなので、レンダーターゲットは必ずTB_RENDER_TARGET_VIEWである必要がある
                return TB_E_FAIL;
            }
            break;
        }
    }

    m_DepthStencil = depthStencil;
    return TB_S_OK;
}

/*!
 * シーン開始
 * @author teco
 */
void TbRenderDevice::BeginScene()
{
#if 0 // マルチビューの場合に弊害になるので、ビューの設定はしなくてよい？
    TbRenderView* view = GetView(0);
    if( view ) {
        view->GetDepend().SetCurrent();
    }
#endif
    // ビューポートのサイズを現在のレンダーターゲットに合わせる
    updateViewport( this , m_RenderTarget[0] );
}

/*!
 * クリア
 * @author teco
 */
TbResult TbRenderDevice::Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil )
{
    GLbitfield mask = 0;
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_COLOR_BIT ){
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_DEPTH_BIT ){
        // ここがTRUEになっていないとクリアできない
        SetDepthWriteMask(TB_DEPTH_MASK_ALL);
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_STENCIL_BIT ){
        mask |= GL_STENCIL_BUFFER_BIT;
    }
    GX2ClearColor( &m_Depend.m_ColorBuffer  color.r , color.g , color.b , color.a );
    GX2ClearDepthStencilEx( );
    return TB_S_OK;
}

/*!
 * カラーバッファに書き込むマスク設定
 * @param colorMaskBit TbColorWriteMaskBitの論理和
 * @author teco
 */
TbResult TbRenderDevice::SetColorWriteMask( TbUint32 colorMaskBit )
{
    TbBool redMask = ( colorMaskBit & TB_COLOR_MASK_BIT_RED ) > 0;
    TbBool greenMask = ( colorMaskBit & TB_COLOR_MASK_BIT_GREEN ) > 0;
    TbBool blueMask = ( colorMaskBit & TB_COLOR_MASK_BIT_BLUE ) > 0;
    TbBool alphaMask = ( colorMaskBit & TB_COLOR_MASK_BIT_ALPHA ) > 0;
    glColorMask( redMask , greenMask , blueMask , alphaMask );
    return TB_S_OK;
}

/*!
 * バッファに書き込むかどうかの設定取得
 * @return TbColorWriteMaskBitの論理和
 * @author teco
 */
TbUint32 TbRenderDevice::GetColorWriteMask()
{
    TbUint32 result = 0;
    return result;
}

/*!
 * ストリームソース設定
 * @param streamNumber ストリーム番号
 * @param vertexBuffer 頂点バッファ
 * @param offset オフセット
 * @param stride ストライド
 * @author teco
 */
void TbRenderDevice::SetVertexBuffer( TbUint32 streamNumber , TbVertexBuffer* vertexBuffer , TbUint32 offsetInBytes , TbUint32 stride )
{
    m_Depend.SetVertexStream( streamNumber , vertexBuffer , offsetInBytes , stride );
}

/*!
 * 頂点インデックスバッファ設定
 * @author teco
 */
void TbRenderDevice::SetIndexBuffer( TbIndexBuffer* indexBuffer )
{
    m_IndexBuffer = indexBuffer;
}

/*!
 * プリミティブ描画
 * @author teco
 */
TbResult TbRenderDevice::DrawPrimitive( const TbPrimitiveType primitiveType , TbUint32 startVertex , TbUint32 primitiveCount )
{
    TbBool isActiveShader = !isActiveFixedShader();
    if( isActiveShader ){
        m_ShaderManager.GetDepend().EnableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().EnableVertexAttribute();
    }

    glDrawArrays ( PRIM_TYPE[primitiveType] , 
                   startVertex , 
                   TbGetPrimitiveVertexCount(primitiveType,primitiveCount) ); 

    if( isActiveShader ){
        m_ShaderManager.GetDepend().DisableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().DisableVertexAttribute();
    }
    return TB_S_OK;
}

/*!
 * プリミティブ描画( 生の頂点配列使用 )
 * @author teco
 */
TbResult TbRenderDevice::DrawPrimitive( const TbPrimitiveType primitiveType , const TbUint32 primitiveCount , const void* vertexs , TbUint32 stride )
{
    if( !vertexs ){ // 頂点バッファが無いよー
        TB_ASSERT( !"vertexs is nullptr" );
        return TB_E_FAIL;
    }
    glBindBuffer( GL_ARRAY_BUFFER , 0 );

    TbBool isActiveShader = !isActiveFixedShader();
    if( isActiveShader ){
        m_ShaderManager.GetDepend().EnableVertexAttribute( vertexs , stride );
    }else{
        m_FixedShader.GetDepend().EnableVertexAttribute( vertexs , stride );
    }

    glDrawArrays ( PRIM_TYPE[primitiveType] , 
                   0 , 
                   TbGetPrimitiveVertexCount(primitiveType,primitiveCount) );

    if( isActiveShader ){
        m_ShaderManager.GetDepend().DisableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().DisableVertexAttribute();
    }
    return TB_S_OK;
}

/*!
 * 頂点インデックスを利用したプリミティブ描画
 * @author teco
 */
TbResult TbRenderDevice::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                           const TbUint32 minVertexIndex ,
                                           const TbUint32 vertexCount , 
                                           const TbUint32 startIndex , 
                                           const TbUint32 primitiveCount )
{
    GLenum format;
    TbUint32 offset = 0U;
    if( !m_IndexBuffer ){
        return TB_E_FAIL;
    }
    if( TB_S_OK != TbIndexBufferDepend::ConvertFormatToGLenum( format , m_IndexBuffer->GetFormat() ) ||
        TB_S_OK != TbIndexBufferDepend::GetByteCountFromFormat( offset , m_IndexBuffer->GetFormat() ) )
    {
        return TB_E_FAIL;
    }
    offset *= startIndex;
    TB_UNUSE( minVertexIndex );
    TB_UNUSE( vertexCount );
    TbBool isActiveShader = !isActiveFixedShader();
    if( isActiveShader ){
        m_ShaderManager.GetDepend().EnableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().EnableVertexAttribute();
    }

    // 描画
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , m_IndexBuffer->GetDepend().GetVboId() );
    glDrawElements( PRIM_TYPE[primitiveType] , 
                    TbGetPrimitiveVertexCount(primitiveType,primitiveCount) ,
                    format , reinterpret_cast<void*>(offset) );

    if( isActiveShader ){
        m_ShaderManager.GetDepend().DisableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().DisableVertexAttribute();
    }
    return TB_S_OK;
}

/*!
 * 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
 * @author teco
 */
TbResult TbRenderDevice::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                               const TbUint32 minVertexIndex ,
                                               const TbUint32 vertexCount , 
                                               const void* indexData , 
                                               const TbIndexBufferFormat indexFormat , 
                                               const void* vertexs , 
                                               TbUint32 stride ,
                                               const TbUint32 primitiveCount )
{
    TB_UNUSE( minVertexIndex );
    TB_UNUSE( vertexCount );
    
    GLenum format;
    if( TB_S_OK != TbIndexBufferDepend::ConvertFormatToGLenum( format , indexFormat ) ){
        // 引数が不正だよー
        return TB_E_FAIL;
    }

    if( !vertexs ){ // 頂点バッファが無いよー
        TB_ASSERT( !"vertexs is nullptr" );
        return TB_E_FAIL;
    }
    glBindBuffer( GL_ARRAY_BUFFER , 0 );

    TbBool isActiveShader = !isActiveFixedShader();
    if( isActiveShader ){
        m_ShaderManager.GetDepend().EnableVertexAttribute( vertexs , stride );
    }else{
        m_FixedShader.GetDepend().EnableVertexAttribute( vertexs , stride );
    }

    // 描画
    glDrawElements( PRIM_TYPE[primitiveType] , 
                    TbGetPrimitiveVertexCount(primitiveType,primitiveCount) ,
                    format , indexData );
    
    if( isActiveShader ){
        m_ShaderManager.GetDepend().DisableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().DisableVertexAttribute();
    }
    return TB_S_OK;
}

/*!
 * シーン終了
 * @author teco
 */
void TbRenderDevice::EndScene()
{
    GX2Flush();
}

/*!
 * テクスチャ設定
 * @author teco
 */
TbBool TbRenderDevice::SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType )
{
    GX2Texture* gx2Tex = texture ? texture->GetDepend().GetTexture() : nullptr;
    switch( shaderType )
    {
    case TB_SHADER_TYPE_VERTEX:
        GX2SetVertexTexture( gx2Tex , stage );
        break;
    case TB_SHADER_TYPE_PIXEL:
        GX2SetPixelTexture( gx2Tex , stage );
        m_Texture[ stage ] = texture;
        break;
    case TB_SHADER_TYPE_GEOMETRY:
        GX2SetGeometryTexture( gx2Tex , stage );
        break;
    }
    return TB_TRUE;
}

/*!
 * 現在利用されているテクスチャを取得
 * @author teco
 */
TbTexture*  TbRenderDevice::GetTexture( TbSint32 stage , TbShaderType shaderType )
{
    TbTexture* result = nullptr;
    if( TB_SHADER_TYPE_PIXEL == shaderType && 0 <= stage && stage < TEXTURE_STAGE_MAX )
    {
        result = m_Texture[ stage ];
    }
    return result;    
}

/*!
 * 表面が時計周りかどうか
 * @author teco
 */
TbResult TbRenderDevice::SetFrontFaceClockWise( TbBool isFrontFaceClockWise )
{
    glFrontFace( isFrontFaceClockWise ? GL_CW : GL_CCW );
    return TB_S_OK;
}

/*!
 * カリング設定
 * @author teco
 */
TbResult TbRenderDevice::SetCullMode( TbCullMode cullMode )
{
    switch( cullMode ){
    case TB_CULL_MODE_NONE:
        glDisable( GL_CULL_FACE );
        break;
    case TB_CULL_MODE_FRONT:        
        glEnable( GL_CULL_FACE );
        glCullFace( GL_FRONT );
        break;
    case TB_CULL_MODE_BACK:
        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
        break;
    default:
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * フィルモード設定
 * @author teco
 */
TbResult TbRenderDevice::SetFillMode( TbFillMode fillMode )
{
    // TB_FILL_MODE_SOLID固定
    if( fillMode != TB_FILL_MODE_SOLID){
        TB_ASSERT(!"Unsupported");
    }
    return TB_E_FAIL;
}

/*!
 * テクスチャサンプラーセット
 * @author teco
 */
TbResult TbRenderDevice::SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType )
{
    // 引数不正チェック
    if( slot >= TEXTURE_SAMPLER_SLOT_MAX || 
        TB_SHADER_TYPE_PIXEL != shaderType )
    {
        return TB_E_FAIL;
    }
    // nullptrの場合はここで終了
    if( !sampler ){
        m_TextureSampler[ slot ] = nullptr;
        return TB_S_OK;
    }
    TbTexture* texture = GetTexture( slot );
    if( TB_SUCCEEDED( m_Depend.SetCurrentTextureStage( slot ) ) && setTextureSampler( slot , texture , sampler ) ){
        m_TextureSampler[ slot ] = sampler;
    }
    return TB_S_OK;
}

/*!
 * テクスチャサンプラー取得
 * @author teco
 */
TbTextureSampler* TbRenderDevice::GetTextureSampler( TbUint32 slot , TbShaderType shaderType )
{
    // 引数不正チェック
    if( slot >= TEXTURE_SAMPLER_SLOT_MAX || 
        TB_SHADER_TYPE_PIXEL != shaderType )
    {
        return nullptr;
    }
    return m_TextureSampler[ slot ];
}

/*!
 * ブレンド有効設定
 * @author teco
 */
TbResult TbRenderDevice::SetEnableBlend( TbBool isEnable )
{
    TbUint32 index = !!isEnable;
    SET_ENABLE_FUNC_GL[index](GL_BLEND);
    m_BlendState.isEnableBlend = isEnable;
    return TB_S_OK;
}

/*!
 * ブレンド比較タイプ
 * @author teco
 */
TbResult TbRenderDevice::SetBlendOp( TbBlendOp op )
{
   GLenum opGL;
    if( TbBlendOpToGLenum( &opGL , op ) )
    {
        glBlendEquation( opGL );
        m_BlendState.blendOp = op;
        return TB_S_OK;    
    }
    return TB_E_FAIL;
 }

/*!
 * ブレンド係数
 * @author teco
 */
TbResult TbRenderDevice::SetBlendFactor( TbBlendFactor src , TbBlendFactor dst )
{
    GLenum srcFactorGL;
    GLenum dstFactorGL;
    if( TbBlendFactorToGLenum( &srcFactorGL , src ) && 
        TbBlendFactorToGLenum( &dstFactorGL , dst ) )
    {
        glBlendFunc( srcFactorGL , dstFactorGL );
        m_BlendState.blendSrcFactor = src;
        m_BlendState.blendDstFactor = dst;
        return TB_S_OK;    
    }
    return TB_E_FAIL;
}

/*!
 * アルファテスト有効設定
 * @author teco
 */
TbResult TbRenderDevice::SetEnableAlphaTest( TbBool isEnable )
{
#if TB_GLAPI_IS_GL2 || TB_GLAPI_IS_GLES1
    TbUint32 index = !!isEnable;
    SET_ENABLE_FUNC_GL[index](GL_ALPHA_TEST);
    m_AlphaTestState.isEnableAlphaTest = isEnable;
    return TB_S_OK;
#else
    // GL_ALPHA_TESTを使えない場合は、シェーダ側でdiscard命令を使って対応
    TB_ASSERT(!"Unsupported");
    return TB_E_FAIL;
#endif
}

/*!
 * アルファ比較関数
 * @author teco
 */
TbResult TbRenderDevice::SetAlphaTestFunc( TbCmpFunc func )
{
#if TB_GLAPI_IS_GL2 || TB_GLAPI_IS_GLES1
    GLenum funcGL;
    if( TbCmpFuncToGLenum( &funcGL , func ) == TB_S_OK ){
        m_AlphaTestState.alphaTestFunc = func;
        TbFloat32 refF32 = static_cast<TbFloat32>(m_AlphaTestState.alphaTestRef) / 255.0f;
        glAlphaFunc( funcGL , refF32 );
        return TB_S_OK;
    }
#else
    // gl関数を使えない場合は、シェーダ側で条件とdiscard命令を使って対応
    TB_ASSERT(!"Unsupported");
#endif
    return TB_E_FAIL;
}

/*!
 * アルファ比較関数
 * @author teco
 */
TbResult TbRenderDevice::SetAlphaTestRef( TbUint8 ref )
{
#if TB_GLAPI_IS_GL2 || TB_GLAPI_IS_GLES1
    GLenum funcGL;
    if( TbCmpFuncToGLenum( &funcGL , m_AlphaTestState.alphaTestFunc ) == TB_S_OK ){
        m_AlphaTestState.alphaTestRef = ref;
        TbFloat32 refF32 = static_cast<TbFloat32>(m_AlphaTestState.alphaTestRef) / 255.0f;
        glAlphaFunc( funcGL , refF32 );
        return TB_S_OK;
    }
#else
    // gl関数を使えない場合は、シェーダ側で条件とdiscard命令を使って対応
    TB_ASSERT(!"Unsupported");
#endif
    return TB_E_FAIL;
}

/*!
 * 深度テストを有効にする
 * @author teco
 */
TbResult TbRenderDevice::SetEnableDepthTest( TbBool isEnable )
{
    SET_ENABLE_FUNC_GL[ !!isEnable ]( GL_DEPTH_TEST );
    m_DepthStencilState.isEnableDepthTest = isEnable;
    return TB_S_OK;
}

/*!
 * 深度バッファの書き込みマスク設定
 * @author teco
 */
TbResult TbRenderDevice::SetDepthWriteMask( TbDepthMask mask )
{
    glDepthMask( mask == TB_DEPTH_MASK_ALL );
    m_DepthStencilState.depthWriteMask = mask;
    return TB_S_OK;
}

/*!
 * 深度比較タイプ
 * @author teco
 */
TbResult TbRenderDevice::SetDepthTestFunc( TbCmpFunc func )
{
    GLenum funcGL;
    if( TbCmpFuncToGLenum( &funcGL , func ) == TB_S_OK ){
        glDepthFunc( funcGL );
        m_DepthStencilState.depthTestFunc = func;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ステンシルテストを有効にする
 * @author teco
 */
TbResult TbRenderDevice::SetEnableStencilTest( TbBool isEnable )
{
    if( isEnable ){
        glEnable( GL_STENCIL_TEST );
    }else{
        glDisable( GL_STENCIL_TEST );
    }
    m_DepthStencilState.isEnableStencilTest = isEnable;
    return TB_S_OK;
}

/*!
 * ステンシルバッファの読み込みマスク設定
 * @author teco
 */
TbResult TbRenderDevice::SetStencilReadMask( TbUint8 mask )
{
    GLenum funcGL;
    if( TbCmpFuncToGLenum( &funcGL , m_DepthStencilState.stencilTestFunc ) == TB_S_OK ){
        m_DepthStencilState.stencilReadMask = mask;
        glStencilFunc( funcGL , m_DepthStencilState.stencilTestRef , m_DepthStencilState.stencilReadMask );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*! 
 * ステンシルバッファの書き込みマスク
 * @author teco
 */
TbResult TbRenderDevice::SetStencilWriteMask( TbUint8 mask )
{
    m_DepthStencilState.stencilWriteMask = mask;
    glStencilMask( m_DepthStencilState.stencilWriteMask );
    return TB_S_OK;
}

// 
/*!
 * ステンシルテストの処理
 * @param failOp 失敗時の処理
 * @param zfailOp ステンシルテストに合格してZテストに失敗した時の処理
 * @param passOp 合格時の処理
 * @author teco
 */
TbResult TbRenderDevice::SetStencilOp( TbStencilOp failOp , TbStencilOp zfailOp , TbStencilOp passOp )
{
    // GLは両面同じ設定
    GLenum failOpGL;
    GLenum zfailOpGL;
    GLenum passOpGL;
//    SYS_ASSERT( face == TB_RENDER_FACE_FRONT , "front only ( front = both)" );
    if( TbStencilOpToGLenum( &failOpGL , failOp ) == TB_S_OK && 
        TbStencilOpToGLenum( &zfailOpGL , zfailOp ) == TB_S_OK && 
        TbStencilOpToGLenum( &passOpGL , passOp ) == TB_S_OK )
    {
        glStencilOp( failOpGL , zfailOpGL , passOpGL );
        m_DepthStencilState.stencilOp.failOp = failOp;
        m_DepthStencilState.stencilOp.zfailOp = zfailOp;
        m_DepthStencilState.stencilOp.passOp = passOp;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ステンシル比較タイプ
 * @author teco
 */
TbResult TbRenderDevice::SetStencilTestFunc( TbCmpFunc func )
{
    GLenum funcGL;
    if( TbCmpFuncToGLenum( &funcGL , func ) == TB_S_OK ){
        glStencilFunc( funcGL , m_DepthStencilState.stencilTestRef , m_DepthStencilState.stencilReadMask );
        m_DepthStencilState.stencilTestFunc = func;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ステンシル参照値設定
 * @author teco
 */
TbResult TbRenderDevice::SetStencilTestRef( TbUint8 ref )
{
    GLenum funcGL;
    if( TbCmpFuncToGLenum( &funcGL , m_DepthStencilState.stencilTestFunc ) == TB_S_OK ){
        m_DepthStencilState.stencilTestRef = ref;
        glStencilFunc( funcGL , m_DepthStencilState.stencilTestRef , m_DepthStencilState.stencilReadMask );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 固定機能シェーダ
 * @author teco
 */
TbBool TbRenderDevice::isActiveFixedShader() const
{
    const TbShaderProgram* program = m_ShaderManager.GetCurrentProgram();
#if TB_IS_ENABLE_FIXED_SHADER
    return !program;
#else
    return !program || program == m_ShaderManager.GetDepend().GetSimulatedFixedShader();
#endif
}

/*!
 * 状態をダンプ
 * @author teco
 */
void TbRenderDevice::Dump()
{
}

}

#endif