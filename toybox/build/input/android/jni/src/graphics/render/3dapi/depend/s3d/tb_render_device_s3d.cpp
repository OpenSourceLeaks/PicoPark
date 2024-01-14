/*!
 * 描画デバイスの依存コード(Stage3D)
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_render_device.h"

#if TB_3DAPI_IS_S3D

#include "base/util/tb_depend_cast.h"
#include "graphics/3dapi/tb_3dapi_util.h"
#include "graphics/3dapi/tb_render_view.h"
#include "graphics/3dapi/tb_depth_stencil.h"
#include "graphics/3dapi/tb_vertex_buffer.h"
#include "graphics/3dapi/tb_index_buffer.h"
#include "graphics/3dapi/tb_depth_stencil.h"
#include "graphics/3dapi/tb_texture.h"
#include "graphics/3dapi/tb_texture_sampler.h"
#include "graphics/3dapi/depend/s3d/tb_render_util_s3d.h"
#include "graphics/display/tb_display_manager.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

namespace
{
    // ビューポート更新
    void updateViewport( TbRenderDevice* device , TbRenderTarget* renderTarget )
    {
#if 0
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
                    // ビューポート自動切換え(ウィンドウのサイズに合わせる)
                    device->SetViewport( 0 , 0 , renderTarget->GetDepend().view->GetWindow()->GetWidth() , 
                                                 renderTarget->GetDepend().view->GetWindow()->GetHeight() );
                }
                break;
            }
        }
#endif
    }
    
    /*!
     * インデックスバッファに書き込むための配列作成
     * @author Miyake Shunsuke
     */
    TbBool createIndexArray( TbUint16 dst[] , TbPrimitiveType primitiveType , TbUint32 startVertex , TbUint32  primitiveCount )
    {
        TbUint32 count = primitiveCount * 3;
        if( TB_PT_TRIANGLESTRIP == primitiveType )
        {
            TbUint32 vertexIndex = startVertex;
            TbUint32 index = 0;
            for( TbUint32 primitveIndex = 0; primitveIndex < primitiveCount; primitveIndex+=2 , vertexIndex+=2 ) {
                dst[index++] = vertexIndex+0;
                dst[index++] = vertexIndex+1;
                dst[index++] = vertexIndex+2;
                if( primitveIndex+1 < primitiveCount ) {
                    dst[index++] = vertexIndex+3;
                    dst[index++] = vertexIndex+2;
                    dst[index++] = vertexIndex+1;
                }
            }
        }
        else if( TB_PT_TRIANGLELIST == primitiveType )
        {
            for( TbUint32 i = 0; i < count; ++i ) {
                dst[i] = startVertex + i;
            }
        }
        else
        {
            // 上記のタイプ以外は未サポート
            return TB_FALSE;
        }
        return TB_TRUE;
    }
    
    const TbSizeT PRE_INDEX_BUFFER_SIZE = 4*1024; // とりあえず4KB分確保しておく
}

// 依存コード

AS3::ui::flash::display::Stage3D     TbRenderDeviceDepend::s_Stage3d;
AS3::ui::flash::display3D::Context3D TbRenderDeviceDepend::s_Context; 

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.07.13
 */
TB_INLINE TbRenderDeviceDepend::TbRenderDeviceDepend()
    : m_BitArray()
    , m_IndexBuffer(NULL)
{
    TbMemClear( m_VertexStream , sizeof(m_VertexStream) );
}

/*!
 * 頂点ストリーム設定
 * @param stream ストリーム番号
 * @param offset オフセット
 * @param stride ストライド
 * @author Miyake Shunsuke
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
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.05.10
 */
TbRenderDevice::TbRenderDevice( TbSizeT viewMax )
    : m_Window( NULL )
    , m_AdapterIndex(0)
    , m_IndexBuffer( NULL )
    , m_FixedShader()
    , m_ViewList( viewMax )
    , m_ActiveTextureCount( 0U )
    , m_DepthStencil( NULL )
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
 * @author Miyake Shunsuke
 * @since 2010.05.10
 */
TbRenderDevice::~TbRenderDevice()
{
    TB_SAFE_DELETE(m_IndexBuffer);
}

