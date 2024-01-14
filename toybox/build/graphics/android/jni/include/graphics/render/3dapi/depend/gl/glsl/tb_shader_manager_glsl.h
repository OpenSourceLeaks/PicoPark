/*!
 * シェーダーコンテキストクラス( GLSL )
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */
 
#ifndef _INCLUDED_TB_SHADER_MANAGER_GLSL_H_
#define _INCLUDED_TB_SHADER_MANAGER_GLSL_H_

#include <base/container/tb_array.h>
#include <base/math/tb_matrix.h>

namespace toybox
{

// Uniform変数位置の不正値
const TbUint32 TB_SHADER_GLSL_CONST_HANDLE_INVALID = 0xFFFFFFFF;

//! 定数ハンドル
struct TbShaderConstHandleDepend
{
    static const TbUint32 HANDLE_NAME_MAX = 64;

    TbShaderConstHandleDepend() 
        : handle(TB_SHADER_GLSL_CONST_HANDLE_INVALID) 
    {
        name[0] = '\0';
    }

    // ==比較演算子
    TbBool operator==(const TbShaderConstHandleDepend& target){
        return target.handle == handle;
    }
    // !=比較演算子
    TbBool operator!=(const TbShaderConstHandleDepend& target){
        return target.handle != handle;
    }

    GLuint   handle;
    TbChar8  name[HANDLE_NAME_MAX];
};

class TbShaderManager;
class TbShaderProgram;

/*!
 * シェーダー管理GLSL依存クラス
 * @author Miyake Shunsuke
 * @since 2010.06.11
 */
class TbShaderManagerDepend
{
public:

    static const TbUint32 CONSTANT_MAX = 10;

    //! 頂点シェーダ
    struct VertexShader {
        VertexShader() : shader(0) , format( nullptr ) {}
        GLuint                  shader;               //!< シェーダ
        const TbVertexFormat*   format;               //!< 頂点フォーマット
    };

    //! ピクセルシェーダ
    typedef GLuint PixelShader;

public:

    // コンストラクタ
    TbShaderManagerDepend();

    // デストラクタ
    ~TbShaderManagerDepend();

public:

    // 定数名設定
    const TbChar8* GetConstantName( const TbUint32 index );

    // 定数名設定
    TbResult SetConstantName( const TbUint32 index , const TbChar8* name );

public:

    // 各シェーダの格納限界を設定
    void SetCapacity( TbUint32 vertexMax , TbUint32 pixelMax ) {
        m_VertexShaderPool.SetCapacity( vertexMax );
        m_PixelShaderPool.SetCapacity( pixelMax );
    }

    // 頂点シェーダ情報取得
    VertexShader* GetVertexShader( TbShaderId id ) {
        return m_VertexShaderPool.GetAt(id);
    }

    // ピクセルシェーダ情報取得
    PixelShader* GetPixelShader( TbShaderId id ) {
        return m_PixelShaderPool.GetAt(id);
    }

public:

    // 頂点属性を利用可能にする (StreamSourceを利用)
    TbBool EnableVertexAttribute();

    // 頂点属性を利用可能にする
    TbBool EnableVertexAttribute( const void* vertexs , TbUint32 stride );

    // 頂点属性の使用不可にする
    TbBool DisableVertexAttribute();

    // Model View Projection行列のみはこれを利用して設定 )
    void SetModelViewProjectionMatrix( const TbMatrix& mtx ) {
        m_ModelViewProjMatrix = mtx;
    }

#if !TB_IS_ENABLE_FIXED_SHADER

    // 固定機能のシミュレーション用シェーダが有効かどうか
    const TbShaderProgram* GetSimulatedFixedShader() const {
        return m_SimulatedFixedShader;
    }

    // 固定機能のシミュレーション用シェーダの設定
    void SetSimulatedFixedShader( const TbShaderProgram* program ) {
        m_SimulatedFixedShader = program;
    }

#endif

private:

    // シェーダプールが一杯か
    TbBool isFullVertexShaderPool() const { 
        return m_VertexShaderPool.IsFull(); 
    }

    // シェーダプールが一杯か
    TbBool isFullPixelShaderPool() const { 
        return m_PixelShaderPool.IsFull(); 
    }

    // 頂点シェーダ追加
    TbShaderId addVertexShader( GLuint shader );

    // ピクセルシェーダ追加
    TbShaderId addPixelShader( GLuint shader );

    // 頂点シェーダと頂点フォーマットをバインド
    TbBool bindVertexFormat( const TbVertexFormat* format , const TbShaderId shaderId );

    // テクスチャサンプラ変数情報更新
    void updateSamplerVar();

    // テクスチャサンプラ変数情報更新
    void updateSamplerVar( TbUint32 slot );

    // セットアップ
    GLuint createShader( GLenum type , const char* buffer , TbUint32 ufferSize );

    // コンパイルエラーチェック
    TbBool isCompileError( GLuint shader );

    // 反転行列設定更新
    void updateTurnOverMatrix();

private:

    enum{
        FLAG_USE,
        FLAG_CHANGE,
        FLAG_TURN_OVER,
    };
    //! 頂点シェーダプール
    typedef TbPool<VertexShader> VertexShaderPool;

    //! ピクセルシェーダプール
    typedef TbPool<PixelShader> PixelShaderPool;

private:

    TbBitArray16        m_BitArray;                   //!< ビット配列
    VertexShaderPool    m_VertexShaderPool;           //!< 頂点シェーダプール
    PixelShaderPool     m_PixelShaderPool;            //!< ピクセルシェーダプール
    TbMatrix            m_ModelViewProjMatrix;        //!< モデルビュープロジェクション行列

private: // 固定機能シェーダシミュレーション用

#if !TB_IS_ENABLE_FIXED_SHADER
    const TbShaderProgram* m_SimulatedFixedShader;
#endif

private:

    friend class TbRenderDevice;
    friend class TbShaderManager;

};

}

#endif
