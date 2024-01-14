/*!
 * @file
 * @brief 描画コマンドバッファ
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_COMMAND_BUFFER_H_
#define _INCLUDED_TB_RENDER_COMMAND_BUFFER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>
#include <graphics/render/3dapi/tb_shader_manager.h>
#include <graphics/display/tb_display_types.h>
#include <base/math/tb_geometry.h>
#include <base/util/tb_singleton.h>
#include <graphics/render/tb_render_command_types.h>

namespace toybox
{
    
class TbRenderCommandAllocator
{
public:
    
    // コンストラクタ
    TbRenderCommandAllocator();
    
    // デストラクタ
    ~TbRenderCommandAllocator();
    
private:

    // コピー禁止
    TB_DISALLOW_COPY_AND_ASSIGN(TbRenderCommandAllocator);

public:
    
    // 初期化
    void Initialize( TbSizeT size );

    // 初期化
    void Initialize( void* buffer , TbSizeT size );

    // サイズ取得
    TbSizeT GetSize() const {
        return m_Size;
    }
    
    // 使用サイズ取得
    TbSizeT GetUsedSize() const {
        return m_Top;
    }

public:
    
    // アロケート
    void* Allocate( TbSizeT size );
    
    // クリア
    void Clear();
    
private:
    
    enum Flag
    {
        FLAG_INITIALIZE , 
        FLAG_MAX , 
    };

private:
    TbUint8* m_Buffer;
    TbSizeT m_Top;
    TbSizeT m_Size;
};

/*! 
 * 描画デバイス
 */
class TbRenderCommandBuffer
{
public:

    // コンストラクタ
    TbRenderCommandBuffer();

    // デストラクタ
    ~TbRenderCommandBuffer();

private:

    // コピー禁止
    TB_DISALLOW_COPY_AND_ASSIGN(TbRenderCommandBuffer);

public:

    // 実際に描画コマンドを発行する
    void Flush();

public:

    // コマンド追加可能
    void SetEnable( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_ADD,isEnable);
    }

    // アロケータ設定
    void SetAllocator( TbRenderCommandAllocator* allocator ) {
        if( TB_VERIFY(!m_Allocator) ){
            // 多重登録禁止
            m_Allocator = allocator;
        }
    }

public: // バッファ

    //! 明示的にバッファをクリア
    TbResult Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil );

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

public: // ビューポートシザー設定

    TbResult SetViewportScissorState(const TbViewportScissorState* state);

public: // ラスタライズ設定

    // 設定
    TbResult SetRasterizerState(const TbRasterizerState* state);

public: // ブレンド設定

    // ブレンド設定
    TbResult SetBlendState(const TbBlendState* state);

public: // アルファテスト(DirectX10以降廃止されたし必要ないかも・・・)

    // アルファテスト有効設定
    TbResult SetEnableAlphaTest( TbBool isEnable );

    // アルファ比較関数
    TbResult SetAlphaTestFunc( TbCmpFunc func );

    // アルファ参照値
    TbResult SetAlphaTestRef( TbUint8 ref );

public: // 深度ステンシル設定

    // 深度ステンシル設定
    TbResult SetDepthStencilState( const TbDepthStencilState* state );

public: // 固定機能シェーダ設定

    // シェーダ設定
    TbResult SetFixedShader() {
        SetShaderProgram(nullptr);
        return TB_S_OK;
    }

    // 頂点フォーマット設定
    TbResult SetFixedShaderVertexFormat( const TbVertexFormat* vertexFormat );

    // シェーディングモード設定
    TbResult SetFixedShaderShadeMode( TbFixedShaderShadeMode shadeMode );

    // ライト計算有効設定
    TbResult SetFixedShaderEnableLighting( TbBool isEnable );

    // ライト有効設定
    TbResult SetFixedShaderEnableLight( TbUint32 index , TbBool isEnable );

    // ライト設定
    TbResult SetFixedShaderLight( TbUint32 index , const TbFixedLight* light );

    // マテリアル設定
    TbResult SetFixedShaderMaterial( const TbFixedMaterial* material );

    // テクスチャ合成方法設定( 入力RGB )
    TbResult SetFixedShaderTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg );

    // テクスチャ合成方法設定( RGB演算方式 )
    TbResult SetFixedShaderTextureColorOp( TbUint32 stage , TbTextureOp op );

    // テクスチャ合成方法設定( 入力アルファ )
    TbResult SetFixedShaderTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg );

    // テクスチャ合成方法設定( アルファ演算方式 )
    TbResult SetFixedShaderTextureAlphaOp( TbUint32 stage , TbTextureOp op );