/*!
 * セットアップ
 * @param window 対応ウィンドウ
 * @param parameter パラメータ
 * @author Miyake Shunsuke
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
        
        // Contextの設定
        {
            TbUint32 width = m_Window->GetWidth();
            TbUint32 height = m_Window->GetHeight();
            if( parameter.backBufferWidth > 0 ) {
                width = parameter.backBufferWidth;
            }
            if( parameter.backBufferHeight > 0 ) {
                height = parameter.backBufferHeight;
            }
            m_Depend.GetContext()->configureBackBuffer(width, height, 2,
                                           parameter.enableCreateDepthStencil , false);
        }
        
        // インデックスバッファを確保しておく
        m_Depend.m_IndexBuffer = TB_NEW TbIndexBuffer(TB_INDEX_BUFFER_FORMAT_U16,PRE_INDEX_BUFFER_SIZE);

        // フルスクリーン機能は未サポート
//        m_BitArray.SetBit( FLAG_FULLSCREEN , TbDisplayManager::GetInstance().IsFullscreen( m_AdapterIndex ) );
        m_BitArray.SetBit( FLAG_FULLSCREEN , TB_FALSE );
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

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbBool TbRenderDevice::GetCurrentDisplayMode( TbUint32 index , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetCurrentDisplayMode( index , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbBool TbRenderDevice::GetDisplayMode( TbUint32 index , TbSint32 mode , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetDisplayMode( index , mode , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbBool TbRenderDevice::GetDisplayMode( TbUint32 index , TbSint32 mode , TbColorFormat format , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetDisplayMode( index , mode , format , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbSint32 TbRenderDevice::GetDisplayModeNum( TbUint32 index ) const
{
    return TbDisplayManager::GetInstance().GetDisplayModeNum( index );
}

/*!
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbSint32 TbRenderDevice::GetDisplayModeNum( TbUint32 index , TbColorFormat format ) const
{
    return TbDisplayManager::GetInstance().GetDisplayModeNum( index , format );
}


/*!
 * ビュー生成
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */
TbRenderView* TbRenderDevice::CreateView( const TbRenderViewParameter& parameter )
{
    if( !m_ViewList.IsEmpty() ){
        return NULL;
    }
    TbRenderView* result = NULL;
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
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult TbRenderDevice::SetViewport( TbUint32 x , TbUint32 y , TbUint32 width , TbUint32 height )
{
    
    return TB_S_OK;
}

/*!
 * ビューポート取得
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult TbRenderDevice::GetViewport( TbRectS32& rect ) const
{
    return TB_E_FAIL;
}

/*!
 * レンダーターゲット設定
 * @author Miyake Shunsuke
 * @since 2013.04.07
 */
TbResult TbRenderDevice::SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget )
{
    if( index > 0 ){
        // MRTは非対応
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
            TB_ASSERT(!"Unsupported");
            break;
        case TB_RENDER_TARGET_TYPE_VIEW: // ビュー
            {
                m_Depend.GetContext()->setRenderToBackBuffer();
                TbRenderView* view = renderTarget->GetDepend().view;
                // 深度ステンシル自動切換え
                m_DepthStencil = view->GetDepthStencil();
            }
            break;
        case TB_RENDER_TARGET_TYPE_TEXTURE: // テクスチャ
            {
//                m_Depend.m_BitArray.SetBit(FLAG_REQUEST_SET_RENDER_TEXTURE); // 遅延させる？
                // 必要になった時に設定する
                m_Depend.GetContext()->setRenderToTexture( renderTarget->GetDepend().texture->GetDepend().GetTexture() ,
                                                           m_DepthStencil && m_DepthStencil->GetType() == TB_DEPTH_STENCIL_TYPE_TEXTURE
                                                         );
            }
            break;
        }
    }
    m_RenderTarget[index] = renderTarget;
    // 0番のレンダーターゲットが更新された場合、ビューポートも更新する
    updateViewport( this , m_RenderTarget[index] );
    return TB_S_OK;
}

/*!
 * 深度ステンシル設定
 * @author Miyake Shunsuke
 * @since 2011.05.27
 */
TbResult TbRenderDevice::SetDepthStencil( TbDepthStencil* depthStencil )
{
    if( m_DepthStencil == depthStencil ){
        return TB_S_OK;
    }

    if( depthStencil ){
        switch( depthStencil->GetType() )
        {
        case TB_DEPTH_STENCIL_TYPE_BUFFER: // バッファ
            break;
        case TB_DEPTH_STENCIL_TYPE_TEXTURE: // テクスチャ
            break;
        case TB_DEPTH_STENCIL_TYPE_VIEW: // 標準
            break;
        }
    }
    m_DepthStencil = depthStencil;
    return TB_S_OK;
}

/*!
 * シーン開始
 * @author Miyake Shunsuke
 * @since 2010.05.10
 */
