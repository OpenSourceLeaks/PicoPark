/*!
 * 3DAPI定義
 * @author Miyake Shunsuke
 * @since 2011.07.03
 */

#ifndef _INCLUDED_TB_3DAPI_TYPES_H_
#define _INCLUDED_TB_3DAPI_TYPES_H_

#include <base/container/tb_pool.h>

// 3DAPIのタイプ
#define TB_3DAPI_DX9 1 // DirectX9
#define TB_3DAPI_GL  2 // OpenGL
#define TB_3DAPI_S3D 3 // Stage3D
#define TB_3DAPI_GX2 4 // GX2
#define TB_3DAPI_GFX 5 // GFX ( for NX)

// 3DAPI決定
#if TB_PLATFORM_IS_WIN
#   if defined(TB_USE_GL)
#       define TB_3DAPI_TARGET TB_3DAPI_GL
#   else
#       define TB_3DAPI_TARGET TB_3DAPI_DX9
#   endif
#elif (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_APPLE) || (TB_PLATFORM_IS_EMSCRIPTEN)
#   define TB_3DAPI_TARGET TB_3DAPI_GL
#elif (TB_PLATFORM_IS_FLASH)
#   define TB_3DAPI_TARGET TB_3DAPI_S3D
#elif (TB_PLATFORM_IS_WIIU)
#   define TB_3DAPI_TARGET TB_3DAPI_GX2
#elif (TB_PLATFORM_IS_NX)
#   define TB_3DAPI_TARGET TB_3DAPI_GFX
#else
#   error "Invalid Platform"
#endif

#define TB_3DAPI_IS_DX9 (TB_3DAPI_TARGET==TB_3DAPI_DX9)
#define TB_3DAPI_IS_GL (TB_3DAPI_TARGET==TB_3DAPI_GL)
#define TB_3DAPI_IS_S3D (TB_3DAPI_TARGET==TB_3DAPI_S3D)
#define TB_3DAPI_IS_GX2 (TB_3DAPI_TARGET==TB_3DAPI_GX2)
#define TB_3DAPI_IS_GFX (TB_3DAPI_TARGET==TB_3DAPI_GFX)

// 深度値タイプ
#define TB_DEPTH_VALUE_TYPE_MINUS_AND_PLUS 1 // ( -1.0f ～ 1.0f )
#define TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS 2  // ( 0.0f ～ 1.0f )

// 3DAPI定義
#if TB_3DAPI_IS_DX9     // DirectX

#   define TB_IS_ENABLE_FIXED_SHADER 1 // 固定機能シェーダを利用可能
#   define TB_DEPTH_VALUE_TYPE TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS
#   include <d3d9.h>
#   include <DxErr.h>
#   include <D3DX9Shader.h>
#if IB_IS_DEBUG
#   pragma comment(lib, "d3dx9d.lib")
#else
#   pragma comment(lib, "d3dx9.lib")
#endif
#   pragma comment(lib,"d3dxof.lib")
#   pragma comment(lib,"dxguid.lib")
#   pragma comment(lib, "d3d9.lib")
#   pragma comment(lib, "dxerr.lib")

#elif TB_3DAPI_IS_GL    // OpenGL

#define TB_GLAPI_GL2    0   // OpenGL2.0相当
#define TB_GLAPI_GLES1  100 // OpenGLES1.1
#define TB_GLAPI_GLES2  101 // OpenGLES2.0

