/*!
 * @file
 * @brief 描画デバイス
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_DEVICE_H_
#define _INCLUDED_TB_RENDER_DEVICE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_render_parameter.h>
#include <graphics/render/3dapi/tb_render_device_cache.h>
#include <graphics/render/3dapi/tb_render_state_preset.h>
#include <graphics/render/3dapi/tb_color.h>
#include <graphics/render/3dapi/tb_shader_manager.h>
#include <graphics/render/3dapi/tb_depth_stencil_state.h>
#include <graphics/render/3dapi/tb_blend_state.h>
#include <graphics/render/3dapi/tb_rasterizer_state.h>
#include <graphics/render/3dapi/tb_viewport_scissor_state.h>
#include <graphics/render/3dapi/tb_texture_sampler.h>
#include <graphics/display/tb_display_types.h>
#include <base/math/tb_geometry.h>
#include <base/container/tb_array.h>
#include <graphics/render/3dapi/tb_fixed_shader.h>
#include <base/util/tb_singleton.h>

// 依存コード
#include "./depend/tb_render_device_depend.h"

namespace toybox
{

class TbRenderView;
class TbRenderTarget;
struct TbRenderViewParameter;
class TbVertexBuffer;
class TbVertexFormat;
class TbIndexBuffer;
class TbWindow;
class TbTexture;
class TbTextureSampler;
class TbDepthStencil;

/*! 
 * 描画デバイス
 */
class TbRenderDevice : public TbSingletonRef<TbRenderDevice>
{
public:

    static const TbUint32 TEXTURE_SAMPLER_SLOT_MAX = 4; // テクスチャサンプラスロット最大数
    static const TbSint32 TEXTURE_STAGE_MAX = 4;        // マルチテクスチャステージ最大数
    static const TbSint32 RENDER_TARGET_MAX = 8;        // マルチレンダーターゲット最大数

    struct AlphaTestState
    {
        TbBool    isEnableAlphaTest;
        TbCmpFunc alphaTestFunc;
        TbUint8   alphaTestRef;
    };
    
    struct Caps
    {
        TbUint32 shaderConstMax[TB_SHADER_TYPE_MAX]; // 各シェーダの定数レジスタ最大数
    };

public:

    // 表示用矩形領域取得
    static TbRectS32 GetPresentRect( TbRenderPresentType type , const TbRenderTarget* rt , const TbWindow* window );

public:

    // コンストラクタ
    explicit TbRenderDevice( TbSizeT viewMax );

    // デストラクタ
    ~TbRenderDevice();

private:

    // コピー禁止
    TB_DISALLOW_COPY_AND_ASSIGN(TbRenderDevice);

public:

    // 初期化
    TbBool Initialize( TbUint32 adapterIndex , TbWindow* focusWindow , TbUint32 flags , const TbRenderViewParameter& parameter );

    // 初期化(簡易版)
    TbBool Initialize( TbWindow* focusWindow , bool isEnableDepthStencil , bool isFullscreen );

    // 初期化済みか
    TbBool IsInitialized() const {
        return m_BitArray.Test(FLAG_INITIALIZE);
    }

public: // GPU情報

    // 性能チェック
    void GetCaps( Caps& caps );

public: // ディスプレイ

    // ディスプレイアダプタ番号取得
    TbUint32 GetAdapterIndex() const { 
        return m_AdapterIndex; 
    }

    // 対応しているディスプレイのグラフィックスモードを取得
    TbBool GetCurrentDisplayMode( TbUint32 index , TbDisplayMode& outDisplayMode ) const;

    // 対応しているディスプレイのグラフィックスモードを取得
    TbBool GetDisplayMode( TbUint32 index , TbSint32 mode , TbDisplayMode& outDisplayMode ) const;

    // 対応しているディスプレイのグラフィックスモードを取得
    TbBool GetDisplayMode( TbUint32 index , TbSint32 mode , TbColorFormat format , TbDisplayMode& outDisplayMode ) const;

    // 対応しているディスプレイのグラフィックスモード数を取得
    TbSint32 GetDisplayModeNum( TbUint32 index ) const;

    // 対応しているディスプレイのグラフィックスモード数を取得(フォーマット指定)
    TbSint32 GetDisplayModeNum( TbUint32 index , TbColorFormat format ) const;

public: // ウィンドウ

    // 対応ウィンドウ取得
    TbWindow* GetWindow() { 
        return m_Window; 
    }

    // 対応ウィンドウ取得
    const TbWindow* GetWindow() const { 
        return m_Window; 
    }

    // フルスクリーン設定
    TbBool SetFullscreen( TbBool isFullscreen );

    // フルスクリーンかどうか
    TbBool IsFullscreen() const { 
        return m_BitArray.Test( FLAG_FULLSCREEN ); 
    }

public: // ビュー

    // ビュー生成
    TbRenderView* CreateView( const TbRenderViewParameter& parameter );

    // ビュー取得
    TbRenderView* GetView( TbSint32 index );

    // 生成済みビュー数取得
    TbUint32 GetViewCount() const { 
        return m_ViewList.GetCount(); 
    }

public: // バッファ