void TbRenderDevice::BeginScene()
{
    m_Depend.m_IndexBufferIndex = 0; // リセット
    
    // ビューポートのサイズを現在のレンダーターゲットに合わせる
    updateViewport( this , m_RenderTarget[0] );
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2011.05.26
 */
TbResult TbRenderDevice::Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil )
{
    TbUint32 mask = 0;
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_COLOR_BIT ){
        mask |= Context3DClearMask::COLOR;
    }
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_DEPTH_BIT ){
        // ここがTRUEになっていないとクリアできない
        mask |= Context3DClearMask::DEPTH;
    }
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_STENCIL_BIT ){
        mask |= Context3DClearMask::STENCIL;
    }
    m_Depend.GetContext()->clear( color.r , color.g , color.b , color.a , depth , stencil , mask );
    return TB_S_OK;
}

/*!
 * カラーバッファに書き込むマスク設定
 * @param colorMaskBit TbColorWriteMaskBitの論理和
 * @author Miyake Shunsuke
 * @since 2011.06.09
 */
TbResult TbRenderDevice::SetColorWriteMask( TbUint32 colorMaskBit )
{
    m_Depend.GetContext()->setColorMask((colorMaskBit&TB_COLOR_MASK_BIT_RED)>0,
                                        (colorMaskBit&TB_COLOR_MASK_BIT_GREEN)>0,
                                        (colorMaskBit&TB_COLOR_MASK_BIT_BLUE)>0,
                                        (colorMaskBit&TB_COLOR_MASK_BIT_ALPHA)>0);
    return TB_S_OK;
}

/*!
 * バッファに書き込むかどうかの設定取得
 * @return TbColorWriteMaskBitの論理和
 * @author Miyake Shunsuke
 * @since 2011.06.09
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
 * @author Miyake Shunsuke
 * @since 2010.05.10
 */
void TbRenderDevice::SetVertexBuffer( TbUint32 streamNumber , TbVertexBuffer* vertexBuffer , TbUint32 offsetInBytes , TbUint32 stride )
{
    // フォーマット情報と一緒に設定する必要がある
    m_Depend.SetVertexStream( streamNumber , vertexBuffer , offsetInBytes , stride );
}

/*!
 * 頂点インデックスバッファ設定
 * @author Miyake Shunsuke
 * @since 2011.06.06
 */
void TbRenderDevice::SetIndexBuffer( TbIndexBuffer* indexBuffer )
{
    m_IndexBuffer = indexBuffer;
}

/*!
 * プリミティブ描画
 * @author Miyake Shunsuke
 * @since 2010.05.10
 */
TbResult TbRenderDevice::DrawPrimitive( const TbPrimitiveType primitiveType , TbUint32 startVertex , TbUint32 primitiveCount )
{
    static TbUint16 s_IndexArray[100000]; // 10万ポリゴンまで対応
    static AS3::ui::flash::display3D::IndexBuffer3D i3dbuffer;
    static AS3::ui::flash::display3D::IndexBuffer3D i3dbuffer2;
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    
    // 内部で予約しておいたインデックスバッファを利用して描画
    TB_ASSERT( primitiveType == TB_PT_TRIANGLELIST ); // トライアングルリスト以外認めない
    
    TbBool isActiveShader = !isActiveFixedShader();
    if( isActiveShader ){
        m_ShaderManager.GetDepend().EnableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().EnableVertexAttribute();
    }
    
    // 必要なインデックスバッファのインデックス数
    TbUint32 indexCount = primitiveCount * 3;
    {
        if( indexCount > TB_ARRAY_COUNT_OF(s_IndexArray) ) {
            // ポリゴン数多すぎ
            return TB_E_FAIL;
        }
        if(!createIndexArray( s_IndexArray , primitiveType , startVertex , primitiveCount )){
            // インデックス配列生成失敗
            return TB_E_FAIL;
        }
        // インデックスバッファ書き込み(四倍しているのは16bitサイズ分)
        m_Depend.m_IndexBuffer->Write(m_Depend.m_IndexBufferIndex*2,indexCount*2,s_IndexArray);
    }

    // 描画
    context->drawTriangles( m_Depend.m_IndexBuffer->GetDepend().indexBuffer , m_Depend.m_IndexBufferIndex , primitiveCount );    
    m_Depend.m_IndexBufferIndex += indexCount;
    return TB_S_OK;
}

/*!
 * プリミティブ描画( 生の頂点配列使用 )
 * @author Miyake Shunsuke
 * @since 2011.03.19
 */