#   define TB_DEPTH_VALUE_TYPE TB_DEPTH_VALUE_TYPE_MINUS_AND_PLUS
#   if (TB_PLATFORM_IS_IOS)
#       define TB_GL_API
#       define TB_GLAPI_TARGET TB_GLAPI_GLES2
#       define TB_IS_ENABLE_FIXED_SHADER 0 // 固定機能シェーダを利用不可
#       define  TB_IS_ENABLE_GLEW 0
#       include <OpenGLES/ES2/gl.h>
#   elif (TB_PLATFORM_IS_ANDROID)
#       define TB_GL_API
#       ifdef TB_USE_GLES2
#           define TB_GLAPI_TARGET TB_GLAPI_GLES2
#           define TB_IS_ENABLE_FIXED_SHADER 0 // 固定機能シェーダを利用不可
#           include <GLES2/gl2.h>
#           include <GLES2/gl2ext.h>
#       else
#           define TB_GLAPI_TARGET TB_GLAPI_GLES1
#           define TB_IS_ENABLE_FIXED_SHADER 1 // 固定機能シェーダを利用可能
#           include <GLES/gl.h>
#           include <GLES/glext.h>
#       endif
#       define  TB_IS_ENABLE_GLEW 0
#   elif (TB_PLATFORM_IS_EMSCRIPTEN)
#       define TB_GL_API
#       define TB_GLAPI_TARGET TB_GLAPI_GLES2
#       define TB_IS_ENABLE_FIXED_SHADER 0 // 固定機能シェーダを利用不可
#       define  TB_IS_ENABLE_GLEW 0
#       include <EGL/egl.h>
#       include <EGL/eglext.h>
#       include <GLES2/gl2.h>
#       include <GLES2/gl2ext.h>
#   elif (TB_PLATFORM_IS_WIN)
#       define  TB_GL_API _stdcall
#       if defined(TB_USE_GLES2)
#           define TB_GLAPI_TARGET TB_GLAPI_GLES2
#           define TB_IS_ENABLE_FIXED_SHADER 0 // 固定機能シェーダを利用不可
#           include <EGL/egl.h>
#           include <EGL/eglext.h>
#           include <GLES2/gl2.h>
#           include <GLES2/gl2ext.h>
#           pragma comment(lib,"libGLESv2.lib")
#           pragma comment(lib,"libEGL.lib")
#           define  TB_IS_ENABLE_GLEW 0
#       else
#           define  TB_GLAPI_TARGET TB_GLAPI_GL2
#           define  TB_IS_ENABLE_FIXED_SHADER (0) // 固定機能シェーダ切り替え可能(0にするとシミュレートします)
#           define  TB_IS_ENABLE_GLEW 1
#           include <glew.h>
#           include <wglew.h>
#           include <GL/gl.h>
#           pragma comment(lib,"glew32.lib")
#       endif
#       pragma comment(lib,"opengl32.lib")
#   else
#       error "invalid platform"
#   endif

#define TB_GLAPI_IS_GL2     (TB_GLAPI_TARGET==TB_GLAPI_GL2)
#define TB_GLAPI_IS_GLES2   (TB_GLAPI_TARGET==TB_GLAPI_GLES2)
#define TB_GLAPI_IS_GLES1   (TB_GLAPI_TARGET==TB_GLAPI_GLES1)
#define TB_GLAPI_IS_GLES    (TB_GLAPI_IS_GLES1 || TB_GLAPI_IS_GLES2)
#   include <graphics/render/3dapi/depend/gl/tb_gl_ext.h>

#elif TB_3DAPI_IS_S3D     // Stage3D

#   define TB_IS_ENABLE_FIXED_SHADER 0 // 固定機能シェーダを利用不可能
#   define TB_DEPTH_VALUE_TYPE TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS
#   include <AGAL.h>

#elif TB_3DAPI_IS_GX2  // 

#   define TB_GX2_WINDOW_WIDTH   (1280)
#   define TB_GX2_WINDOW_HEIGHT  (720)
#   define TB_GX2_TV_RENDER_MODE (GX2_TV_RENDER_1080)

#elif TB_3DAPI_IS_GFX  // GFX

#   define TB_GFX_WINDOW_WIDTH   (1280)
#   define TB_GFX_WINDOW_HEIGHT  (720)
#   define TB_IS_ENABLE_FIXED_SHADER (0)
#   define TB_DEPTH_VALUE_TYPE TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS

#   include <nn/gfx.h>
#if NN_GFX_IS_TARGET_NVN
#   include <nvn/nvn.h>
#   include <nvn/nvn_FuncPtrInline.h>
#if defined( NN_BUILD_TARGET_PLATFORM_OS_NN )
#   include <nvnTool/nvnTool_GlslcInterface.h>
#endif
#endif
#if defined( NN_BUILD_TARGET_PLATFORM_OS_NN ) && defined( NN_BUILD_APISET_NX )
#   include <nv/nv_MemoryManagement.h>
#endif

