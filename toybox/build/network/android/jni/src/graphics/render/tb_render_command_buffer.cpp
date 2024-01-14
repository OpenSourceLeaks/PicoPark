/*!
 * @file
 * @brief 描画コマンドバッファ
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#include "tb_fwd.h"
#include "graphics/render/tb_render_command_buffer.h"
#include "graphics/render/3dapi/tb_3dapi_util.h"

// 指定アロケータからアロケート
void* operator new(size_t size , toybox::TbRenderCommandAllocator& allocator ) {
    return allocator.Allocate(size);
}
    
// デリート
void operator delete( void* ptr , toybox::TbRenderCommandAllocator& allocator )
{}

// 指定アロケータからアロケート
void* operator new[](size_t size , toybox::TbRenderCommandAllocator& allocator ) {
    return allocator.Allocate(size);
}
    
// デリート
void operator delete[]( void* ptr , toybox::TbRenderCommandAllocator& allocator )
{}

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbRenderCommandAllocator::TbRenderCommandAllocator()
    : m_Buffer(nullptr)
    , m_Top(0)
    , m_Size(0)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbRenderCommandAllocator::~TbRenderCommandAllocator()
{
}
    
/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
void TbRenderCommandAllocator::Initialize( void* buffer , TbSizeT size )
{
    m_Buffer = reinterpret_cast<TbUint8*>(buffer);
    m_Size = size;
}
        
/*!
 * アロケート
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
void* TbRenderCommandAllocator::Allocate( TbSizeT size )
{
    TbUint32 allocSize = TB_ALIGN_ROUND_UP( size , 16 ); // 16バイトアライメントしておく
    void* result = nullptr;
    if( TB_VERIFY(m_Top + size < m_Size) ) {
        result = &m_Buffer[m_Top];
        m_Top += size;
    }
    return result;
}

/*!
 * クリア
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
void TbRenderCommandAllocator::Clear()
{
    m_Top = 0;
}

////

/*!
 * コンストラク
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbRenderCommandBuffer::TbRenderCommandBuffer()
    : m_Allocator(nullptr)
    , m_Top(nullptr)
    , m_Tail(nullptr)
{
    SetEnable(TB_TRUE);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbRenderCommandBuffer::~TbRenderCommandBuffer()
{
}

/*!
 * 明示的にバッファをクリア
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandClear* command = new(*m_Allocator) TbRenderCommandClear();
    command->clearTargetBit = clearTargetBit;
    command->color = color;
    command->depth = depth;
    command->stencil = stencil;
    return addCommand(command);
}

/*!
 * レンダーターゲット設定
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetRenderTarget* command = new(*m_Allocator) TbRenderCommandSetRenderTarget();
    command->index = index;
    command->target = renderTarget;
    return addCommand(command);
}

/*!
 * 深度ステンシル設定
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::SetDepthStencil( TbDepthStencil* depthStencil )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetDepthStencil* command = new(*m_Allocator) TbRenderCommandSetDepthStencil();
    command->target = depthStencil;
    return addCommand(command);
}

/*!
 * ストリームソース設定
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::SetVertexBuffer( TbUint32 streamNumber , 
                                             TbVertexBuffer* vertexBuffer , 
                                             TbUint32 offsetInBytes , 
                                             TbUint32 stride )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetVertexBuffer* command = new(*m_Allocator) TbRenderCommandSetVertexBuffer();
    command->streamNumber = streamNumber;
    command->vertexBuffer = vertexBuffer;
    command->offsetInBytes = offsetInBytes;
    command->stride = stride;
    return addCommand(command);
}

/*!
 * 頂点インデックスバッファ設定
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::SetIndexBuffer( TbIndexBuffer* indexBuffer )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }

    // キャッシュチェック
    if( m_Cache.bitArray.Test(Cache::FLAG_INDEX_BUFFER) ){
        if( m_Cache.indexBuffer == indexBuffer ) {
            // キャッシュヒット
            return TB_S_OK;
        }
    }
    m_Cache.indexBuffer = indexBuffer;
    m_Cache.bitArray.SetBit(Cache::FLAG_INDEX_BUFFER);

    TbRenderCommandSetIndexBuffer* command = new(*m_Allocator) TbRenderCommandSetIndexBuffer();
    command->indexBuffer = indexBuffer;
    return addCommand(command);
}

/*!
 * プリミティブ描画
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::DrawPrimitive( const TbPrimitiveType primitiveType , 
                                               const TbUint32 startVertex , 
                                               const TbUint32 primitiveCount )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandDrawPrimitive* command = new(*m_Allocator) TbRenderCommandDrawPrimitive();
    command->primitiveType = primitiveType;
    command->startVertex = startVertex;
    command->primitiveCount = primitiveCount;
    return addCommand(command);
}

/*!
 * プリミティブ描画( 生の頂点配列使用 )
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::DrawPrimitive( const TbPrimitiveType primitiveType , 
                        const TbUint32 primitiveCount , 
                        const void* vertexs , 
                        const TbUint32 stride )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandDrawPrimitiveRaw* command = new(*m_Allocator) TbRenderCommandDrawPrimitiveRaw();
    command->primitiveType = primitiveType;
    TbSizeT size = stride*TbGetPrimitiveVertexCount(primitiveType,primitiveCount);
    command->vertexs = new(*m_Allocator) TbUint8[size];
    TbMemCopy( command->vertexs , vertexs , size );
    command->primitiveCount = primitiveCount;
    command->stride = stride;
    return addCommand(command);
}

/*!
 * 頂点インデックスを利用したプリミティブ描画
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                               const TbUint32 minVertexIndex ,
                               const TbUint32 vertexCount , 
                               const TbUint32 startIndex , 
                               const TbUint32 primitiveCount )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandDrawIndexedPrimitive* command = new(*m_Allocator) TbRenderCommandDrawIndexedPrimitive();
    command->primitiveType = primitiveType;
    command->minVertexIndex = minVertexIndex;
    command->vertexCount = vertexCount;
    command->startIndex = startIndex;
    command->primitiveCount = primitiveCount;
    return addCommand(command);
}

/*!
 * 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                               const TbUint32 minVertexIndex ,
                               const TbUint32 vertexCount , 
                               const void* indexData , 
                               const TbIndexBufferFormat indexFormat , 
                               const void* vertexs , 
                               TbUint32 stride ,
                               const TbUint32 primitiveCount )
{
    static const TbSizeT INDEX_FORMAT_SIZE[] = {
        2 ,
        4
    };

    if( !m_Allocator) {
        return TB_E_FAIL;
    }

    TbRenderCommandDrawIndexedPrimitiveRaw* command = new(*m_Allocator) TbRenderCommandDrawIndexedPrimitiveRaw();
    command->primitiveType = primitiveType;
    command->minVertexIndex = minVertexIndex;
    command->vertexCount = vertexCount;

    // インデックス配列コピー
    {
        TbSizeT size = INDEX_FORMAT_SIZE[indexFormat]*TbGetPrimitiveVertexCount(primitiveType,primitiveCount);
        command->indexData = new(*m_Allocator) TbUint8[size];
        TbMemCopy( command->indexData , indexData , size );
        command->indexFormat = indexFormat;
    }

    // 頂点コピー
    {
        TbSizeT size = stride*vertexCount;
        command->vertexs = new(*m_Allocator) TbUint8[size];
        TbMemCopy( command->vertexs , vertexs , size );
        command->stride = stride;
        command->primitiveCount = primitiveCount;
    }

    return addCommand(command);
}

/*!
 * テクスチャ設定
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetTexture* command = new(*m_Allocator) TbRenderCommandSetTexture();
    command->stage = stage;
    command->texture = texture;
    command->shaderType = shaderType;
    return addCommand(command);
}

/*!
 * テクスチャサンプラーセット
 * @author Miyake Shunsuke
 * @since 2012.12.05
 */
