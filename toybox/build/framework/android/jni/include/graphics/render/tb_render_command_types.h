/*!
 * @file
 * @brief 描画コマンド定義
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_COMMAND_TYPES_H_
#define _INCLUDED_TB_RENDER_COMMAND_TYPES_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_3dapi.h>

namespace toybox
{

// 描画コマンドタイプ
enum TbRenderCommandType
{
    // クリア
    TB_RENDER_COMMAND_TYPE_CLEAR ,                  // バッファクリア
    // 描画設定
    TB_RENDER_COMMAND_TYPE_SET_RENDER_TARGET    ,   // レンダーターゲット設定
    TB_RENDER_COMMAND_TYPE_SET_DEPTH_STENCIL    ,   // 深度ステンシルバッファ設定
    // 描画
    TB_RENDER_COMMAND_TYPE_SET_STREAM_SOURCE    ,   //  ストリームソース設定
    TB_RENDER_COMMAND_TYPE_SET_INDICES          ,   //  インデックスバッファ設定 
    TB_RENDER_COMMAND_TYPE_DRAW_PRIM            ,   //  プリミティブ描画 
    TB_RENDER_COMMAND_TYPE_DRAW_PRIM_RAW        ,   //  プリミティブ描画 
    TB_RENDER_COMMAND_TYPE_DRAW_INDEXED_PRIM    ,   //  プリミティブ描画
    TB_RENDER_COMMAND_TYPE_DRAW_INDEXED_PRIM_RAW,   //  プリミティブ描画
    // テクスチャ設定
    TB_RENDER_COMMAND_TYPE_SET_TEXTURE ,                // テクスチャ設定
    TB_RENDER_COMMAND_TYPE_SET_TEXTURE_SAMPLER ,        // テクスチャサンプラ設定
    // ビューポートシザー設定
    TB_RENDER_COMMAND_TYPE_SET_VIEWPORT_SCISSOR_STATE ,   // 
    // ラスタライズ設定
    TB_RENDER_COMMAND_TYPE_SET_RASTERIZER_STATE  ,  // ラスタライズ設定
    // ブレンド設定
    TB_RENDER_COMMAND_TYPE_SET_BLEND_STATE,           // ブレンド設定
    // アルファテスト
    TB_RENDER_COMMAND_TYPE_SET_ENABLE_ALPHA_TEST ,      // 
    TB_RENDER_COMMAND_TYPE_SET_ALPHA_TEST_FUNC ,
    TB_RENDER_COMMAND_TYPE_SET_ALPHA_TEST_REF ,
    // 深度テスト
    TB_RENDER_COMMAND_TYPE_SET_DEPTH_STENCIL_STATE ,
    // 固定機能シェーダ設定
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_VERTEX_FORMAT , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_SHADEMODE , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_ENABLE_LIGHTING , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_ENABLE_LIGHT , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_LIGHT , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_MATERIAL , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_COLOR_ARG , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_COLOR_OP , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_ALPHA_ARG , 
    TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_ALPHA_OP , 
    // シェーダ設定
    TB_RENDER_COMMAND_TYPE_SET_RROGRAM , 
    TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_B , 
    TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_F , 
    TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_I , 
    TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_MTX , 
    TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_MTX43 , 
    TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_BUF,
    // 行列
    TB_RENDER_COMMAND_TYPE_SET_WORLD_MATRIX , 
    TB_RENDER_COMMAND_TYPE_SET_VIEW_MATRIX , 
    TB_RENDER_COMMAND_TYPE_SET_PROJECTION_MATRIX , 
    TB_RENDER_COMMAND_TYPE_SET_VIEW_PROJECTION_MATRIX , 
    TB_RENDER_COMMAND_TYPE_SET_WORLD_VIEW_PROJECTION_MATRIX , 
    // デバッグ用
    TB_RENDER_COMMAND_TYPE_DUMP_DEVICE , 
    //
    TB_RENDER_COMMAND_TYPE_MAX , 
};

// 描画コマンド
struct TbRenderCommand
{
    TbRenderCommand( TbRenderCommandType in_type )
        : type(in_type) , next(nullptr)
    {}

    TbRenderCommandType type;
    TbRenderCommand*    next;
};

// 描画コマンド(クリア)
struct TbRenderCommandClear : public TbRenderCommand
{
    TbRenderCommandClear()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_CLEAR)
    {}
    TbUint32 clearTargetBit;
    TbClearColor color;
    TbFloat32    depth;
    TbUint32     stencil;
};

// 描画コマンド(レンダーターゲット設定))
struct TbRenderCommandSetRenderTarget : public TbRenderCommand
{
    TbRenderCommandSetRenderTarget()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_RENDER_TARGET)
    {}
    TbUint32        index;
    TbRenderTarget* target;
};

// 描画コマンド(深度ステンシルバッファ設定))
struct TbRenderCommandSetDepthStencil : public TbRenderCommand
{
    TbRenderCommandSetDepthStencil()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_DEPTH_STENCIL)
    {}
    TbDepthStencil* target;
};

// 描画コマンド(ストリームソース設定)
struct TbRenderCommandSetVertexBuffer: public TbRenderCommand
{
    TbRenderCommandSetVertexBuffer()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_STREAM_SOURCE)
    {}
    TbUint32        streamNumber;
    TbVertexBuffer* vertexBuffer;
    TbUint32        offsetInBytes;
    TbUint32        stride;
};

// インデックスバッファ設定
struct TbRenderCommandSetIndexBuffer : public TbRenderCommand
{
    TbRenderCommandSetIndexBuffer()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_INDICES)
    {}
    TbIndexBuffer* indexBuffer;
};

// プリミティブ描画
struct TbRenderCommandDrawPrimitive : public TbRenderCommand
{
    TbRenderCommandDrawPrimitive()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_DRAW_PRIM)
    {}
    TbPrimitiveType primitiveType;
    TbUint32 startVertex;
    TbUint32 primitiveCount;
};

// プリミティブ描画
struct TbRenderCommandDrawPrimitiveRaw : public TbRenderCommand
{
    TbRenderCommandDrawPrimitiveRaw()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_DRAW_PRIM_RAW)
    {}
    TbPrimitiveType primitiveType;
    void*    vertexs;
    TbUint32 primitiveCount;
    TbUint32 stride;
};

// プリミティブ描画
struct TbRenderCommandDrawIndexedPrimitive : public TbRenderCommand
{
    TbRenderCommandDrawIndexedPrimitive()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_DRAW_INDEXED_PRIM)
    {}
    TbPrimitiveType primitiveType;
    TbUint32 minVertexIndex;
    TbUint32 vertexCount;
    TbUint32 startIndex;
    TbUint32 primitiveCount;
};

// プリミティブ描画
struct TbRenderCommandDrawIndexedPrimitiveRaw : public TbRenderCommand
{
    TbRenderCommandDrawIndexedPrimitiveRaw()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_DRAW_INDEXED_PRIM_RAW)
    {}
    TbPrimitiveType primitiveType;
    TbUint32 minVertexIndex;
    TbUint32 vertexCount;
    void*    indexData;
    TbIndexBufferFormat indexFormat;
    void* vertexs;
    TbUint32 stride;
    TbUint32 primitiveCount;
};

// テクスチャ設定
struct TbRenderCommandSetTexture : public TbRenderCommand
{
    TbRenderCommandSetTexture()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_TEXTURE)
    {}
    TbSint32 stage;
    TbTexture* texture;
    TbShaderType shaderType;
};

// テクスチャサンプラ設定
struct TbRenderCommandSetTextureSampler : public TbRenderCommand
{
    TbRenderCommandSetTextureSampler()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_TEXTURE_SAMPLER)
    {}
    TbSint32 slot;
    TbTextureSampler* sampler;
    TbShaderType shaderType;
};


// ビューポートシザーステート設定)
struct TbRenderCommandSetViewportScissorState : public TbRenderCommand
{
    TbRenderCommandSetViewportScissorState()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_VIEWPORT_SCISSOR_STATE)
    {}
    const TbViewportScissorState* state;
};

// ラスタライザ設定
struct TbRenderCommandSetRasterizerState : public TbRenderCommand
{
    TbRenderCommandSetRasterizerState()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_RASTERIZER_STATE)
    {}
    const TbRasterizerState* state;
};

// ブレンド有効
struct TbRenderCommandSetBlendState : public TbRenderCommand
{
    TbRenderCommandSetBlendState()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_BLEND_STATE)
    {}
    const TbBlendState* state;
};

// アルファテスト有効設定
struct TbRenderCommandSetEnableAlphaTest : public TbRenderCommand
{
    TbRenderCommandSetEnableAlphaTest()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_ENABLE_ALPHA_TEST)
    {}
    TbBool isEnable;
};

// アルファ比較関数
struct TbRenderCommandSetAlphaTestFunc : public TbRenderCommand
{
    TbRenderCommandSetAlphaTestFunc()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_ALPHA_TEST_FUNC)
    {}
    TbCmpFunc func;
};

// アルファ参照値
struct TbRenderCommandSetAlphaTestRef : public TbRenderCommand
{
    TbRenderCommandSetAlphaTestRef()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_ALPHA_TEST_REF)
    {}
    TbUint8 ref;
};

// 深度ステンシル有効
struct TbRenderCommandSetDepthStencilState : public TbRenderCommand
{
    TbRenderCommandSetDepthStencilState()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_DEPTH_STENCIL_STATE)
    {}
    const TbDepthStencilState* state;
};

// 頂点フォーマット設定
struct TbRenderCommandSetFixedShaderVertexFormat : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderVertexFormat()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_VERTEX_FORMAT)
    {}
    const TbVertexFormat* vertexFormat;
};


// シェーディングモード設定
struct TbRenderCommandSetFixedShaderShadeMode : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderShadeMode()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_SHADEMODE)
    {}
    TbFixedShaderShadeMode shadeMode;
};

// ライト計算有効設定
struct TbRenderCommandSetFixedShaderEnableLighting : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderEnableLighting()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_ENABLE_LIGHTING)
    {}
    TbBool isEnable;
};

// ライト有効設定
struct TbRenderCommandSetFixedShaderEnableLight : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderEnableLight()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_ENABLE_LIGHT)
    {}
    TbUint32 index;
    TbBool isEnable;
};

// ライト設定
struct TbRenderCommandSetFixedShaderLight : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderLight()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_LIGHT)
    {}
    TbUint32 index;
    TbFixedLight light;
};

// マテリアル設定
struct TbRenderCommandSetFixedShaderMaterial : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderMaterial()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_MATERIAL)
    {}
    TbFixedMaterial material;
};

// テクスチャ合成方法設定( 入力RGB )
struct TbRenderCommandSetFixedShaderTextureColorArg : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderTextureColorArg()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_COLOR_ARG)
    {}
    TbUint32 stage;
    TbUint32 argIndex;
    TbTextureArg arg;
};

// テクスチャ合成方法設定( RGB演算方式 )
struct TbRenderCommandSetFixedShaderTextureColorOp : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderTextureColorOp()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_COLOR_OP)
    {}
    TbUint32 stage;
    TbTextureOp op;
};

// テクスチャ合成方法設定( 入力アルファ )
struct TbRenderCommandSetFixedShaderTextureAlphaArg : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderTextureAlphaArg()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_ALPHA_ARG)
    {}
    TbUint32 stage;
    TbUint32 argIndex;
    TbTextureArg arg;
};

// テクスチャ合成方法設定( アルファ演算方式 )
struct TbRenderCommandSetFixedShaderTextureAlphaOp : public TbRenderCommand
{
    TbRenderCommandSetFixedShaderTextureAlphaOp()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_FIXED_SHADER_TEXTURE_ALPHA_OP)
    {}
    TbUint32 stage;
    TbTextureOp op;
};

// シェーダバインド
struct TbRenderCommandSetShaderProgram : public TbRenderCommand
{
    TbRenderCommandSetShaderProgram()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_RROGRAM)
    {}
    TbShaderProgram* program;
};

// 頂点シェーダ定数設定ハンドルから
struct TbRenderCommandSetShaderConstB : public TbRenderCommand
{
    TbRenderCommandSetShaderConstB()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_B)
    {}
    TbUint32 shaderBit;
    TbShaderConstHandle* handle;
    TbBool* value;
    TbUint32 count;
};

// 頂点シェーダ定数設定ハンドルから
struct TbRenderCommandSetShaderConstF : public TbRenderCommand
{
    TbRenderCommandSetShaderConstF()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_F)
    {}
    TbUint32 shaderBit;
    TbShaderConstHandle* handle;
    TbFloat32* value;
    TbUint32 count;
};

// 頂点シェーダ定数設定ハンドルから
struct TbRenderCommandSetShaderConstI : public TbRenderCommand
{
    TbRenderCommandSetShaderConstI()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_I)
    {}
    TbUint32 shaderBit;
    TbShaderConstHandle* handle;
    TbSint32* value;
    TbUint32 count;
};

// 頂点シェーダ定数設定ハンドルから
struct TbRenderCommandSetShaderConstMtx : public TbRenderCommand
{
    TbRenderCommandSetShaderConstMtx()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_MTX)
    {}
    TbUint32 shaderBit;
    TbShaderConstHandle* handle;
    TbMatrix44* value;
    TbUint32 count;
};

// 頂点シェーダ定数設定ハンドルから
struct TbRenderCommandSetShaderConstMtx43 : public TbRenderCommand
{
    TbRenderCommandSetShaderConstMtx43()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_MTX43)
    {}
    TbUint32 shaderBit;
    TbShaderConstHandle* handle;
    TbMatrix43* value;
    TbUint32 count;
};

// 頂点シェーダ定数設定ハンドルから
struct TbRenderCommandSetShaderConstBuffer : public TbRenderCommand
{
    TbRenderCommandSetShaderConstBuffer()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_SHADER_CONST_BUF)
    {}
    TbUint32 shaderBit;
    TbShaderConstHandle* handle;
    const TbConstantBuffer* buffer;
};


// ワールド行列
struct TbRenderCommandSetWorldMatrix : public TbRenderCommand
{
    TbRenderCommandSetWorldMatrix()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_WORLD_MATRIX)
    {}
    TbMatrix44 matrix;
};

// ビュー行列
struct TbRenderCommandSetViewMatrix : public TbRenderCommand
{
    TbRenderCommandSetViewMatrix()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_VIEW_MATRIX)
    {}
    TbMatrix44 matrix;
};

// 射影行列
struct TbRenderCommandSetProjectionMatrix : public TbRenderCommand
{
    TbRenderCommandSetProjectionMatrix()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_PROJECTION_MATRIX)
    {}
    TbMatrix44 matrix;
};

// ビュー行列
struct TbRenderCommandSetViewProjectionMatrix : public TbRenderCommand
{
    TbRenderCommandSetViewProjectionMatrix()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_VIEW_PROJECTION_MATRIX)
    {}
    TbMatrix44 view;
    TbMatrix44 projection;
};

// 射影行列
struct TbRenderCommandSetWorldViewProjectionMatrix : public TbRenderCommand
{
    TbRenderCommandSetWorldViewProjectionMatrix()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_SET_WORLD_VIEW_PROJECTION_MATRIX)
    {}
    TbMatrix44 world;
    TbMatrix44 view;
    TbMatrix44 projection;
};

// 射影行列
struct TbRenderCommandDumpDevice : public TbRenderCommand
{
    TbRenderCommandDumpDevice()
        : TbRenderCommand(TB_RENDER_COMMAND_TYPE_DUMP_DEVICE)
    {}
};

}

#endif