TbResult TbRenderDevice::DrawPrimitive( const TbPrimitiveType primitiveType , const TbUint32 primitiveCount , const void* vertexs , TbUint32 stride )
{
    return TB_E_FAIL;
}

/*!
 * 頂点インデックスを利用したプリミティブ描画
 * @author Miyake Shunsuke
 * @since 2011.06.06
 */
TbResult TbRenderDevice::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                           const TbUint32 minVertexIndex ,
                                           const TbUint32 vertexCount , 
                                           const TbUint32 startIndex , 
                                           const TbUint32 primitiveCount )
{
    if( !m_IndexBuffer ){
        return TB_E_FAIL;
    }
    if( TB_S_OK != TbIndexBufferDepend::IsSupportedFormat( m_IndexBuffer->GetFormat() ) ) {
        return TB_E_FAIL;
    }
    TB_UNUSE( minVertexIndex );
    TB_UNUSE( vertexCount );
    TB_ASSERT( primitiveType == TB_PT_TRIANGLELIST ); // トライアングルリスト以外認めない
    
    TbBool isActiveShader = !isActiveFixedShader();
    if( isActiveShader ){
        m_ShaderManager.GetDepend().EnableVertexAttribute();
    }else{
        m_FixedShader.GetDepend().EnableVertexAttribute();
    }

    // 描画
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    context->drawTriangles( m_IndexBuffer->GetDepend().indexBuffer , startIndex , primitiveCount );

    return TB_S_OK;
}

/*!
 * 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
 * @author Miyake Shunsuke
 * @since 2011.06.06
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
    // 生データは未サポート
    return TB_E_FAIL;

}

/*!
 * シーン終了
 * @author Miyake Shunsuke
 * @since 2010.07.13
 */
void TbRenderDevice::EndScene()
{
}

/*!
 * 画面に表示
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */
void TbRenderDevice::Present()
{
    TbRenderView* view = m_ViewList.GetAt(0);
    if( TB_VERIFY(view) ){
        view->Present();
    }
}

/*!
 * テクスチャ設定
 * @author Miyake Shunsuke
 * @since 2010.09.29
 */