TbResult TbRenderCommandBuffer::SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetTextureSampler* command = new(*m_Allocator) TbRenderCommandSetTextureSampler();
    command->slot = slot;
    command->sampler = sampler;
    command->shaderType = shaderType;
    return addCommand(command);
}

// ビューポート設定
TbResult TbRenderCommandBuffer::SetViewportScissorState(const TbViewportScissorState* state)
{
    if (!m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetViewportScissorState* command = new(*m_Allocator) TbRenderCommandSetViewportScissorState();
    command->state = state;
    return addCommand(command);
}

// ラスタライザステート設定
TbResult TbRenderCommandBuffer::SetRasterizerState( const TbRasterizerState* state )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetRasterizerState* command = new(*m_Allocator) TbRenderCommandSetRasterizerState();
    command->state = state;
    return addCommand(command);
}

// ブレンド有効
TbResult TbRenderCommandBuffer::SetBlendState( const TbBlendState* state )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetBlendState* command = new(*m_Allocator) TbRenderCommandSetBlendState();
    command->state = state;
    return addCommand(command);
}

// アルファテスト有効設定
TbResult TbRenderCommandBuffer::SetEnableAlphaTest( TbBool isEnable )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetEnableAlphaTest* command = new(*m_Allocator) TbRenderCommandSetEnableAlphaTest();
    command->isEnable = isEnable;
    return addCommand(command);
}