    //! 明示的にバッファをクリア(第一引数はTbRenderBufferTypeBitの論理和)
    TbResult Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil );

    // レンダーターゲット設定
    TbResult SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget );

    // レンダーターゲット設定(深度ステンシルバッファと同時設定
    TbResult SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget , TbDepthStencil* depthStencil );

    // レンダーターゲット取得
    TbRenderTarget* GetRenderTarget( TbUint32 index )
    {
        if( index < RENDER_TARGET_MAX ){
            return m_RenderTarget[ index ];
        }
        return nullptr;
    }

    // レンダーターゲット取得
    const TbRenderTarget* GetRenderTarget( TbUint32 index ) const 
    {
        if( index < RENDER_TARGET_MAX ){
            return m_RenderTarget[ index ];
        }
        return nullptr;
    }

    // 深度ステンシル設定
    TbResult SetDepthStencil( TbDepthStencil* depthStencil );

    // 深度ステンシル取得
    TbDepthStencil* GetDepthStencil() { 
        return m_DepthStencil; 
    }

    // 深度ステンシル取得
    const TbDepthStencil* GetDepthStencil() const { 
        return m_DepthStencil; 
    }

    // 現在設定されている深度ステンシルとレンダーターゲットの組み合わせは有効か
    TbBool IsValidRenderTargetAndDepthStencil() const;
    
public: // 描画系

    // 開始
    void BeginScene();

    // 終了
    void EndScene();

    // 画面に表示
    void PresentAll();

    // 画面表示方法を設定
    void SetPresentType( TbRenderPresentType type ) {
        m_PresentType = type;
    }

    // 画面表示方法取得
    TbRenderPresentType GetPresentType() const {
        return m_PresentType;
    }

    // ストリームソース設定
    void SetVertexBuffer( TbUint32 streamNumber , TbVertexBuffer* vertexBuffer , TbUint32 offsetInBytes , TbUint32 stride );

    // 頂点インデックスバッファ設定
    void SetIndexBuffer( TbIndexBuffer* indexBuffer );

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

    // 現在割り当てられているテクスチャ数
    TbUint32 GetActiveTextureCount() const { return 
        m_ActiveTextureCount; 
    }

    // テクスチャ設定
    TbBool SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType = TB_SHADER_TYPE_PIXEL );

    // 使用中テクスチャ取得
    TbTexture*  GetTexture( TbSint32 stage , TbShaderType shaderType = TB_SHADER_TYPE_PIXEL );

    // テクスチャサンプラーセット
    TbResult SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType = TB_SHADER_TYPE_PIXEL );

    // テクスチャサンプラー取得
    TbTextureSampler* GetTextureSampler( TbUint32 slot , TbShaderType shaderType = TB_SHADER_TYPE_PIXEL );

public: // ビューポートシザー

    // ビューポート設定
    TbResult SetViewportScissorState( const TbViewportScissorState* state );

    // ビューポート取得
    TbResult GetViewport(TbRectS32* rect) const;

public: // ラスタライズ設定

    // ラスタライザステート設定
    TbResult SetRasterizerState(const TbRasterizerState* state);

public: // ブレンド設定

    // ブレンドステート設定
    TbResult SetBlendState(const TbBlendState* state);

public: // 深度ステンシル設定

    // 深度テストを有効設定
    TbResult SetDepthStencilState( const TbDepthStencilState* state );

public: // アルファテスト(DirectX10以降廃止されたし必要ないかも・・・)

        // アルファテスト有効設定
    TbResult SetEnableAlphaTest(TbBool isEnable);

    // アルファ比較関数
    TbResult SetAlphaTestFunc(TbCmpFunc func);

    // アルファ参照値
    TbResult SetAlphaTestRef(TbUint8 ref);

public: // シェーダ

    // シェーダコンテキスト取得
    TbShaderManager& GetShaderManager(){ 
        return m_ShaderManager; 
    }

    // シェーダコンテキスト取得
    const TbShaderManager& GetShaderManager() const { 
        return m_ShaderManager;
    }

    // 固定シェーダ取得
    TbFixedShader& GetFixedShader() { 
        return m_FixedShader; 
    }

    // 固定シェーダ取得
    const TbFixedShader& GetFixedShader() const { 
        return m_FixedShader;
    }

public: // シェーダ自動分岐処理

    // ワールド行列
    void SetWorldMatrix( const TbMatrix44& matrix ) {
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_WORLD,matrix);
        m_ShaderManager.SetWorldMatrix(matrix);
#if 0
        if( isActiveFixedShader() ){
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_WORLD,matrix);
        }else{
            m_ShaderManager.SetWorldMatrix(matrix);
        }
#endif
    }

    // ビュー行列
    void SetViewMatrix( const TbMatrix44& matrix ) {
        // todo最適化
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_VIEW,matrix);
        m_ShaderManager.SetViewMatrix(matrix);
#if 0
        if( isActiveFixedShader() ){
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_VIEW,matrix);
        }else{
            m_ShaderManager.SetViewMatrix(matrix);
        }
