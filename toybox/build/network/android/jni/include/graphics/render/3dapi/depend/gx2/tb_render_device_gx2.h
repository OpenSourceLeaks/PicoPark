/*!
 * @file
 * @brief TbRenderDeviceの機種依存コード
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_DEVICE_GL_H_
#define _INCLUDED_TB_RENDER_DEVICE_GL_H_

#include <graphics/render/3dapi/tb_color.h>
#include <graphics/render/3dapi/tb_texture_sampler.h>

namespace toybox
{

typedef TbColorF128 TbClearColor;
class TbVertexBuffer;
class TbRenderView;
class TbDepthStencil;

struct TbVertexStreamGL{
    TbVertexBuffer* vertexBuffer;
    TbUint32 offset;
    TbUint32 stride;
};

/*!
 * デバイスの機種依存部分
 */
class TbRenderDeviceDepend
{
public:
    
    enum
    {
        FLAG_CURRENT
    };
    
public:
    
    // コンストラクタ
    TbRenderDeviceDepend();
    
    // デストラクタ
    ~TbRenderDeviceDepend();
    
private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbRenderDeviceDepend);

public:

    // セットアップ
    TbBool Initialize( TbRenderView* view );

    // FBO(Frame Buffer Object)のIDを取得
    GLuint GetFboId() { 
        return m_FboId; 
    }

    // ビュー取得
    TbRenderView* GetRenderView() { 
        return m_RenderView; 
    }

    // 現在の頂点バッファ設定
    void SetVertexStream( TbSint32 stream , TbVertexBuffer* vertexBuffer , TbUint32 offset , TbUint32 stride );

    // 現在の頂点バッファ取得
    TB_INLINE TbVertexStreamGL* GetVertexStream( TbSint32 stream ) { 
        return ( 0 <= stream && stream < TB_VERTEX_STREAM_MAX ) ? &m_VertexStream[ stream ] : nullptr ; 
    }

    // デフォルトの深度ステンシルクラス
    TbDepthStencil* GetDefaultDepthStencil() { 
        return m_DefaultDepthStencil; 
    }

    // デフォルトのテクスチャサンプラ取得
    const TbTextureSampler& GetDefaultTextureSampler() const {
        return m_DefaultTextureSampler;
    }
    
    // アクティブなテクスチャステージの設定
    TbResult SetCurrentTextureStage( TbUint32 stage );

   // アクティブなテクスチャステージ取得
    TbUint32 GetCurrentTextureStage() const;


private:    // 固有変数

    TbBitArray8             m_BitArray;                                //!< ビット配列
    TbRenderView*           m_RenderView;                              //!< ビュー
    TbVertexStreamGL        m_VertexStream[TB_VERTEX_STREAM_MAX];      //!< ストリーム番号別頂点バッファ
    GLuint                  m_FboId;                                   //!< フレームバッファオブジェクト
    TbDepthStencil*         m_DefaultDepthStencil;                     //!< デフォルト深度ステンシル
    TbTextureSampler        m_DefaultTextureSampler;                   //!< デフォルトのテクスチャサンプラ
    TbUint32                m_CurrentTextureStage;                     //!< 現在アクティブなテクスチャステージ

private:

    friend class TbRenderDevice;

};

}

#endif