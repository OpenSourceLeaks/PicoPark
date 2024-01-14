/*!
 * デバイス実装ファイル
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_device.h"

#if TB_3DAPI_IS_GFX

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
#include "graphics/util/tb_gr_util.h"

namespace
{
    size_t COMMAND_MEMORY_SIZE = 1024 * 1024; // 1MiB
    size_t CONTROL_MEMORY_SIZE = 1024 * 1024; // 1MiB

    static const nn::gfx::PrimitiveTopology PRIM_TYPE[toybox::TB_PT_MAX] =
    {
        nn::gfx::PrimitiveTopology_PointList ,
        nn::gfx::PrimitiveTopology_LineList ,
        nn::gfx::PrimitiveTopology_LineStrip ,
        nn::gfx::PrimitiveTopology_TriangleList ,
        nn::gfx::PrimitiveTopology_TriangleStrip ,
        nn::gfx::PrimitiveTopology_End , // Fan が見当たらない
    };

    void outOfCommandMemoryEventCallback(
        nn::gfx::CommandBuffer* commandBuffer, const nn::gfx::OutOfMemoryEventArg& arg)
    {
        // arg.minRequiredSize
        // commandBuffer->AddCommandMemory();
        TB_ASSERT(0);
    }

    void outOfControlMemoryEventCallback(
        nn::gfx::CommandBuffer* commandBuffer, const nn::gfx::OutOfMemoryEventArg& arg)
    {
        // arg.minRequiredSize
        // commandBuffer->AddControlMemory();
        TB_ASSERT(0);
    }

}

namespace toybox
{

// GFX用デバイス取得
nn::gfx::Device* TbGetGfxDevice() {
    return &TbRenderDevice::GetInstance().GetDepend().GetDevice();
}

// コマンドバッファ取得
nn::gfx::CommandBuffer* TbGetGfxCommandBuffer() {
    return &TbRenderDevice::GetInstance().GetDepend().GetCommandBuffer();
}

// 依存コード

/*!
 * コンストラクタ
 * @author teco
 */
TB_INLINE TbRenderDeviceDepend::TbRenderDeviceDepend()
    : m_BitArray()
    , m_ControlMemory(nullptr)
    , m_IsRecordingCommand(TB_FALSE)
    , m_IsEnableCommandCache(TB_TRUE)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderDeviceDepend::~TbRenderDeviceDepend()
{
    TB_ASSERT(!m_IsRecordingCommand);
    m_Fence.Finalize(&m_Device);
    m_CommandBuffer.Finalize(&m_Device);
    m_CommandMemory.Finalize();
    m_Queue.Finalize(&m_Device);
    m_Device.Finalize();
    TB_SAFE_DELETE_ARRAY(m_ControlMemory);
}

/*!
 * セットアップ
 * @param view ベースビュー
 * @author teco
 */
TbBool TbRenderDeviceDepend::Initialize()
{
    // デバイス
    {
        nn::gfx::Device::InfoType info;
        info.SetDefault();
#if TB_IS_DEBUG
        info.SetDebugMode(nn::gfx::DebugMode_Enable);
#endif
        info.SetApiVersion(nn::gfx::ApiMajorVersion, nn::gfx::ApiMinorVersion);
        m_Device.Initialize(info);
    }

    // キュー
    {
        nn::gfx::Queue::InfoType info;
        info.SetDefault();
        info.SetCapability(nn::gfx::QueueCapability_Graphics);
        m_Queue.Initialize(&m_Device, info);
    }

    // コマンドバッファ
    {
        nn::gfx::CommandBuffer::InfoType info;
        info.SetDefault();
        info.SetQueueCapability(nn::gfx::QueueCapability_Graphics);
        info.SetCommandBufferType(nn::gfx::CommandBufferType_Direct);
        m_CommandBuffer.Initialize(&m_Device, info);

        size_t commandMemoryAlingn = nn::gfx::CommandBuffer::GetCommandMemoryAlignment(&m_Device);
        m_CommandMemory.Initialize(nn::gfx::MemoryPoolProperty_CpuUncached | nn::gfx::MemoryPoolProperty_GpuCached,
            COMMAND_MEMORY_SIZE,
            commandMemoryAlingn);

        size_t controlMemoryAlign = nn::gfx::CommandBuffer::GetControlMemoryAlignment(&m_Device);
        m_ControlMemory = new TbUint8[CONTROL_MEMORY_SIZE+ controlMemoryAlign];
        void* controlMemoryAligned = nn::util::BytePtr(m_ControlMemory).AlignUp(controlMemoryAlign).Get();

        m_CommandBuffer.Reset();
        m_CommandBuffer.AddCommandMemory(m_CommandMemory.Get(), 0,COMMAND_MEMORY_SIZE);
        m_CommandBuffer.AddControlMemory(controlMemoryAligned,CONTROL_MEMORY_SIZE);

    }
        nn::gfx::Fence::InfoType info;
        info.SetDefault();
        m_Fence.Initialize(&m_Device, info);
    {

    }

    return TB_TRUE;
}

