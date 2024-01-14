/*!
 * @file
 * @brief TbRenderDeviceの機種依存コード(Stage3D用)
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_DEVICE_S3D_H_
#define _INCLUDED_TB_RENDER_DEVICE_S3D_H_

#include <graphics/3dapi/tb_color.h>
#include <graphics/3dapi/tb_texture_sampler.h>

namespace toybox
{

typedef TbColorF128 TbClearColor;
class TbVertexBuffer;
class TbIndexBuffer;
class TbRenderView;
class TbDepthStencil;

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
    
    struct VertexStream {
        TbVertexBuffer* vertexBuffer;
        TbUint32 offset;
        TbUint32 stride;
    };

public:
    
    // コンストラクタ
    TbRenderDeviceDepend();
        
private:

    // コピー代入禁止
    TB_DISALLOW_COPY_AND_ASSIGN(TbRenderDeviceDepend);

public:
    
    // コンテキスト取得
    AS3::ui::flash::display3D::Context3D& GetContext() {
        return s_Context;
    }
    
    // コンテキスト取得
    const AS3::ui::flash::display3D::Context3D& GetContext() const {
        return s_Context;
    }

public: // 
    
    // Stage3d設定
    static void SetStage3d( AS3::ui::flash::display::Stage3D& stage ) {
        s_Stage3d = stage;
    }

    // Context3D設定
    static void SetContext( AS3::ui::flash::display3D::Context3D& context ) {
        s_Context = context;
    }
    
    // 頂点バッファ設定
    void SetVertexStream( TbSint32 stream , TbVertexBuffer* vertexBuffer , TbUint32 offset , TbUint32 stride );

    // 現在の頂点バッファ取得
    TB_INLINE VertexStream* GetVertexStream( TbSint32 stream ) { 
        return ( 0 <= stream && stream < TB_VERTEX_STREAM_MAX ) ? &m_VertexStream[ stream ] : NULL ; 
    }
        
private: // static変数

    static AS3::ui::flash::display::Stage3D     s_Stage3d;
    static AS3::ui::flash::display3D::Context3D s_Context; 

private: // 固有変数

    TbBitArray8     m_BitArray;                             //!< ビット配列
    VertexStream    m_VertexStream[TB_VERTEX_STREAM_MAX];   //!< ストリーム番号別頂点バッファ
    TbIndexBuffer*  m_IndexBuffer;                          //!< インデックスバッファ
    TbSizeT         m_IndexBufferIndex;                     //!< インデックバッファの現在のインデックス
    
private:

    friend class TbRenderDevice;

};

}

#endif