#endif
    }

    // 射影行列
    void SetProjectionMatrix( const TbMatrix44& matrix ) {
        // todo最適化
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_PROJECTION,matrix);
        m_ShaderManager.SetProjectionMatrix(matrix);
#if 0
        if( isActiveFixedShader() ){
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_PROJECTION,matrix);
        }else{
            m_ShaderManager.SetProjectionMatrix(matrix);
        }
#endif
    }

    // View Projection行列
    void SetViewProjectionMatrix( const TbMatrix44& view , const TbMatrix44& projection ) {
        // todo最適化
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_VIEW,view);
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_PROJECTION,projection);
        m_ShaderManager.SetViewProjectionMatrix(view,projection);
#if 0
        if( isActiveFixedShader() ){
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_VIEW,view);
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_PROJECTION,projection);
        }else{
            m_ShaderManager.SetViewProjectionMatrix(view,projection);
        }
#endif
    }

    // World View Projection行列
    void SetWorldViewProjectionMatrix( const TbMatrix44& world , const TbMatrix44& view , const TbMatrix44& projection ) {
        // todo最適化
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_WORLD,world);
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_VIEW,view);
        m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_PROJECTION,projection);
        m_ShaderManager.SetWorldViewProjectionMatrix(world,view,projection);
#if 0
        if( isActiveFixedShader() ){
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_WORLD,world);
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_VIEW,view);
            m_FixedShader.SetMatrix(TB_FIXED_SHADER_MATRIX_PROJECTION,projection);
        }else{
            m_ShaderManager.SetWorldViewProjectionMatrix(world,view,projection);
        }
#endif
    }

public: // デバッグ

    // 状態をダンプ
    void Dump();

public: // 依存コード

    // 依存コード取得
    TB_INLINE TbRenderDeviceDepend& GetDepend() { 
        return m_Depend; 
    }

    // 依存コードconst取得
    TB_INLINE const TbRenderDeviceDepend& GetDepend() const { 
        return m_Depend; 
    }

private:

    enum {
        FLAG_INITIALIZE , 
        FLAG_FULLSCREEN,
        FLAG_UPDATE_MVP , 
    };

    enum {
        MATRIX_WORLD , 
        MATRIX_VIEW , 
        MATRIX_PROJECTION , 
        MATRIX_MAX
    };

private:

    // 固定機能シェーダが有効かどうか
    TbBool isActiveFixedShader() const;

    // 共通初期化
    void initCommon();

    // ステート系初期化
    void initDefaultState(const TbRenderViewParameter& param);

    // アルファテストステートの初期化
    void initAlphaTestState();

private:    // 汎用変数

    TbWindow*                    m_Window;                  //!< 対応ウィンドウ
    TbBitArray32                 m_BitArray;                //!< ビット配列
    TbUint32                     m_AdapterIndex;            //!< ディスプレイアダプタインデックス
    TbShaderManager              m_ShaderManager;           //!< シェーダ
    TbFixedShader                m_FixedShader;             //!< 固定シェーダ
    TbRenderPresentType          m_PresentType;             //!< 表示タイプ
    TbArray<TbRenderView*>       m_ViewList;                //!< ビューリスト
    TbIndexBuffer*               m_IndexBuffer;             //!< 設定されているインデックスバッファ
    TbUint32                     m_ActiveTextureCount;      //!< 設定されているテクスチャ数
    TbTexture*                   m_Texture[TEXTURE_STAGE_MAX];               //!< 設定されているテクスチャ
    TbTextureSampler*            m_TextureSampler[TEXTURE_SAMPLER_SLOT_MAX]; //!< テクスチャ用サンプラ
    TbRenderTarget*              m_RenderTarget[RENDER_TARGET_MAX];          //!< 現在設定されている
    TbDepthStencil*              m_DepthStencil;            //!< 現在設定されている深度ステンシル
    const TbViewportScissorState* m_ViewportSciessorState;   //!< ビューポートシザーステート
    const TbRasterizerState*     m_RasterizerState;         //!> ラスタライザステート
    const TbBlendState*          m_BlendState;              //!< ブレンドステート
    const TbDepthStencilState*   m_DepthStencilState;       //!< 深度ステンシルステート
    AlphaTestState               m_AlphaTestState;          //!< アルファテストステート
    TbMatrix                     m_Matrix[MATRIX_MAX];      //!< モデルビュー射影行列
    TbRenderDeviceCache          m_Cache;
    TbRenderDeviceDepend         m_Depend;                  //!< 依存データ

private:

    TbRenderStatePreset          m_RenderStatePreset;
    TbTextureSampler             m_DefaultTextureSampler;          //!< デフォルトテクスチャサンプラ
    TbViewportScissorState       m_DefaultViewportScissorState;   //!< ビューポートシザーステート
    TbRasterizerState            m_DefaultRasterizerState;         //!> ラスタライザステート
    TbBlendState                 m_DefaultBlendState;              //!< ブレンドステート
    TbDepthStencilState          m_DefaultDepthStencilState;       //!< 深度ステンシルステート
    
private:

    friend class TbRenderDeviceDepend;
};

}

#endif