/*!
* コマンドバッファ実行
* @author teco
*/
void TbRenderDeviceDepend::executeCommandBuffer()
{
    TB_RESQUE(m_IsRecordingCommand);
    m_CommandBuffer.End();
    m_Queue.ExecuteCommand(&m_CommandBuffer,&m_Fence);
    m_Queue.Flush();
    m_Fence.Sync(nn::TimeSpan::FromSeconds(1));
}

/*!
 * コマンドバッファ実行
 * @author teco
 */
void TbRenderDeviceDepend::beginCommandBuffer(TbRenderDevice* common)
{
    m_IsRecordingCommand = TB_TRUE;
    m_CommandBuffer.Begin();

    m_IsEnableCommandCache = TB_FALSE;
    m_CommandBuffer.InvalidateMemory(nn::gfx::GpuAccess_ShaderCode |
        nn::gfx::GpuAccess_Descriptor |
        nn::gfx::GpuAccess_Texture |
        nn::gfx::GpuAccess_IndexBuffer |
        nn::gfx::GpuAccess_ConstantBuffer |
        nn::gfx::GpuAccess_VertexBuffer);

    if (common->m_DepthStencil) {
        TbRenderTarget* rt = common->GetRenderTarget(0);
        common->SetRenderTarget(0, rt);
    }
    common->GetShaderManager().SetProgram(common->GetShaderManager().GetCurrentProgram());
    common->SetBlendState(common->m_BlendState);
    common->SetRasterizerState(common->m_RasterizerState);
    common->SetDepthStencilState(common->m_DepthStencilState);
    common->SetViewportScissorState(common->m_ViewportSciessorState);
    m_IsEnableCommandCache = TB_TRUE;
}

/*!
 * コンストラクタ
 * @author teco
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
    , m_ViewportSciessorState( nullptr )
    , m_RasterizerState( nullptr )
    , m_BlendState(nullptr)
    , m_DepthStencilState(nullptr)
    , m_Depend()
{
    TbMemClear( m_Texture , sizeof(m_Texture) );
    TbMemClear( m_TextureSampler , sizeof( m_TextureSampler ) );
    TbMemClear( m_RenderTarget , sizeof(m_RenderTarget) );}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderDevice::~TbRenderDevice()
{
    m_ShaderManager.Finalize();
    m_RenderStatePreset.Finalize();
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
 */