// アルファ比較関数
TbResult TbRenderCommandBuffer::SetAlphaTestFunc( TbCmpFunc func )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetAlphaTestFunc* command = new(*m_Allocator) TbRenderCommandSetAlphaTestFunc();
    command->func = func;
    return addCommand(command);
}

// アルファ参照値
TbResult TbRenderCommandBuffer::SetAlphaTestRef( TbUint8 ref )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetAlphaTestRef* command = new(*m_Allocator) TbRenderCommandSetAlphaTestRef();
    command->ref = ref;
    return addCommand(command);
}

// 深度テストを有効設定
TbResult TbRenderCommandBuffer::SetDepthStencilState( const TbDepthStencilState* state )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetDepthStencilState* command = new(*m_Allocator) TbRenderCommandSetDepthStencilState();
    command->state = state;
    return addCommand(command);
}

// 頂点フォーマット設定
TbResult TbRenderCommandBuffer::SetFixedShaderVertexFormat( const TbVertexFormat* vertexFormat )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderVertexFormat* command = new(*m_Allocator) TbRenderCommandSetFixedShaderVertexFormat();
    command->vertexFormat = vertexFormat;
    return addCommand(command);
}

// シェーディングモード設定
TbResult TbRenderCommandBuffer::SetFixedShaderShadeMode( TbFixedShaderShadeMode shadeMode )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderShadeMode* command = new(*m_Allocator) TbRenderCommandSetFixedShaderShadeMode();
    command->shadeMode = shadeMode;
    return addCommand(command);
}

// ライト計算有効設定
TbResult TbRenderCommandBuffer::SetFixedShaderEnableLighting( TbBool isEnable )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderEnableLighting* command = new(*m_Allocator) TbRenderCommandSetFixedShaderEnableLighting();
    command->isEnable = isEnable;
    return addCommand(command);
}

// ライト有効設定
TbResult TbRenderCommandBuffer::SetFixedShaderEnableLight( TbUint32 index , TbBool isEnable )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderEnableLight* command = new(*m_Allocator) TbRenderCommandSetFixedShaderEnableLight();
    command->index = index;
    command->isEnable = isEnable;
    return addCommand(command);
}

// ライト設定
TbResult TbRenderCommandBuffer::SetFixedShaderLight( TbUint32 index , const TbFixedLight* light )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderLight* command = new(*m_Allocator) TbRenderCommandSetFixedShaderLight();
    command->index = index;
    command->light = *light;
    return addCommand(command);
}

// マテリアル設定
TbResult TbRenderCommandBuffer::SetFixedShaderMaterial( const TbFixedMaterial* material )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderMaterial* command = new(*m_Allocator) TbRenderCommandSetFixedShaderMaterial();
    command->material = *material;
    return addCommand(command);
}

