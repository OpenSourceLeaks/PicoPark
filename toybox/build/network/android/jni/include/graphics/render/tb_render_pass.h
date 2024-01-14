/*!
 * レンダリングパス
 * @author Miyake Shunsuke
 * @since 2011.09.01
 */

#ifndef _INCLUDED_TB_RENDER_PASS_H_
#define _INCLUDED_TB_RENDER_PASS_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/tb_render_command_buffer.h>

namespace toybox
{

class TbRenderTarget;
class TbDepthStencil;
class TbRenderPassManager;
class TbRasterizerState;
class TbViewportScissorState;
class TbBlendState;
class TbDepthStencilState;

class TbRenderPass
{
    friend class TbRenderPassManager;
    typedef TbStaticFunction32<void (TbRenderPass& pass)> BeginFunc;
public:

    // コンストラクタ
    TbRenderPass();

    // デストラクタ
    virtual ~TbRenderPass();

public:

    // コマンドバッファ用アロケータ設定
    void SetRenderCommandAllocator( TbRenderCommandAllocator* allocator ){
        m_CommandBuffer.SetAllocator(allocator);
    }

    // 先頭にコマンドを積むためのコールバック関数
    void SetBeginCallback( const BeginFunc& func ) {
        m_BeginCallback = func;
    }

    // 利用可能
    void SetEnable( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE,isEnable);
    }

    // 利用可能か
    TbBool IsEnable() const {
        return m_BitArray.Test(FLAG_ENABLE);
    }

public:

    //! 明示的にバッファをクリア
    TbResult Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil ) {
        return m_CommandBuffer.Clear(clearTargetBit,color,depth,stencil);
    }

    // レンダーターゲット設定
    TbResult SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget );

    // 深度ステンシル設定
    TbResult SetDepthStencil( TbDepthStencil* depthStencil );    
    
public: // 描画
    
    // ストリームソース設定
    TbResult SetVertexBuffer( TbUint32 streamNumber , TbVertexBuffer* vertexBuffer , TbUint32 offsetInBytes , TbUint32 stride );

    // 頂点インデックスバッファ設定
    TbResult SetIndexBuffer( TbIndexBuffer* indexBuffer );

    // プリミティブ描画
    TbResult DrawPrimitive( const TbPrimitiveType primitiveType , 
                            const TbUint32 startVertex , 
                            const TbUint32 primitiveCount );

    // プリミティブ描画( 生の頂点配列使用 )
    TbResult DrawPrimitive( const TbPrimitiveType primitiveType , 
                            const TbUint32 primitiveCount , 
                            const void* vertexs , 
                            const TbUint32 stride );

    // 頂点インデックスを利用したプリミティブ描画
    TbResult DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                   const TbUint32 minVertexIndex ,
                                   const TbUint32 vertexCount , 
                                   const TbUint32 startIndex , 
                                   const TbUint32 primitiveCount );

    // 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
    TbResult DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                   const TbUint32 minVertexIndex ,
                                   const TbUint32 vertexCount , 
                                   const void* indexData , 
                                   const TbIndexBufferFormat indexFormat , 
                                   const void* vertexs , 
                                   TbUint32 stride ,
                                   const TbUint32 primitiveCount );

public: // テクスチャ設定

    // テクスチャ設定
    TbResult SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType = TB_SHADER_TYPE_PIXEL );

    // テクスチャサンプラーセット
    TbResult SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType = TB_SHADER_TYPE_PIXEL );

public: // ビューポートシザー

    // ビューポートシザーステート設定
    TbResult SetViewportScissorState(const TbViewportScissorState* state);

public: // ラスタライズ設定

    // ラスタライザ設定
    TbResult SetRasterizerState( const TbRasterizerState* state );

    // 深度ステンシルステート設定
    TbResult SetRasterizerState(TbRenderStatePreset::RasterizerType type) {
        return SetRasterizerState(TbRenderStatePreset::GetInstance().GetRasterizerState(type));
    }

public: // ブレンド設定

    // ブレンドステート設定
    TbResult SetBlendState( const TbBlendState* state );

    // 深度ステンシルステート設定
    TbResult SetBlendState(TbRenderStatePreset::BlendType type) {
        return SetBlendState(TbRenderStatePreset::GetInstance().GetBlendState(type));
    }

public: // アルファテスト(DirectX10以降廃止されたし必要ないかも・・・)

    // アルファテスト有効設定
    TbResult SetEnableAlphaTest( TbBool isEnable );

    // アルファ比較関数
    TbResult SetAlphaTestFunc( TbCmpFunc func );

    // アルファ参照値
    TbResult SetAlphaTestRef( TbUint8 ref );

public: // 深度ステンシルステート設定

    // 深度ステンシルステート設定
    TbResult SetDepthStencilState( const TbDepthStencilState* state );
    
    // 深度ステンシルステート設定
    TbResult SetDepthStencilState(TbRenderStatePreset::DepthStencilType type) {
        return SetDepthStencilState(TbRenderStatePreset::GetInstance().GetDepthStencilState(type));
    }