public: // シェーダ設定

    // シェーダ設定
    TbResult SetShaderProgram( TbShaderProgram* program );

    // 頂点シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantBool( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbBool* value ,TbUint32 count );

    // 頂点シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantF32( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbFloat32* value , TbUint32 count );

    // 頂点シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantS32( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbSint32* value , TbUint32 count );

    // 頂点シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbMatrix44* value , TbUint32 count );

    // 頂点シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle* handle , const TbMatrix43* value , TbUint32 count );

    // 頂点シェーダ定数設定( ハンドルから )
    TbResult SetShaderConstantBuffer(TbUint32 shaderBit, TbShaderConstHandle* handle, const TbConstantBuffer* buffer );

public: // 行列

    // ワールド行列
    TbResult SetWorldMatrix( const TbMatrix44& matrix );

    // ビュー行列
    TbResult SetViewMatrix( const TbMatrix44& matrix );

    // 射影行列
    TbResult SetProjectionMatrix( const TbMatrix44& matrix );

    // View Projection行列
    TbResult SetViewProjectionMatrix( const TbMatrix44& view , const TbMatrix44& projection );

    // World View Projection行列
    TbResult SetWorldViewProjectionMatrix( const TbMatrix44& world , const TbMatrix44& view , const TbMatrix44& projection );

public: // デバッグ用コマンド

    // デバイス状態をダンプする
    TbResult DumpDevice();

public: //

    // 先頭コマンド取得
    TbRenderCommand* GetCommandTop() {
        return m_Top;
    }

    // 末尾コマンド取得
    TbRenderCommand* GetCommandTail() {
        return m_Tail;
    }

    // コマンドクリア
    void ClearCommand() {
        m_Top = nullptr;
        m_Tail = nullptr;
        m_Cache.Clear();
    }

private:

    enum Flag
    {
        FLAG_ENABLE_ADD , // コマンド追加可能
        FLAG_MAX
    };

    // キャッシュ(余計なコマンドが詰まれないようにするためのキャッシュ
    struct Cache
    {
        Cache() 
            : shaderProgram(nullptr)
        {}

        void Clear()
        {
            bitArray.Clear();
            shaderProgram = nullptr;
            indexBuffer = nullptr;
        }

        enum Flag
        {
            FLAG_SHADER_PROGRAM ,
            FLAG_INDEX_BUFFER
        };

        TbBitArray32     bitArray;
        TbShaderProgram* shaderProgram;
        TbIndexBuffer*   indexBuffer;
    };
    
private:
    
    //! コマンド追加
    TbResult addCommand( TbRenderCommand* command ) {
        if( !command ) {
            return TB_E_FAIL;
        }
        if( !TB_VERIFY(m_BitArray.Test(FLAG_ENABLE_ADD)) ){
            return TB_E_FAIL;
        }
        if( !m_Top ) {
            m_Top = command;
            m_Tail = command;
        }else{
            m_Tail->next = command;
            m_Tail = command;
        }
        return TB_S_OK;
    }

private:    // メンバ変数

    TbBitArray32                m_BitArray;
    Cache                       m_Cache;
    TbRenderCommandAllocator*   m_Allocator;
    TbRenderCommand*            m_Top;
    TbRenderCommand*            m_Tail;

};

}

#endif
