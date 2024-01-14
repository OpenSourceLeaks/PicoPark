/*!
 * シェーダーコンテキストクラス( GLSL )
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */
 
#ifndef _INCLUDED_TB_SHADER_MANAGER_GLSL_H_
#define _INCLUDED_TB_SHADER_MANAGER_GLSL_H_

#include <base/container/tb_array.h>
#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_constant_buffer.h>

namespace toybox
{

//! 定数ハンドル
struct TbShaderConstHandleDepend
{
    static const TbUint32 HANDLE_NAME_MAX = 64;
    static const TbSint32 INVALID_HANDLE = -1;

    TbShaderConstHandleDepend() 
    {
        TbFill(handle, INVALID_HANDLE);
        name[0] = '\0';
    }

    // ==比較演算子
    TbBool operator==(const TbShaderConstHandleDepend& target){
        return TbEqualsArray(target.handle, handle);
    }
    // !=比較演算子
    TbBool operator!=(const TbShaderConstHandleDepend& target){
        return !TbEqualsArray(target.handle,handle);
    }

    void ResetHandle() {
        TbFill(handle, INVALID_HANDLE);
    }

    TbSint32 handle[TB_SHADER_TYPE_MAX];
    TbChar8  name[HANDLE_NAME_MAX];
};

class TbShaderManager;
class TbShaderProgram;

/*!
 * シェーダー管理GFX依存クラス
 * @author Miyake Shunsuke
 */
class TbShaderManagerDepend
{
public:

    static const TbUint32 CONSTANT_MAX = 10;

    struct VertexShader
    {
        VertexShader()
            : format(nullptr)
            , shader(nullptr)
            , vertexStateMemory(nullptr)
            , vertexState(nullptr)
        {}
        ~VertexShader()
        {
            TB_ASSERT(!vertexState);
        }
        const TbVertexFormat* format;
        nn::gfx::Shader* shader;
        TbUint8*                vertexStateMemory;
        nn::gfx::VertexState*   vertexState;      //1< 頂点ステート
    };

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

    // シェーダ取得
    nn::gfx::Shader* GetShader(TbShaderType type, TbShaderId id) 
    {
        nn::gfx::Shader* ret = nullptr;
        switch (type) {
        case TB_SHADER_TYPE_VERTEX:
            {
                VertexShader* val = m_VertexShaderPool.GetAt(id);
                if (val) {
                    ret = val->shader;
                }
            }
            break;
        case TB_SHADER_TYPE_PIXEL:
            {
                nn::gfx::Shader** val = m_PixelShaderPool.GetAt(id);;
                if (val) {
                    ret = *val;
                }
            }
            break;
        case TB_SHADER_TYPE_GEOMETRY:
            break;
        default:
            break;
        }
        return ret;
    }

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
    TbShaderId addVertexShader(nn::gfx::Shader* shader);

    // ピクセルシェーダ追加
    TbShaderId addPixelShader(nn::gfx::Shader* shader);

    // 頂点シェーダと頂点フォーマットをバインド
    TbBool bindVertexFormat( const TbVertexFormat* format , const TbShaderId shaderId );

    // テクスチャサンプラ変数情報更新
    void updateSamplerVar();

    // テクスチャサンプラ変数情報更新
    void updateSamplerVar( TbUint32 slot );

    // セットアップ
    nn::gfx::Shader* createShader( nn::gfx::ShaderStage stage , const char* buffer , TbUint32 ufferSize );

private:

    enum{
        FLAG_USE,
        FLAG_CHANGE,
        FLAG_TURN_OVER,
    };
    //! シェーダプール
    typedef TbPool<VertexShader>     VertexShaderPool;
    typedef TbPool<nn::gfx::Shader*> ShaderPool;

    template<TbUint32 COUNT>
    class ConstantBuffer
    {
    public:
        ConstantBuffer() : m_Index(0) { TbFillNull(m_Buffer); }
        ~ConstantBuffer(){ Finalize(); }
    public:
        // 初期化
        void Initialize( TbSizeT size)
        {
            for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Buffer); ++i)
            {
                m_Buffer[i] = TB_NEW TbConstantBuffer(size);
            }
        }
        // 後始末
        void Finalize()
        {
            for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Buffer); ++i)
            {
                TB_SAFE_DELETE(m_Buffer[i]);
            }
        }
    public:
        // リセット
        void Reset()
        {
            m_Index = 0;
        }
        // 確保
        TbConstantBuffer* Allocate() {
            if (m_Index < TB_ARRAY_COUNT_OF(m_Buffer)) {
                return m_Buffer[m_Index++];
            }
            return nullptr;
        }
    private:
        TbSizeT             m_Index;
        TbConstantBuffer*   m_Buffer[COUNT];
    };

    static const TbSizeT WVP_BUFFER_COUNT = 128;

private:

    TbBitArray16        m_BitArray;                   //!< ビット配列
    VertexShaderPool    m_VertexShaderPool;           //!< 頂点シェーダプール
    ShaderPool          m_PixelShaderPool;            //!< ピクセルシェーダプール

private:

    ConstantBuffer<WVP_BUFFER_COUNT>    m_ConstantBufferWVP;    //!< constantBuffer

private:

    friend class TbRenderDevice;
    friend class TbShaderManager;

};

}

#endif