public: // 固定機能シェーダ設定

    // 固定機能シェーダ有効
    TbResult SetFixedShader() {
        return m_CommandBuffer.SetFixedShader();
    }

    // 頂点フォーマット設定
    TbResult SetFixedShaderVertexFormat( const TbVertexFormat* vertexFormat ) {
        return m_CommandBuffer.SetFixedShaderVertexFormat(vertexFormat);
    }

    // シェーディングモード設定
    TbResult SetFixedShaderShadeMode( TbFixedShaderShadeMode shadeMode ) {
        return m_CommandBuffer.SetFixedShaderShadeMode(shadeMode);
    }

    // ライト計算有効設定
    TbResult SetFixedShaderEnableLighting( TbBool isEnable ) {
        return m_CommandBuffer.SetFixedShaderEnableLighting(isEnable);
    }

    // ライト有効設定
    TbResult SetFixedShaderEnableLight( TbUint32 index , TbBool isEnable ){
        return m_CommandBuffer.SetFixedShaderEnableLight(index,isEnable);
    }

    // ライト設定
    TbResult SetFixedShaderLight( TbUint32 index , const TbFixedLight* light ){
        return m_CommandBuffer.SetFixedShaderLight(index,light);
    }

    // マテリアル設定
    TbResult SetFixedShaderMaterial( const TbFixedMaterial* material ){
        return m_CommandBuffer.SetFixedShaderMaterial(material);
    }

    // テクスチャ合成方法設定( 入力RGB )
    TbResult SetFixedShaderTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg ) {
        return m_CommandBuffer.SetFixedShaderTextureColorArg(stage,argIndex,arg);
    }

    // テクスチャ合成方法設定( RGB演算方式 )
    TbResult SetFixedShaderTextureColorOp( TbUint32 stage , TbTextureOp op ) {
        return m_CommandBuffer.SetFixedShaderTextureColorOp(stage,op);
    }

    // テクスチャ合成方法設定( 入力アルファ )
    TbResult SetFixedShaderTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg ){
        return m_CommandBuffer.SetFixedShaderTextureAlphaArg(stage,argIndex,arg);
    }


    // テクスチャ合成方法設定( アルファ演算方式 )
    TbResult SetFixedShaderTextureAlphaOp( TbUint32 stage , TbTextureOp op ) {
        return m_CommandBuffer.SetFixedShaderTextureAlphaOp(stage,op);
    }

public: // シェーダ設定

    // シェーダバインド
    TbResult SetShaderProgram( TbShaderProgram* program ) {
        return m_CommandBuffer.SetShaderProgram(program);
    }

    // シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantBool( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbBool* value ,TbUint32 count ){
        return m_CommandBuffer.SetShaderConstantBool(shaderBit,handle,value,count);
    }

    // シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantF( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbFloat32* value , TbUint32 count ){
       return  m_CommandBuffer.SetShaderConstantF32(shaderBit,handle,value,count);
    }

    // シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantI( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbSint32* value , TbUint32 count ) {
        return m_CommandBuffer.SetShaderConstantS32(shaderBit,handle,value,count);
    }

    // シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbMatrix44* value , TbUint32 count ) {
        return m_CommandBuffer.SetShaderConstantMatrix(shaderBit,handle,value,count);
    }

    // シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantBuffer( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbConstantBuffer* buffer ) {
        return m_CommandBuffer.SetShaderConstantBuffer(shaderBit,handle, buffer);
    }

    // シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantMatrix(TbUint32 shaderBit, TbShaderConstHandle* handle, const TbMatrix43* value, TbUint32 count) {
        return m_CommandBuffer.SetShaderConstantMatrix(shaderBit, handle, value, count);
    }

    // ワールド行列
    TbResult SetWorldMatrix( const TbMatrix44& matrix ) {
        return m_CommandBuffer.SetWorldMatrix(matrix);
    }

    // ビュー行列
    TbResult SetViewMatrix( const TbMatrix44& matrix ) {
        return m_CommandBuffer.SetViewMatrix(matrix);
    }

    // 射影行列
    TbResult SetProjectionMatrix( const TbMatrix44& matrix ) {
        return m_CommandBuffer.SetProjectionMatrix(matrix);
    }

    // View Projection行列
    TbResult SetViewProjectionMatrix( const TbMatrix44& view , const TbMatrix44& projection ) {
        return m_CommandBuffer.SetViewProjectionMatrix(view,projection);
    }

    // World View Projection行列
    TbResult SetWorldViewProjectionMatrix( const TbMatrix44& world , const TbMatrix44& view , const TbMatrix44& projection ) {
        return m_CommandBuffer.SetWorldViewProjectionMatrix(world,view,projection);
    }

public: //デバッグ用

    // デバイス状態をダンプ
    TbResult DumpDevice() {
        return m_CommandBuffer.DumpDevice();
    }


private:

    enum Flag
    {
        FLAG_ENABLE_CALL_COMMAND , // コマンド発行可能
        FLAG_ENABLE ,
        FLAG_MAX ,
    };

private:

    // コマンドをつめるようにする
    void begin();

    // コマンド発行終了
    void end();

    // コマンドGPU転送
    void flush();

private:

    TbBitArray32            m_BitArray;
    BeginFunc               m_BeginCallback;
    TbRenderCommandBuffer   m_CommandBuffer;    //!< コマンドバッファ

};

};

#endif