TbBool TbRenderDevice::SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType )
{
    if( TB_SHADER_TYPE_PIXEL == shaderType && 0 <= stage && stage < TEXTURE_STAGE_MAX )
    {
        AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
        // 割り当てテクスチャ数増減
        if( texture && !m_Texture[ stage ] ){
            context->setTextureAt( stage , texture->GetDepend().GetTexture() );
            ++m_ActiveTextureCount;
        }else if( !texture && m_Texture[ stage ] ){
            --m_ActiveTextureCount;
        }
        // 保持しておく
        m_Texture[ stage ] = texture;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 現在利用されているテクスチャを取得
 * @author Miyake Shunsuke
 * @since 2010.10.17
 */
TbTexture*  TbRenderDevice::GetTexture( TbSint32 stage , TbShaderType shaderType )
{
    TbTexture* result = NULL;
    if( TB_SHADER_TYPE_PIXEL == shaderType && 0 <= stage && stage < TEXTURE_STAGE_MAX )
    {
        result = m_Texture[ stage ];
    }
    return result;    
}

/*!
 * 表面が時計周りかどうか
 * @author Miyake Shunsuke
 * @since 2011.03.25
 */
TbResult TbRenderDevice::SetFrontFaceClockWise( TbBool isFrontFaceClockWise )
{
    return TB_E_FAIL;
}

/*!
 * カリング設定
 * @author Miyake Shunsuke
 * @since 2011.03.24
 */
TbResult TbRenderDevice::SetCullMode( TbCullMode cullMode )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    switch( cullMode ){
    case TB_CULL_MODE_NONE:
        context->setCulling(Context3DTriangleFace::NONE);
        break;
    case TB_CULL_MODE_FRONT:        
        context->setCulling(Context3DTriangleFace::FRONT);
        break;
    case TB_CULL_MODE_BACK:
        context->setCulling(Context3DTriangleFace::BACK);
        break;
    default:
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * フィルモード設定
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbRenderDevice::SetFillMode( TbFillMode fillMode )
{
    // TB_FILL_MODE_SOLID固定
    if( fillMode != TB_FILL_MODE_SOLID){
        TB_ASSERT(!"Unsupported");
    }
}

/*!
 * テクスチャサンプラーセット
 * @author Miyake Shunsuke
 * @since 2011.04.03
 */
TbResult TbRenderDevice::SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType )
{
    // 引数不正チェック
    if( slot >= TEXTURE_SAMPLER_SLOT_MAX || 
        TB_SHADER_TYPE_PIXEL != shaderType )
    {
        return TB_E_FAIL;
    }
    // NULLの場合はここで終了
    if( !sampler ){
        m_TextureSampler[ slot ] = NULL;
        return TB_S_OK;
    }
    m_TextureSampler[ slot ] = sampler;
    
#if 0 // setSamplerStateAがサポートさてていない
    flash::display3D::Context3D& context = m_Depend.GetContext();
#endif
    return TB_S_OK;
}

/*!
 * テクスチャサンプラー取得
 * @author Miyake Shunsuke
 * @since 2011.04.03
 */
TbTextureSampler* TbRenderDevice::GetTextureSampler( TbUint32 slot , TbShaderType shaderType )
{
    // 引数不正チェック
    if( slot >= TEXTURE_SAMPLER_SLOT_MAX || 
        TB_SHADER_TYPE_PIXEL != shaderType )
    {
        return NULL;
    }
    return m_TextureSampler[ slot ];
}

/*!
 * ブレンド有効設定
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbRenderDevice::SetEnableBlend( TbBool isEnable )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    // 非有効にするやり方が見当たらず。。。
    m_BlendState.isEnableBlend = isEnable;
    return TB_S_OK;
}

/*!
 * ブレンド比較タイプ
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbRenderDevice::SetBlendOp( TbBlendOp op )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    if( TB_VERIFY(TB_BLEND_OP_ADD == op) ) // TB_BLEND_OP_ADD以外未サポート
    {
        m_BlendState.blendOp = op;
        return TB_S_OK;    
    }
    return TB_E_FAIL;
 }

/*!
 * ブレンド係数
 * @author Miyake Shunsuke
 * @since 2013.04.11
 */
TbResult TbRenderDevice::SetBlendFactor( TbBlendFactor src , TbBlendFactor dst )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    AS3::ui::String srcFactorS3D;
    AS3::ui::String dstFactorS3D;
    if( TbBlendFactorToAS3String( srcFactorS3D , src ) && 
        TbBlendFactorToAS3String( dstFactorS3D , dst ) )
    {
        context->setBlendFactors( srcFactorS3D , dstFactorS3D );
        m_BlendState.blendSrcFactor = src;
        m_BlendState.blendDstFactor = dst;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * アルファテスト有効設定
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbRenderDevice::SetEnableAlphaTest( TbBool isEnable )
{
    // 未サポート
    return TB_E_FAIL;
}

/*!
 * アルファ比較関数
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbRenderDevice::SetAlphaTestFunc( TbCmpFunc func )
{
    // 未サポート
    return TB_E_FAIL;
}

/*!
 * アルファ比較関数
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbRenderDevice::SetAlphaTestRef( TbUint8 ref )
{
    // 未サポート
    return TB_E_FAIL;
}

/*!
 * 深度テストを有効にする
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult TbRenderDevice::SetEnableDepthTest( TbBool isEnable )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    // 非有効にするやり方が見当たらず。。。
    m_DepthStencilState.isEnableDepthTest = isEnable;
    return TB_S_OK;
}

/*!
 * 深度バッファの書き込みマスク設定
 * @author Miyake Shunsuke
 * @since 2011.06.09
 */
TbResult TbRenderDevice::SetDepthWriteMask( TbDepthMask mask )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    m_DepthStencilState.depthWriteMask = mask;
    AS3::ui::String funcS3D;
    TB_VERIFY(TbCmpFuncToAS3String( funcS3D , m_DepthStencilState.depthTestFunc ));
    context->setDepthTest( m_DepthStencilState.depthWriteMask == TB_DEPTH_MASK_ALL , funcS3D );
    return TB_S_OK;
}

/*!
 * 深度比較タイプ
 * @author Miyake Shunsuke
 * @since 2011.06.09
 */
TbResult TbRenderDevice::SetDepthTestFunc( TbCmpFunc func )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    AS3::ui::String funcS3D;
    if( TbCmpFuncToAS3String( funcS3D , func ) ){
        m_DepthStencilState.depthTestFunc = func;
        context->setDepthTest( m_DepthStencilState.depthWriteMask == TB_DEPTH_MASK_ALL , funcS3D );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ステンシルテストを有効にする
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
TbResult TbRenderDevice::SetEnableStencilTest( TbBool isEnable )
{
    // 非有効にするやり方が見当たらず。。。
    m_DepthStencilState.isEnableStencilTest = isEnable;
    return TB_S_OK;
}

/*!
 * ステンシルバッファの読み込みマスク設定
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */
TbResult TbRenderDevice::SetStencilReadMask( TbUint8 mask )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();    
    m_DepthStencilState.stencilReadMask = mask;
    context->setStencilReferenceValue( m_DepthStencilState.stencilTestRef , 
                                       m_DepthStencilState.stencilReadMask ,
                                       m_DepthStencilState.stencilWriteMask );
    return TB_S_OK;
}

/*! 
 * ステンシルバッファの書き込みマスク
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */
TbResult TbRenderDevice::SetStencilWriteMask( TbUint8 mask )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();    
    m_DepthStencilState.stencilWriteMask = mask;
    context->setStencilReferenceValue( m_DepthStencilState.stencilTestRef , 
                                       m_DepthStencilState.stencilReadMask ,
                                       m_DepthStencilState.stencilWriteMask );
    return TB_S_OK;
}

// 
/*!
 * ステンシルテストの処理
 * @param failOp 失敗時の処理
 * @param zfailOp ステンシルテストに合格してZテストに失敗した時の処理
 * @param passOp 合格時の処理
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */
TbResult TbRenderDevice::SetStencilOp( TbStencilOp failOp , TbStencilOp zfailOp , TbStencilOp passOp )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();    
    AS3::ui::String failOpS3D;
    AS3::ui::String zfailOpS3D;
    AS3::ui::String passOpS3D;
    if( TbStencilOpToAS3String( failOpS3D , failOp ) && 
        TbStencilOpToAS3String( zfailOpS3D , zfailOp ) && 
        TbStencilOpToAS3String( passOpS3D, passOp ) )
    {
        m_DepthStencilState.stencilOp.failOp = failOp;
        m_DepthStencilState.stencilOp.zfailOp = zfailOp;
        m_DepthStencilState.stencilOp.passOp = passOp;
        
        AS3::ui::String funcS3D;
        TB_VERIFY(TbCmpFuncToAS3String( funcS3D , m_DepthStencilState.depthTestFunc ));
        context->setStencilActions( Context3DTriangleFace::FRONT_AND_BACK , 
                                    funcS3D , 
                                    passOpS3D , 
                                    zfailOpS3D , 
                                    failOpS3D );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ステンシル比較タイプ
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */
TbResult TbRenderDevice::SetStencilTestFunc( TbCmpFunc func )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();    
    AS3::ui::String funcS3D;
    if( TbCmpFuncToAS3String( funcS3D , func ) ){
        m_DepthStencilState.stencilTestFunc = func;
        AS3::ui::String failOpS3D;
        AS3::ui::String zfailOpS3D;
        AS3::ui::String passOpS3D;
        TbStencilOpToAS3String( failOpS3D , m_DepthStencilState.stencilOp.failOp );
        TbStencilOpToAS3String( zfailOpS3D , m_DepthStencilState.stencilOp.zfailOp );
        TbStencilOpToAS3String( passOpS3D, m_DepthStencilState.stencilOp.passOp );
        context->setStencilActions( Context3DTriangleFace::FRONT_AND_BACK , 
                                    funcS3D , 
                                    passOpS3D , 
                                    zfailOpS3D , 
                                    failOpS3D );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ステンシル参照値設定
 * @author Miyake Shunsuke
 * @since 2011.06.09
 */
TbResult TbRenderDevice::SetStencilTestRef( TbUint8 ref )
{
    AS3::ui::flash::display3D::Context3D& context = m_Depend.GetContext();
    m_DepthStencilState.stencilTestRef = ref;
    context->setStencilReferenceValue( m_DepthStencilState.stencilTestRef , 
                                       m_DepthStencilState.stencilReadMask ,
                                       m_DepthStencilState.stencilWriteMask );
    return TB_S_OK;
}

/*!
 * 固定機能シェーダ
 * @author Miyake Shunsuke
 * @since 2013.03.13
 */
TbBool TbRenderDevice::isActiveFixedShader() const
{
    const TbShaderProgram* program = m_ShaderManager.GetCurrentProgram();
    return !program || program == m_ShaderManager.GetDepend().GetSimulatedFixedShader();
}

/*!
 * 状態をダンプ
 * @author Miyake Shunsuke
 * @since 2013.03.14
 */
void TbRenderDevice::Dump()
{
}

}

#endif