TbBool TbRenderDevice::Initialize( TbUint32 adapterIndex , TbWindow* focusWindow , TbUint32 flags , const TbRenderViewParameter& parameter )
{
    if( focusWindow )
    {
        m_Depend.Initialize();
        m_ShaderManager.Initialize();
        m_AdapterIndex = adapterIndex;
        m_Window = focusWindow;
        TbRenderViewParameter localParam;
        TbMemCopy( &localParam , &parameter , sizeof(localParam) );
        localParam.window = m_Window;
        if (0 == localParam.backBufferWidth) {
            localParam.backBufferWidth = m_Window->GetWidth();
        }
        if (0 == localParam.backBufferHeight) {
            localParam.backBufferHeight = m_Window->GetHeight();
        }
        TbRenderView* view = CreateView( localParam );
        m_RenderTarget[0] = view->GetRenderTarget(0); // デフォルトレンダーターゲット
        m_DepthStencil = view->GetDepthStencil();
        initCommon();
        initDefaultState(localParam);
        m_BitArray.SetBit(FLAG_INITIALIZE);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 性能チェック
 * @author Miyake_Shunsuke
 */
void TbRenderDevice::GetCaps( Caps& caps )
{
    // 未実装
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
 */
TbBool TbRenderDevice::GetCurrentDisplayMode( TbUint32 index , TbDisplayMode& outDisplayMode ) const
{
    return TbDisplayManager::GetInstance().GetCurrentDisplayMode( index , &outDisplayMode );
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
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
TbResult TbRenderDevice::SetViewportScissorState(const TbViewportScissorState* state)
{
    if (state) {
        if (state != m_ViewportSciessorState || !m_Depend.m_IsEnableCommandCache) {
            if(m_Depend.m_IsRecordingCommand)
            {
                const nn::gfx::ViewportScissorState& nativeState = state->GetDepend().GetState();
                nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
                commandBuffer.SetViewportScissorState(&nativeState);
                m_ViewportSciessorState = state;
            }
        }
        return TB_S_OK;
    }
    return SetViewportScissorState(&m_DefaultViewportScissorState);
}

/*!
 * ビューポート取得
 * @author teco
 */
TbResult TbRenderDevice::GetViewport( TbRectS32* rect ) const
{
    if (m_ViewportSciessorState && rect) {
        const TbViewportScissorStateParam& param = m_ViewportSciessorState->GetParam();
        rect->x = param.viewPortRect.x;
        rect->y = param.viewPortRect.y;
        rect->width = param.viewPortRect.width;
        rect->height = param.viewPortRect.height;
        return TB_S_OK;
    }
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
    if( m_RenderTarget[index] == renderTarget && m_Depend.m_IsEnableCommandCache ){
        return TB_S_OK;
    }
    m_RenderTarget[index] = renderTarget;
    if (m_DepthStencil && m_RenderTarget[0] && m_Depend.m_IsRecordingCommand) {
        nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
        nn::gfx::DepthStencilView* depthStencilView = &m_DepthStencil->GetDepend().depthStencilView;
        nn::gfx::ColorTargetView* colorView = m_RenderTarget[0]->GetDepend().colorTargetView;
        commandBuffer.SetRenderTargets(1, &colorView, depthStencilView);
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

    if( depthStencil && m_RenderTarget[0] && m_Depend.m_IsRecordingCommand){
        nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
        nn::gfx::DepthStencilView* depthStencilView = &depthStencil->GetDepend().depthStencilView;
        nn::gfx::ColorTargetView* colorView = m_RenderTarget[0]->GetDepend().colorTargetView;
        commandBuffer.SetRenderTargets(1, &colorView, depthStencilView);
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
    nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
    {
        size_t controlMemoryAlign = nn::gfx::CommandBuffer::GetControlMemoryAlignment(&m_Depend.m_Device);
        void* controlMemoryAligned = nn::util::BytePtr(m_Depend.m_ControlMemory).AlignUp(controlMemoryAlign).Get();
        commandBuffer.Reset();
        commandBuffer.AddCommandMemory(m_Depend.m_CommandMemory.Get(), 0, COMMAND_MEMORY_SIZE);
        commandBuffer.AddControlMemory(controlMemoryAligned, CONTROL_MEMORY_SIZE);
    }

    if (m_DepthStencil) {
        m_ViewList[0]->GetDepend().Sync();
        TbRenderTarget* rt = m_ViewList[0]->GetRenderTarget(0);
        SetRenderTarget(0, rt);
    }
    m_ShaderManager.OnBeginScene();

    m_Depend.beginCommandBuffer(this);
}

/*!
 * クリア
 * @author teco
 */
TbResult TbRenderDevice::Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil )
{
    if( 0 == clearTargetBit ){
        return TB_S_OK;
    }
    TB_RESQUE_RET(m_Depend.m_IsRecordingCommand,TB_E_FAIL);

    nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_COLOR_BIT ){
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_RenderTarget); ++i) {
            if (m_RenderTarget[i]) {
                TbClearColor clearColor = TbGammaCorrection(color);
                nn::gfx::ColorTargetView* view = m_RenderTarget[i]->GetDepend().colorTargetView;
                commandBuffer.ClearColor(view, clearColor.r, clearColor.g, clearColor.b, clearColor.a, nullptr);
            }
        }
    }
    TbUint32 clearBit = 0;
    if (clearTargetBit & TB_RENDER_BUFFER_TYPE_DEPTH_BIT)
    {
        clearBit |= nn::gfx::DepthStencilClearMode_Depth;
    }
    if(clearTargetBit & TB_RENDER_BUFFER_TYPE_STENCIL_BIT )
    {
        clearBit |= nn::gfx::DepthStencilClearMode_Stencil;
    }
    if (0 < clearBit && m_DepthStencil ) {
        commandBuffer.ClearDepthStencil(&m_DepthStencil->GetDepend().depthStencilView,
                                        depth,stencil,static_cast<nn::gfx::DepthStencilClearMode>(clearBit), nullptr);
    }

    // Clearするとレンダーターゲットが無効化されるので再設定
    m_Depend.m_IsEnableCommandCache = TB_FALSE;
    SetRenderTarget(0,m_RenderTarget[0]);
    m_Depend.m_IsEnableCommandCache = TB_TRUE;

    return TB_S_OK;
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
    TB_RESQUE(m_Depend.m_IsRecordingCommand);
    nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
    if (vertexBuffer) {
        nn::gfx::GpuAddress gpuAddress;
        vertexBuffer->GetDepend().GetBuffer()->GetGpuAddress(&gpuAddress);
        gpuAddress.Offset(offsetInBytes);
        commandBuffer.SetVertexBuffer(streamNumber, gpuAddress, stride,vertexBuffer->GetSize());
    }
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
    TB_RESQUE_RET(m_Depend.m_IsRecordingCommand, TB_E_FAIL);
    nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
    commandBuffer.Draw( PRIM_TYPE[primitiveType] ,
                        TbGetPrimitiveVertexCount(primitiveType, primitiveCount),
                        startVertex ); 
    return TB_S_OK;
}

/*!
 * プリミティブ描画( 生の頂点配列使用 )
 * @author teco
 */
TbResult TbRenderDevice::DrawPrimitive( const TbPrimitiveType primitiveType , 
                                        const TbUint32 primitiveCount , 
                                        const void* vertexs , 
                                        TbUint32 stride )
{
    TB_RESQUE_RET(m_Depend.m_IsRecordingCommand, TB_E_FAIL);
    // 頂点バッファ、インデックスバッファの生成必須
//    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
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
    TB_RESQUE_RET(m_Depend.m_IsRecordingCommand,TB_E_FAIL);
    if( !m_IndexBuffer ){
        return TB_E_FAIL;
    }

    TbUint32 offset = 0U;
    if( TB_S_OK != TbIndexBuffer::GetByteCountFromFormat( offset , m_IndexBuffer->GetFormat() ) )
    {
        return TB_E_FAIL;
    }
    offset *= startIndex;
    TB_UNUSE( minVertexIndex );

    // 描画
    nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
    nn::gfx::GpuAddress gpuAddress;
    m_IndexBuffer->GetDepend().GetBuffer()->GetGpuAddress(&gpuAddress);
    gpuAddress.Offset(offset);
    commandBuffer.DrawIndexed(PRIM_TYPE[primitiveType],
                             m_IndexBuffer->GetDepend().GetFormat(),
                             gpuAddress,
                             vertexCount,
                             0);
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
    TB_RESQUE_RET(m_Depend.m_IsRecordingCommand, TB_E_FAIL);
    // 頂点バッファ、インデックスバッファの生成必須
    TB_ASSERT(!"unsupported");
    return TB_E_FAIL;
}

/*!
 * シーン終了
 * @author teco
 */
void TbRenderDevice::EndScene()
{
    m_Depend.executeCommandBuffer();
    m_Depend.m_IsRecordingCommand = TB_FALSE;
    m_ShaderManager.OnEndScene();
}

/*!
 * テクスチャ設定
 * @author teco
 */
TbBool TbRenderDevice::SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType )
{
    nn::gfx::TextureView* textrure = texture ? texture->GetDepend().GetTextureView() : nullptr;
    nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
    switch( shaderType )
    {
    case TB_SHADER_TYPE_VERTEX:
        break;
    case TB_SHADER_TYPE_PIXEL:
        if (m_Texture[stage] != texture ) {
            if (texture) {
                ++m_ActiveTextureCount;
            }else{
                --m_ActiveTextureCount;
                TB_ASSERT(0<= m_ActiveTextureCount);
            }
        }
        m_Texture[ stage ] = texture;
        if (m_Depend.m_IsRecordingCommand) {
            m_ShaderManager.GetDepend().updateSamplerVar(stage);
        }
        break;
    case TB_SHADER_TYPE_GEOMETRY:
        break;
    default:
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
    m_TextureSampler[slot] = sampler;
    if (m_Depend.m_IsRecordingCommand) {
        m_ShaderManager.GetDepend().updateSamplerVar(slot);
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
    if (!m_TextureSampler[slot]) {
        return &m_DefaultTextureSampler;
    }
    return m_TextureSampler[ slot ];
}

/*!
* ラスタライザステート設定
* @author teco
*/
TbResult TbRenderDevice::SetRasterizerState(const TbRasterizerState* state)
{
    if (state) {
        if (state != m_RasterizerState || !m_Depend.m_IsEnableCommandCache) {
            if (m_Depend.m_IsRecordingCommand) {
                const nn::gfx::RasterizerState& nativeState = state->GetDepend().GetState();
                nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
                commandBuffer.SetRasterizerState(&nativeState);
            }
            m_RasterizerState = state;
        }
        return TB_S_OK;
    }
    return SetRasterizerState(&m_DefaultRasterizerState);
}

/*!
 * ブレンド有効設定
 * @author teco
 */
TbResult TbRenderDevice::SetBlendState( const TbBlendState* state)
{
    if (state) {
        if (state != m_BlendState) {
            if (m_Depend.m_IsRecordingCommand || !m_Depend.m_IsEnableCommandCache) {
                const nn::gfx::BlendState& nativeState = state->GetDepend().GetState();
                nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
                commandBuffer.SetBlendState(&nativeState);
            }
            m_BlendState = state;
        }
        return TB_S_OK;
    }
    return SetBlendState(&m_DefaultBlendState);
}

/*!
 * 深度テストを有効にする
 * @author teco
 */
TbResult TbRenderDevice::SetDepthStencilState(const TbDepthStencilState* state)
{
    if (state) {
        if (state != m_DepthStencilState || !m_Depend.m_IsEnableCommandCache) {
            if (m_Depend.m_IsRecordingCommand) {
                const nn::gfx::DepthStencilState& nativeState = state->GetDepend().GetState();
                nn::gfx::CommandBuffer& commandBuffer = m_Depend.GetCommandBuffer();
                commandBuffer.SetDepthStencilState(&nativeState);
            }
            m_DepthStencilState = state;
        }
        return TB_S_OK;
    }
    return SetDepthStencilState(&m_DefaultDepthStencilState);
}

/*!
* アルファテスト有効設定
* @author teco
*/
TbResult TbRenderDevice::SetEnableAlphaTest(TbBool isEnable)
{
    // サポートしない
    return TB_E_FAIL;
}

/*!
* アルファ比較関数
* @author Miyake Shunsuke
* @author teco
*/
TbResult TbRenderDevice::SetAlphaTestFunc(TbCmpFunc func)
{
    // サポートしない
    return TB_E_FAIL;
}

/*!
* アルファ比較関数
* @author teco
*/
TbResult TbRenderDevice::SetAlphaTestRef(TbUint8 ref)
{
    // サポートしない
    return TB_E_FAIL;
}


/*!
* 状態をダンプ
* @author teco
*/
TbBool TbRenderDevice::SetFullscreen(TbBool isFullscreen)
{
    return TB_FALSE;
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