namespace toybox
{
    // ネイティブデバイス取得
    extern nn::gfx::Device* TbGetGfxDevice();
}
#else
#   error "3dapi undefined"
#endif
// シェーダ言語のタイプ
#define TB_SHADER_API_HLSL 1
#define TB_SHADER_API_GLSL 2
#define TB_SHADER_API_CG 3
#define TB_SHADER_API_AGAL 4
#define TB_SHADER_API_GX2 5
#define TB_SHADER_API_GFX 6

// シェーダ言語定義
#if defined( TB_USE_CG )
    #define TB_SHADER_API_TARGET TB_SHADER_CG
    #include <Cg/cg.h>
    #pragma comment(lib, "cg.lib")
#else
    #if TB_3DAPI_TARGET == TB_3DAPI_DX9
        #define TB_SHADER_API_TARGET TB_SHADER_API_HLSL
    #elif TB_3DAPI_TARGET == TB_3DAPI_GL
        #define TB_SHADER_API_TARGET TB_SHADER_API_GLSL
    #elif TB_3DAPI_TARGET == TB_3DAPI_S3D
        #define TB_SHADER_API_TARGET TB_SHADER_API_AGAL
    #elif TB_3DAPI_TARGET == TB_3DAPI_GX2
        #define TB_SHADER_API_TARGET TB_SHADER_API_GX2
    #elif TB_3DAPI_TARGET == TB_3DAPI_GFX
        #define TB_SHADER_API_TARGET TB_SHADER_API_GFX
    #else
        #error "Shader Undefinded"
    #endif
#endif

#define TB_SHADER_API_IS_HLSL (TB_SHADER_API_TARGET==TB_SHADER_API_HLSL)
#define TB_SHADER_API_IS_GLSL (TB_SHADER_API_TARGET==TB_SHADER_API_GLSL)
#define TB_SHADER_API_IS_AGAL (TB_SHADER_API_TARGET==TB_SHADER_API_AGAL)
#define TB_SHADER_API_IS_GX2  (TB_SHADER_API_TARGET==TB_SHADER_API_GX2)
#define TB_SHADER_API_IS_GFX  (TB_SHADER_API_TARGET==TB_SHADER_API_GFX)
#if TB_SHADER_API_IS_HLSL
#   define TB_SHADER_API_STR "hlsl"
#elif TB_SHADER_API_IS_GLSL || TB_SHADER_API_IS_GX2
#   define TB_SHADER_API_STR "glsl"
#elif TB_SHADER_API_IS_AGAL
#   define TB_SHADER_API_STR "agal"
#elif TB_SHADER_API_IS_GFX
#   define TB_SHADER_API_STR "gfx"
#endif

#if 0
#define TB_SHADER_MATRIX_IS_TRANSPOSE (1)
#else
#define TB_SHADER_MATRIX_IS_TRANSPOSE (0)
#endif

namespace toybox
{

// デバイスID不正値
typedef TbPoolId TbRenderDeviceId;
static const TbRenderDeviceId TB_RENDER_DEVICE_ID_INVALID = TB_POOL_ID_INVALID;

// デバイスインデックス不正値
typedef TbSint32 TbRenderDeviceIndex;
static const TbRenderDeviceIndex TB_RENDER_DEVICE_INDEX_INVALID = -1;
static const TbRenderDeviceIndex TB_RENDER_DEVICE_INDEX_PRIMARY = 0;

// レンダーターゲット最大数
static const TbUint32 TB_MULTI_RENDER_TARGET_MAX = 8;
    
// 頂点ストリーム最大数
static const TbUint32 TB_VERTEX_STREAM_MAX = 4;

// プリミティブタイプ
enum TbPrimitiveType
{
    TB_PT_BEGIN , 
    TB_PT_POINTLIST = TB_PT_BEGIN , // 各頂点を単独の点として扱う
    TB_PT_LINELIST,                 // 2点ずつ順に線を結びます
    TB_PT_LINESTRIP,                // 各頂点を連続的に結びます
    TB_PT_TRIANGLELIST,             // 3つの点ごとに三角形を作る
    TB_PT_TRIANGLESTRIP,            // 線を一つ共有して3角形を作る
    TB_PT_TRIANGLEFAN,              // 頂点を順番に結んでいきます
    TB_PT_END , 
    TB_PT_MAX = TB_PT_END - TB_PT_BEGIN,