// テクスチャ合成方法設定( 入力RGB )
TbResult TbRenderCommandBuffer::SetFixedShaderTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderTextureColorArg* command = new(*m_Allocator) TbRenderCommandSetFixedShaderTextureColorArg();
    command->stage = stage;
    command->arg = arg;
    return addCommand(command);
}

// テクスチャ合成方法設定( RGB演算方式 )
TbResult TbRenderCommandBuffer::SetFixedShaderTextureColorOp( TbUint32 stage , TbTextureOp op )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderTextureColorOp* command = new(*m_Allocator) TbRenderCommandSetFixedShaderTextureColorOp();
    command->stage = stage;
    command->op = op;
    return addCommand(command);
}

// テクスチャ合成方法設定( 入力アルファ )
TbResult TbRenderCommandBuffer::SetFixedShaderTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderTextureAlphaArg* command = new(*m_Allocator) TbRenderCommandSetFixedShaderTextureAlphaArg();
    command->stage = stage;
    command->arg = arg;
    return addCommand(command);
}

// テクスチャ合成方法設定( アルファ演算方式 )
TbResult TbRenderCommandBuffer::SetFixedShaderTextureAlphaOp( TbUint32 stage , TbTextureOp op )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetFixedShaderTextureAlphaOp* command = new(*m_Allocator) TbRenderCommandSetFixedShaderTextureAlphaOp();
    command->stage = stage;
    command->op = op;
    return addCommand(command);
}

// シェーダバインド
TbResult TbRenderCommandBuffer::SetShaderProgram( TbShaderProgram* program )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }

    // キャッシュチェック
    if( m_Cache.bitArray.Test(Cache::FLAG_SHADER_PROGRAM) ){
        if( m_Cache.shaderProgram == program ) {
            // キャッシュヒット
            return TB_S_OK;
        }
    }
    m_Cache.shaderProgram = program;
    m_Cache.bitArray.SetBit(Cache::FLAG_SHADER_PROGRAM);

    // コマンド追加
    TbRenderCommandSetShaderProgram* command = new(*m_Allocator) TbRenderCommandSetShaderProgram();
    command->program = program;
    return addCommand(command);
}

// 頂点シェーダ定数設定( ハンドルから )
TbResult TbRenderCommandBuffer::SetShaderConstantBool( TbUint32 shaderBit , 
                                                       TbShaderConstHandle* handle ,
                                                       const TbBool* value ,
                                                       TbUint32 count )
{
    if( !m_Allocator || !handle ) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetShaderConstB* command = new(*m_Allocator) TbRenderCommandSetShaderConstB();
    command->shaderBit = shaderBit;
    command->handle = handle;
    command->value = new(*m_Allocator) TbBool[count];
    command->count = count;
    TbMemCopy(command->value,value,sizeof(*value)*count);
    return addCommand(command);
}

// シェーダ定数設定( ハンドルから )
TbResult TbRenderCommandBuffer::SetShaderConstantF32( TbUint32 shaderBit , 
                                                      TbShaderConstHandle* handle , 
                                                      const TbFloat32* value , 
                                                      TbUint32 count )
{
    if( !m_Allocator || !handle ) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetShaderConstF* command = new(*m_Allocator) TbRenderCommandSetShaderConstF();
    command->shaderBit = shaderBit;
    command->handle = handle;
    command->value = new(*m_Allocator) TbFloat32[count];
    command->count = count;
    TbMemCopy(command->value,value,sizeof(*value)*count);
    return addCommand(command);
}

// シェーダ定数設定( ハンドルから )
TbResult TbRenderCommandBuffer::SetShaderConstantS32( TbUint32 shaderBit , 
                                                      TbShaderConstHandle* handle , 
                                                      const TbSint32* value , 
                                                      TbUint32 count )
{
    if( !m_Allocator || !handle ) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetShaderConstI* command = new(*m_Allocator) TbRenderCommandSetShaderConstI();
    command->shaderBit = shaderBit;
    command->handle = handle;
    command->value = new(*m_Allocator) TbSint32[count];
    command->count = count;
    TbMemCopy(command->value,value,sizeof(*value)*count);
    return addCommand(command);
}

