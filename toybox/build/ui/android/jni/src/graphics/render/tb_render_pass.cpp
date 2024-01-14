/*!
 * レンダリングパス
 * @author Miyake Shunsuke
 * @since 2011.09.01
 */

#include <tb_fwd.h>
#include <graphics/render/tb_render_pass.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbRenderPass::TbRenderPass()
{
    SetEnable(TB_TRUE);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbRenderPass::~TbRenderPass()
{
}

/*!
 * コマンドをつめるようにする
 * @author Miyake Shunsuke
 * @since 2013.02.28
 */
void TbRenderPass::begin()
{
    // デフォルト設定
    if( !m_BitArray.Test(FLAG_ENABLE_CALL_COMMAND) ){
        if( m_BeginCallback.IsValid() ){
            m_BeginCallback(*this);
        }
    }
    m_BitArray.SetBit(FLAG_ENABLE_CALL_COMMAND,TB_TRUE);
}

/*!
 * コマンドを実際にGPUに転送
 * @author Miyake Shunsuke
 * @since 2013.02.28
 */
void TbRenderPass::end()
{
    m_BitArray.SetBit(FLAG_ENABLE_CALL_COMMAND,TB_FALSE);
}

/*!
 * コマンドを実際にGPUに転送
 * @author Miyake Shunsuke
 * @since 2013.02.28
 */
void TbRenderPass::flush()
{
    if(IsEnable()){
        m_CommandBuffer.Flush();
    }
    m_CommandBuffer.ClearCommand();
}

/*!
 * レンダーターゲット設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget )
{
    return m_CommandBuffer.SetRenderTarget(index,renderTarget);
}

/*!
 * 深度ステンシル設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetDepthStencil( TbDepthStencil* depthStencil )
{
    return m_CommandBuffer.SetDepthStencil(depthStencil);
}

/*!
 * ストリームソース設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetVertexBuffer( TbUint32 streamNumber , TbVertexBuffer* vertexBuffer , TbUint32 offsetInBytes , TbUint32 stride )
{
    return m_CommandBuffer.SetVertexBuffer(streamNumber,vertexBuffer,offsetInBytes,stride);
}

/*!
 * 頂点インデックスバッファ設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetIndexBuffer( TbIndexBuffer* indexBuffer )
{
    return m_CommandBuffer.SetIndexBuffer(indexBuffer);
}

/*!
 * プリミティブ描画
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass:: DrawPrimitive( const TbPrimitiveType primitiveType , 
                                        const TbUint32 startVertex , 
                                        const TbUint32 primitiveCount )
{
    return m_CommandBuffer.DrawPrimitive(primitiveType,startVertex,primitiveCount);
}

/*!
 * プリミティブ描画( 生の頂点配列使用 )
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::DrawPrimitive( const TbPrimitiveType primitiveType , 
                                        const TbUint32 primitiveCount , 
                                        const void* vertexs , 
                                        const TbUint32 stride )
{
    return m_CommandBuffer.DrawPrimitive(primitiveType,primitiveCount,vertexs,stride);
}

/*!
 * 頂点インデックスを利用したプリミティブ描画
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                                const TbUint32 minVertexIndex ,
                                                const TbUint32 vertexCount , 
                                                const TbUint32 startIndex , 
                                                const TbUint32 primitiveCount )
{
    return m_CommandBuffer.DrawIndexedPrimitive(primitiveType,
                                                minVertexIndex,
                                                vertexCount,
                                                startIndex,
                                                primitiveCount);
}

/*!
 * 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                const TbUint32 minVertexIndex ,
                                const TbUint32 vertexCount , 
                                const void* indexData , 
                                const TbIndexBufferFormat indexFormat , 
                                const void* vertexs , 
                                TbUint32 stride ,
                                const TbUint32 primitiveCount )
{
    return m_CommandBuffer.DrawIndexedPrimitive(primitiveType,
                                                minVertexIndex,
                                                vertexCount,
                                                indexData,
                                                indexFormat,
                                                vertexs,
                                                stride,
                                                primitiveCount);
}

/*!
 * テクスチャ設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType )
{
    return m_CommandBuffer.SetTexture(stage,texture,shaderType);
}

/*!
 * テクスチャサンプラーセット
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType )
{
    return m_CommandBuffer.SetTextureSampler(slot,sampler,shaderType);
}

/*!
* ビューポート設定
* @author Miyake Shunsuke
* @since 2013.02.27
*/
TbResult TbRenderPass::SetViewportScissorState(const TbViewportScissorState* state)
{
    return m_CommandBuffer.SetViewportScissorState(state);
}

/*!
 * フィルモード設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetRasterizerState(const TbRasterizerState* state)
{
    return m_CommandBuffer.SetRasterizerState(state);
}

/*!
 * ブレンド係数
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetBlendState( const TbBlendState* state )
{
    return m_CommandBuffer.SetBlendState(state);
}

/*!
 * アルファテスト有効設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetEnableAlphaTest( TbBool isEnable )
{
    return m_CommandBuffer.SetEnableAlphaTest(isEnable);
}

/*!
 * アルファ比較関数
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetAlphaTestFunc( TbCmpFunc func )
{
    return m_CommandBuffer.SetAlphaTestFunc(func);
}

/*!
 * アルファ参照値
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetAlphaTestRef( TbUint8 ref )
{
    return m_CommandBuffer.SetAlphaTestRef(ref);
}

/*!
 * 深度ステンシルステート設定
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbResult TbRenderPass::SetDepthStencilState( const TbDepthStencilState* state )
{
    return m_CommandBuffer.SetDepthStencilState(state);
}

}