    TB_PT_FORCE_32BIT = 0x7fffffff , // force 32bit 
};

// ブレンドに使う要素へ乗算する係数
enum TbBlendFactor
{
    TB_BLEND_FACTOR_BEGIN , 
    TB_BLEND_FACTOR_ZERO = TB_BLEND_FACTOR_BEGIN,          // 0
    TB_BLEND_FACTOR_ONE ,           // 1
    TB_BLEND_FACTOR_SRC_COLOR ,     // 書き込む値のカラー
    TB_BLEND_FACTOR_INV_SRC_COLOR , // 1 - 書き込む値のカラー
    TB_BLEND_FACTOR_DEST_COLOR ,    // 書き込み先のカラー
    TB_BLEND_FACTOR_INV_DEST_COLOR ,// 1 - 書き込み先のカラー
    TB_BLEND_FACTOR_SRC_ALPHA ,     // 書き込む値のアルファ
    TB_BLEND_FACTOR_INV_SRC_ALPHA , // 1 - 書き込む値のアルファ
    TB_BLEND_FACTOR_DEST_ALPHA ,    // 書き込み先のアルファ
    TB_BLEND_FACTOR_INV_DEST_ALPHA ,// 1 - 書き込み先のアルファ
//    TB_BLEND_FACTOR_CONST ,         // 定数( 関数で設定 )
//    TB_BLEND_FACTOR_INV_CONST ,     // 1 - 定数
    TB_BLEND_FACTOR_END ,
    TB_BLEND_FACTOR_MAX = TB_BLEND_FACTOR_END - TB_BLEND_FACTOR_BEGIN
};

// ブレンドの計算式
enum TbBlendOp
{
    TB_BLEND_OP_BEGIN , 
    TB_BLEND_OP_ADD = TB_BLEND_OP_BEGIN ,         // 転送元 + 転送先
    TB_BLEND_OP_SUBTRACT ,    // 転送元 - 転送先
    TB_BLEND_OP_REVSUBTRACT , // 転送先 - 転送元
    TB_BLEND_OP_CMP_MIN ,     // min(転送元,転送先)
    TB_BLEND_OP_CMP_MAX ,     // max(転送元,転送先)
    TB_BLEND_OP_END , 
    TB_BLEND_OP_MAX = TB_BLEND_OP_END - TB_BLEND_OP_BEGIN, 
};

// 比較タイプ
enum TbCmpFunc
{
    TB_CMP_FUNC_BEGIN ,
    TB_CMP_FUNC_NEVER = TB_CMP_FUNC_BEGIN ,         // 不合格
    TB_CMP_FUNC_LESS ,          // <
    TB_CMP_FUNC_EQUAL ,         // =
    TB_CMP_FUNC_LESS_EQUAL ,    // <=
    TB_CMP_FUNC_GREATER ,       // >
    TB_CMP_FUNC_NOT_EQUAL ,     // !=
    TB_CMP_FUNC_GREATER_EQUAL , // >=
    TB_CMP_FUNC_ALWAYS ,        // 不合格
    TB_CMP_FUNC_END , 
    TB_CMP_FUNC_MAX = TB_CMP_FUNC_END - TB_CMP_FUNC_BEGIN ,
};

// カラー書き込みマスクビット値
enum TbColorMaskBit
{
    TB_COLOR_MASK_BIT_RED = TB_BIT(0),
    TB_COLOR_MASK_BIT_GREEN = TB_BIT(1),
    TB_COLOR_MASK_BIT_BLUE = TB_BIT(2),
    TB_COLOR_MASK_BIT_ALPHA = TB_BIT(3),
    TB_COLOR_MASK_BIT_ALL = TB_COLOR_MASK_BIT_RED | TB_COLOR_MASK_BIT_GREEN | TB_COLOR_MASK_BIT_BLUE | TB_COLOR_MASK_BIT_ALPHA
};

// 深度バッファ書き込みマスク
enum TbDepthMask
{
    TB_DEPTH_MASK_BEGIN , 
    TB_DEPTH_MASK_NONE = TB_DEPTH_MASK_BEGIN , // 書き込み不可
    TB_DEPTH_MASK_ALL ,                        // 書き込み可能
    TB_DEPTH_MASK_END ,
    TB_DEPTH_MASK_MAX = TB_DEPTH_MASK_END - TB_DEPTH_MASK_BEGIN ,
};

// ステンシル処理
enum TbStencilOp
{
    TB_STENCIL_OP_BEGIN , 
    TB_STENCIL_OP_KEEP = TB_STENCIL_OP_BEGIN , // 更新しない
    TB_STENCIL_OP_ZERO ,        // 0に設定
    TB_STENCIL_OP_REPLACE ,     // 参照値で更新
    TB_STENCIL_OP_INCR_SAT ,    // 最大値に達するまで増加
    TB_STENCIL_OP_DECR_SAT ,    // 0になるまで減少
    TB_STENCIL_OP_INVERT ,      // 反転
    TB_STENCIL_OP_INCR ,        // 増加(最大値を越すと0)
    TB_STENCIL_OP_DECR ,        // 減少(0より小さくなると最大値)
    TB_STENCIL_OP_END ,
    TB_STENCIL_OP_MAX = TB_STENCIL_OP_END - TB_STENCIL_OP_BEGIN 
};

// 描画面
enum TbRenderFace
{
    TB_RENDER_FACE_FRONT , 
    TB_RENDER_FACE_BACK ,
    TB_RENDER_FACE_MAX ,
};

// カリングモード
enum TbCullMode
{
    TB_CULL_MODE_BEGIN ,
    TB_CULL_MODE_NONE = TB_CULL_MODE_BEGIN , // カリングしない
    TB_CULL_MODE_FRONT , // 表面をカリングする
    TB_CULL_MODE_BACK , // 裏面をカリングする
    TB_CULL_MODE_END , 
    TB_CULL_MODE_MAX = TB_CULL_MODE_END - TB_CULL_MODE_BEGIN , 
};

// フィルモード
enum TbFillMode
{
    TB_FILL_MODE_BEGIN , 
    TB_FILL_MODE_POINT = TB_FILL_MODE_BEGIN , // 点を塗りつぶす
    TB_FILL_MODE_WIREFRAME , // ワイヤーフレームを塗りつぶす
    TB_FILL_MODE_SOLID , // 面を塗りつぶす
    TB_FILL_MODE_END ,
    TB_FILL_MODE_MAX = TB_FILL_MODE_END - TB_FILL_MODE_BEGIN
};

// 描画バッファタイプ
enum TbRenderBufferType
{
    TB_RENDER_BUFFER_TYPE_COLOR , // カラーバッファ
    TB_RENDER_BUFFER_TYPE_DEPTH , // カラーバッファ
    TB_RENDER_BUFFER_TYPE_STENCIL , // カラーバッファ
    TB_RENDER_BUFFER_TYPE_MAX , 
};

// 描画バッファビット
enum TbRenderBufferTypeBit
{
    TB_RENDER_BUFFER_TYPE_COLOR_BIT = TB_BIT( TB_RENDER_BUFFER_TYPE_COLOR ) , // カラーバッファ
    TB_RENDER_BUFFER_TYPE_DEPTH_BIT = TB_BIT( TB_RENDER_BUFFER_TYPE_DEPTH ) , // 深度バッファ
    TB_RENDER_BUFFER_TYPE_STENCIL_BIT = TB_BIT( TB_RENDER_BUFFER_TYPE_STENCIL ) , // ステンシルバッファ
};


// シェーダの種類
enum TbShaderType
{
    TB_SHADER_TYPE_BEGIN , 
    TB_SHADER_TYPE_VERTEX = TB_SHADER_TYPE_BEGIN , // 頂点シェーダ
    TB_SHADER_TYPE_PIXEL , // ピクセルシェーダ( フラグメントシェーダ )
    TB_SHADER_TYPE_GEOMETRY , // ジオメトリシェーダ
    TB_SHADER_TYPE_END ,
    TB_SHADER_TYPE_MAX = TB_SHADER_TYPE_END - TB_SHADER_TYPE_BEGIN ,
};

// シェーダ種類ビット
enum TbShaderTypeBit
{
    TB_SHADER_TYPE_BIT_VERTEX = TB_BIT(TB_SHADER_TYPE_VERTEX) , 
    TB_SHADER_TYPE_BIT_PIXEL  = TB_BIT(TB_SHADER_TYPE_PIXEL) , 
};

// ロックフラグビット
enum TbLockFlagBit
{
    TB_LOCK_FLAG_BIT_READ_ONLY  = TB_BIT(0) , 
    TB_LOCK_FLAG_BIT_WRITE_ONLY = TB_BIT(1) , 
};

// テクスチャフィルターモード
enum TbTextureFilterMode
{
    TB_TEXTURE_FILTER_MODE_BEGIN,
    TB_TEXTURE_FILTER_MODE_POINT = TB_TEXTURE_FILTER_MODE_BEGIN,  // 最近点
    TB_TEXTURE_FILTER_MODE_LINEAR,  // 線形補完
    TB_TEXTURE_FILTER_MODE_END,
    TB_TEXTURE_FILTER_MODE_MAX = TB_TEXTURE_FILTER_MODE_END - TB_TEXTURE_FILTER_MODE_BEGIN
};

// テクスチャアドレッシングモード
enum TbTextureAddressMode
{
    TB_TEXTURE_ADDRESS_MODE_BEGIN , 
    TB_TEXTURE_ADDRESS_MODE_REPEAT = TB_TEXTURE_ADDRESS_MODE_BEGIN , // 繰り返し 
    TB_TEXTURE_ADDRESS_MODE_CLAMP ,
    TB_TEXTURE_ADDRESS_MODE_END , 
    TB_TEXTURE_ADDRESS_MODE_MAX = TB_TEXTURE_ADDRESS_MODE_END - TB_TEXTURE_ADDRESS_MODE_BEGIN 
};

// テクスチャ合成引数(固定機能用)
enum TbTextureArg
{
    TB_TEXTURE_ARG_TEXTURE      , // テクスチャ
    TB_TEXTURE_ARG_VERTEX_COLOR , // 頂点カラー
    TB_TEXTURE_ARG_MAX 
};

// テクスチャ合成方法(固定機能用)
enum TbTextureOp
{
    TB_TEXTURE_OP_ARG0     ,  // arg0を出力
    TB_TEXTURE_OP_MODULATE ,  // arg0 * arg1
    TB_TEXTURE_OP_ADD      ,  // arg0 + arg1
    TB_TEXTURE_OP_SUBTRACT ,  // arg0 - arg1
    TB_TEXTURE_OP_MAX
};

// インデックスバッファのフォーマット
enum TbIndexBufferFormat
{
    TB_INDEX_BUFFER_FORMAT_U16 ,
    TB_INDEX_BUFFER_FORMAT_U32 ,
    TB_INDEX_BUFFER_FORMAT_MAX ,
};

// 画面に表示する際の挙動
enum TbRenderPresentType
{
    TB_RENDER_PRESENT_BUFFER             , // レンダリングバッファサイズ
    TB_RENDER_PRESENT_WINDOW             , // ウィンドウサイズ
    TB_RENDER_PRESENT_WINDOW_KEEP_ASPECT , // ウィンドウサイズ(縦横比は維持)

    TB_RENDER_PRESENT_DEFAULT = TB_RENDER_PRESENT_WINDOW
};

}

#endif
