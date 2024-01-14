/*!
 * @file
 * @brief 描画コマンドバッファ発行
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#include "tb_fwd.h"
#include "graphics/render/tb_render_command_buffer.h"
#include "graphics/render/3dapi/tb_render_device.h"

namespace toybox
{

namespace
{
    //! 明示的にバッファをクリア
    void flushClear( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandClear* cmd = reinterpret_cast<TbRenderCommandClear*>(cmdBase);
        device.Clear(cmd->clearTargetBit,cmd->color,cmd->depth,cmd->stencil);
    }

    //! カラーバッファに書き込むマスク設定
    // (レンダーターゲット別は現在未サポート)
    void flushSetColorWriteMask( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
    }

    // レンダーターゲット設定
    void flushSetRenderTarget( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetRenderTarget* cmd = reinterpret_cast<TbRenderCommandSetRenderTarget*>(cmdBase);
        device.SetRenderTarget(cmd->index,cmd->target);
    }

    // 深度ステンシル設定
    void flushSetDepthStencil( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetDepthStencil* cmd = reinterpret_cast<TbRenderCommandSetDepthStencil*>(cmdBase);
        device.SetDepthStencil(cmd->target);
    }
    
    // ストリームソース設定
    void flushSetVertexBuffer( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetVertexBuffer* cmd = reinterpret_cast<TbRenderCommandSetVertexBuffer*>(cmdBase);
        device.SetVertexBuffer(cmd->streamNumber,cmd->vertexBuffer,cmd->offsetInBytes,cmd->stride);
    }

    // 頂点インデックスバッファ設定
    void flushSetIndexBuffer( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetIndexBuffer* cmd = reinterpret_cast<TbRenderCommandSetIndexBuffer*>(cmdBase);
        device.SetIndexBuffer(cmd->indexBuffer);
    }

    // プリミティブ描画
    void flushDrawPrimitive( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandDrawPrimitive* cmd = reinterpret_cast<TbRenderCommandDrawPrimitive*>(cmdBase);
        device.DrawPrimitive(cmd->primitiveType,cmd->startVertex,cmd->primitiveCount);
    }

    // プリミティブ描画( 生の頂点配列使用 )
    void flushDrawPrimitiveRaw( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandDrawPrimitiveRaw* cmd = reinterpret_cast<TbRenderCommandDrawPrimitiveRaw*>(cmdBase);
        device.DrawPrimitive(cmd->primitiveType,
                             cmd->primitiveCount,
                             cmd->vertexs,
                             cmd->stride);
    }

    // 頂点インデックスを利用したプリミティブ描画
    void flushDrawIndexedPrimitive( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandDrawIndexedPrimitive* cmd = reinterpret_cast<TbRenderCommandDrawIndexedPrimitive*>(cmdBase);
        device.DrawIndexedPrimitive(cmd->primitiveType,
                                    cmd->minVertexIndex,
                                    cmd->vertexCount,
                                    cmd->startIndex,
                                    cmd->primitiveCount);
    }

    // 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
    void flushDrawIndexedPrimitiveRaw( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandDrawIndexedPrimitiveRaw* cmd = reinterpret_cast<TbRenderCommandDrawIndexedPrimitiveRaw*>(cmdBase);
        device.DrawIndexedPrimitive(cmd->primitiveType,
                                    cmd->minVertexIndex,
                                    cmd->vertexCount,
                                    cmd->indexData,
                                    cmd->indexFormat,
                                    cmd->vertexs,
                                    cmd->stride,
                                    cmd->primitiveCount);
    }

    // テクスチャ設定
    void flushSetTexture( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetTexture* cmd = reinterpret_cast<TbRenderCommandSetTexture*>(cmdBase);
        device.SetTexture(cmd->stage,cmd->texture,cmd->shaderType);
    }

    // テクスチャサンプラーセット
    void flushSetTextureSampler( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetTextureSampler* cmd = reinterpret_cast<TbRenderCommandSetTextureSampler*>(cmdBase);
        device.SetTextureSampler(cmd->slot,cmd->sampler,cmd->shaderType);
    }

    // ビューポート設定
    void flushSetViewportScissorState(TbRenderDevice& device, TbRenderCommand* cmdBase)
    {
        TbRenderCommandSetViewportScissorState* cmd = reinterpret_cast<TbRenderCommandSetViewportScissorState*>(cmdBase);
        device.SetViewportScissorState(cmd->state);
    }
    
    // ラスタライザー設定
    void flushSetRasterizerState( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetRasterizerState* cmd = reinterpret_cast<TbRenderCommandSetRasterizerState*>(cmdBase);
        device.SetRasterizerState(cmd->state);
    }

    // ブレンド係数
    void flushSetBlendState( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetBlendState* cmd = reinterpret_cast<TbRenderCommandSetBlendState*>(cmdBase);
        device.SetBlendState(cmd->state);
    }

    // アルファテスト有効設定
    void flushSetEnableAlphaTest( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetEnableAlphaTest* cmd = reinterpret_cast<TbRenderCommandSetEnableAlphaTest*>(cmdBase);
        device.SetEnableAlphaTest(cmd->isEnable);
    }

    // アルファ比較関数
    void flushSetAlphaTestFunc( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetAlphaTestFunc* cmd = reinterpret_cast<TbRenderCommandSetAlphaTestFunc*>(cmdBase);
        device.SetAlphaTestFunc(cmd->func);
    }

    // アルファ参照値
    void flushSetAlphaTestRef( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetAlphaTestRef* cmd = reinterpret_cast<TbRenderCommandSetAlphaTestRef*>(cmdBase);
        device.SetAlphaTestRef(cmd->ref);
    }

    // 深度ステンシルタイプ
    void flushSetDepthStencilState( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetDepthStencilState* cmd = reinterpret_cast<TbRenderCommandSetDepthStencilState*>(cmdBase);
        device.SetDepthStencilState(cmd->state);
    }

    // 頂点フォーマット設定
    void flushSetFixedShaderVertexFormat( TbRenderDevice& device , TbRenderCommand* cmdBase)
    {
        TbRenderCommandSetFixedShaderVertexFormat* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderVertexFormat*>(cmdBase);
        device.GetFixedShader().SetVertexFormat(cmd->vertexFormat);
    }

    // シェーディングモード設定
    void flushSetFixedShaderShadeMode( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderShadeMode* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderShadeMode*>(cmdBase);
        device.GetFixedShader().SetShadeMode(cmd->shadeMode);
    }

    // ライト計算有効設定
    void flushSetFixedShaderEnableLighting( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderEnableLighting* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderEnableLighting*>(cmdBase);
        device.GetFixedShader().SetEnableLighting(cmd->isEnable);
    }

    // ライト有効設定
    void flushSetFixedShaderEnableLight( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderEnableLight* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderEnableLight*>(cmdBase);
        device.GetFixedShader().SetEnableLight(cmd->index,cmd->isEnable);
    }

    // ライト設定
    void flushSetFixedShaderLight( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderLight* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderLight*>(cmdBase);
        device.GetFixedShader().SetLight(cmd->index,&cmd->light);
    }

    // マテリアル設定
    void flushSetFixedShaderMaterial( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderMaterial* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderMaterial*>(cmdBase);
        device.GetFixedShader().SetMaterial(&cmd->material);
    }

    // テクスチャ合成方法設定( 入力RGB )
    void flushSetFixedShaderTextureColorArg( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderTextureColorArg* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderTextureColorArg*>(cmdBase);
        device.GetFixedShader().SetTextureColorArg(cmd->stage,cmd->argIndex,cmd->arg);
    }

    // テクスチャ合成方法設定( RGB演算方式 )
    void flushSetFixedShaderTextureColorOp( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderTextureColorOp* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderTextureColorOp*>(cmdBase);
        device.GetFixedShader().SetTextureColorOp(cmd->stage,cmd->op);
    }

    // テクスチャ合成方法設定( 入力アルファ )
    void flushSetFixedShaderTextureAlphaArg( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderTextureAlphaArg* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderTextureAlphaArg*>(cmdBase);
        device.GetFixedShader().SetTextureAlphaArg(cmd->stage,cmd->argIndex,cmd->arg);
    }

    // テクスチャ合成方法設定( アルファ演算方式 )
    void flushSetFixedShaderTextureAlphaOp( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetFixedShaderTextureAlphaOp* cmd = reinterpret_cast<TbRenderCommandSetFixedShaderTextureAlphaOp*>(cmdBase);
        device.GetFixedShader().SetTextureAlphaOp(cmd->stage,cmd->op);
    }

    // シェーダバインド
    void flushSetShaderProgram( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetShaderProgram* cmd = reinterpret_cast<TbRenderCommandSetShaderProgram*>(cmdBase);
        device.GetShaderManager().SetProgram(cmd->program);
    }

    // シェーダ定数設定( ハンドルから )
    void flushSetShaderConstantB( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetShaderConstB* cmd = reinterpret_cast<TbRenderCommandSetShaderConstB*>(cmdBase);
        device.GetShaderManager().SetConstantBool(cmd->shaderBit,*cmd->handle,cmd->value,cmd->count);
    }

    // シェーダ定数設定( ハンドルから )
    void flushSetShaderConstantF( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetShaderConstF* cmd = reinterpret_cast<TbRenderCommandSetShaderConstF*>(cmdBase);
        device.GetShaderManager().SetConstantF32(cmd->shaderBit,*cmd->handle,cmd->value,cmd->count);
    }

    // シェーダ定数設定( ハンドルから )
    void flushSetShaderConstantI( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetShaderConstI* cmd = reinterpret_cast<TbRenderCommandSetShaderConstI*>(cmdBase);
        device.GetShaderManager().SetConstantS32(cmd->shaderBit,*cmd->handle,cmd->value,cmd->count);
    }

    // シェーダ定数設定( ハンドルから )
    void flushSetShaderConstantMatrix( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetShaderConstMtx* cmd = reinterpret_cast<TbRenderCommandSetShaderConstMtx*>(cmdBase);
        device.GetShaderManager().SetConstantMatrix(cmd->shaderBit,*cmd->handle,cmd->value,cmd->count);
    }

    // シェーダ定数設定( ハンドルから )
    void flushSetShaderConstantMatrix43( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetShaderConstMtx43* cmd = reinterpret_cast<TbRenderCommandSetShaderConstMtx43*>(cmdBase);
        device.GetShaderManager().SetConstantMatrix(cmd->shaderBit,*cmd->handle,cmd->value,cmd->count);
    }

    // シェーダ定数設定( ハンドルから )
    void flushSetShaderConstantBuffer(TbRenderDevice& device, TbRenderCommand* cmdBase)
    {
        TbRenderCommandSetShaderConstBuffer* cmd = reinterpret_cast<TbRenderCommandSetShaderConstBuffer*>(cmdBase);
        device.GetShaderManager().SetConstantBuffer(cmd->shaderBit, *cmd->handle, const_cast<TbConstantBuffer*>(cmd->buffer) );
    }

    // ワールド行列
    void flushSetWorldMatrix( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetWorldMatrix* cmd = reinterpret_cast<TbRenderCommandSetWorldMatrix*>(cmdBase);
        device.SetWorldMatrix(cmd->matrix);
    }

    // ビュー行列
    void flushSetViewMatrix( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetViewMatrix* cmd = reinterpret_cast<TbRenderCommandSetViewMatrix*>(cmdBase);
        device.SetViewMatrix(cmd->matrix);
    }

    // 射影行列
    void flushSetProjectionMatrix( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetProjectionMatrix* cmd = reinterpret_cast<TbRenderCommandSetProjectionMatrix*>(cmdBase);
        device.SetProjectionMatrix(cmd->matrix);
    }

    // View Projection行列のみはこれを利用して設定 )
    void flushSetViewProjectionMatrix( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetWorldViewProjectionMatrix* cmd = reinterpret_cast<TbRenderCommandSetWorldViewProjectionMatrix*>(cmdBase);
        device.SetViewProjectionMatrix(cmd->view,cmd->projection);
    }

    // World View Projection行列のみはこれを利用して設定 )
    void flushSetWorldViewProjectionMatrix( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        TbRenderCommandSetWorldViewProjectionMatrix* cmd = reinterpret_cast<TbRenderCommandSetWorldViewProjectionMatrix*>(cmdBase);
        device.SetWorldViewProjectionMatrix(cmd->world,cmd->view,cmd->projection);
    }

    // デバイス状態をダンプ
    void flushDumpDevice( TbRenderDevice& device , TbRenderCommand* cmdBase )
    {
        device.Dump();
    }

    typedef void (*CommandFlushFunc)(TbRenderDevice& device,TbRenderCommand* cmdBase);
    const CommandFlushFunc COMMAND_FLUSH_FUNC[] = 
    {
        flushClear                  , // TB_RENDER_COMMAND_TYPE_CLEAR
        flushSetRenderTarget        , // TB_RENDER_COMMAND_TYPE_SET_RENDER_TARGET
        flushSetDepthStencil        , // TB_RENDER_COMMAND_TYPE_SET_DEPTH_STENCIL
        flushSetVertexBuffer       , // TB_RENDER_COMMAND_TYPE_SET_STREAM_SOURCE
        flushSetIndexBuffer             , // TB_RENDER_COMMAND_TYPE_SET_INDICES
        flushDrawPrimitive          , // TB_RENDER_COMMAND_TYPE_DRAW_PRIM
        flushDrawPrimitiveRaw       , // TB_RENDER_COMMAND_TYPE_DRAW_PRIM_RAW
        flushDrawIndexedPrimitive   , // TB_RENDER_COMMAND_TYPE_DRAW_INDEXED_PRIM
        flushDrawIndexedPrimitiveRaw , // TB_RENDER_COMMAND_TYPE_DRAW_INDEXED_PRIM_RAW
        flushSetTexture             , // TB_RENDER_COMMAND_TYPE_SET_TEXTURE
        flushSetTextureSampler      , // TB_RENDER_COMMAND_TYPE_SET_TEXTURE_SAMPLER
        flushSetViewportScissorState    , // TB_RENDER_COMMAND_TYPE_SET_VIEWPORT_SCISSORS_STATE
        flushSetRasterizerState     , // TB_RENDER_COMMAND_TYPE_SET_RASTERIZER_STATE
        flushSetBlendState          , // TB_RENDER_COMMAND_TYPE_SET_BLEND_STATE
        flushSetEnableAlphaTest     , // TB_RENDER_COMMAND_TYPE_SET_ENABLE_ALPHA_TEST
        flushSetAlphaTestFunc       , // TB_RENDER_COMMAND_TYPE_SET_ALPHA_TEST_FUNC 
        flushSetAlphaTestRef        , // TB_RENDER_COMMAND_TYPE_SET_ALPHA_TEST_REF 
        flushSetDepthStencilState   , // TB_RENDER_COMMAND_TYPE_SET_DEPTH_STENCIL_STATE
        flushSetFixedShaderVertexFormat , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_VERTEX_FORMAT
        flushSetFixedShaderShadeMode    , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_SHADEMODE
        flushSetFixedShaderEnableLighting , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_ENABLE_LIGHTING
        flushSetFixedShaderEnableLight    , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_ENABLE_LIGHT
        flushSetFixedShaderLight          , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_LIGHT
        flushSetFixedShaderMaterial       , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_MATERIAL
        flushSetFixedShaderTextureColorArg  , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_COLOR_ARG
        flushSetFixedShaderTextureColorOp  , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_COLOR_OP
        flushSetFixedShaderTextureAlphaArg , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_ALPHA_ARG
        flushSetFixedShaderTextureAlphaOp  , // TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_ALPHA_OP
        flushSetShaderProgram       , // TB_RENDER_COMMAND_TYPE_SET_PROGRAM
        flushSetShaderConstantB , // TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_B  
        flushSetShaderConstantF , // TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_F  
        flushSetShaderConstantI , // TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_I  
        flushSetShaderConstantMatrix , // TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_MTX  
        flushSetShaderConstantMatrix43 , // TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_MTX43  
        flushSetShaderConstantBuffer , // TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_BUF  
        flushSetWorldMatrix , // TB_RENDER_COMMAND_TYPE_SET_WORLD_MATRIX , 
        flushSetViewMatrix , // TB_RENDER_COMMAND_TYPE_SET_VIEW_MATRIX , 
        flushSetProjectionMatrix , // TB_RENDER_COMMAND_TYPE_SET_PROJECTION_MATRIX , 
        flushSetViewProjectionMatrix , // TB_RENDER_COMMAND_TYPE_SET_VIEW_PROJECTION_MATRIX , 
        flushSetWorldViewProjectionMatrix , // TB_RENDER_COMMAND_TYPE_SET_WORLD_VIEW_PROJECTION_MATRIX ,  
        flushDumpDevice , // TB_RENDER_COMMAND_TYPE_DUMP_DEVICE
    };
    TB_STATIC_ASSERT(TB_RENDER_COMMAND_TYPE_MAX==TB_ARRAY_COUNT_OF(COMMAND_FLUSH_FUNC));
};

/*!
 * 描画コマンド発行
 * @author Miyake Shunsuke
 * @since 2013.02.28
 */
void TbRenderCommandBuffer::Flush()
{
#if 1
    TbRenderCommand* command = m_Top;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    while( command ) {
        if( TB_VERIFY(command->type < TB_RENDER_COMMAND_TYPE_MAX) ){
            COMMAND_FLUSH_FUNC[command->type](device,command);
        }
        command = command->next;
    }
#endif
}

}