// シェーダ定数設定( ハンドルから )
TbResult TbRenderCommandBuffer::SetShaderConstantMatrix( TbUint32 shaderBit , 
                                                         TbShaderConstHandle* handle , 
                                                         const TbMatrix44* value , 
                                                         TbUint32 count )
{
    if( !m_Allocator || !handle ) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetShaderConstMtx* command = new(*m_Allocator) TbRenderCommandSetShaderConstMtx();
    command->shaderBit = shaderBit;
    command->handle = handle;
    command->value = new(*m_Allocator) TbMatrix[count];
    command->count = count;
    TbMemCopy(command->value,value,sizeof(*value)*count);
    return addCommand(command);
}

// シェーダ定数設定( ハンドルから )
TbResult TbRenderCommandBuffer::SetShaderConstantMatrix( TbUint32 shaderBit , 
                                                         TbShaderConstHandle* handle , 
                                                         const TbMatrix43* value , 
                                                         TbUint32 count )
{
    if( !m_Allocator || !handle ) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetShaderConstMtx43* command = new(*m_Allocator) TbRenderCommandSetShaderConstMtx43();
    command->shaderBit = shaderBit;
    command->handle = handle;
    command->value = new(*m_Allocator) TbMatrix43[count];
    command->count = count;
    TbMemCopy(command->value,value,sizeof(*value)*count);
    return addCommand(command);
}

// シェーダ定数設定( ハンドルから )
TbResult TbRenderCommandBuffer::SetShaderConstantBuffer(TbUint32 shaderBit,
    TbShaderConstHandle* handle,
    const TbConstantBuffer* buffer )
{
    if (!m_Allocator || !handle) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetShaderConstBuffer* command = new(*m_Allocator) TbRenderCommandSetShaderConstBuffer();
    command->shaderBit = shaderBit;
    command->handle = handle;
    command->buffer = buffer;
    return addCommand(command);
}

// ワールド行列
TbResult TbRenderCommandBuffer::SetWorldMatrix( const TbMatrix44& matrix )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetWorldMatrix* command = new(*m_Allocator) TbRenderCommandSetWorldMatrix();
    command->matrix= matrix;
    return addCommand(command);
}

// ビュー行列
TbResult TbRenderCommandBuffer::SetViewMatrix( const TbMatrix44& matrix )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetViewMatrix* command = new(*m_Allocator) TbRenderCommandSetViewMatrix();
    command->matrix= matrix;
    return addCommand(command);
}

// 射影行列
TbResult TbRenderCommandBuffer::SetProjectionMatrix( const TbMatrix44& matrix )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetProjectionMatrix* command = new(*m_Allocator) TbRenderCommandSetProjectionMatrix();
    command->matrix= matrix;
    return addCommand(command);
}

// View Projection行列
TbResult TbRenderCommandBuffer::SetViewProjectionMatrix( const TbMatrix44& view , const TbMatrix44& projection )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetViewProjectionMatrix* command = new(*m_Allocator) TbRenderCommandSetViewProjectionMatrix();
    command->view = view;
    command->projection = projection;
    return addCommand(command);
}

// World View Projection行列
TbResult TbRenderCommandBuffer::SetWorldViewProjectionMatrix( const TbMatrix44& world , const TbMatrix44& view , const TbMatrix44& projection )
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandSetWorldViewProjectionMatrix* command = new(*m_Allocator) TbRenderCommandSetWorldViewProjectionMatrix();
    command->world = world;
    command->view = view;
    command->projection = projection;
    return addCommand(command);

}

// デバイス状態をダンプする
TbResult TbRenderCommandBuffer::DumpDevice()
{
    if( !m_Allocator) {
        return TB_E_FAIL;
    }
    TbRenderCommandDumpDevice* command = new(*m_Allocator) TbRenderCommandDumpDevice();
    return addCommand(command);
}

}
