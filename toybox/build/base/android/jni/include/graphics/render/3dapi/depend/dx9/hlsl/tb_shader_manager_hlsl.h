/*!
 * @file
 * @brief シェーダ管理クラス( HLSL版 )
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SHADER_CONTEXT_HLSL_H_
#define _INCLUDED_TB_SHADER_CONTEXT_HLSL_H_

#include <base/container/tb_pool.h>

namespace toybox
{

//! 定数ハンドル
struct TbShaderConstHandleDepend
{
    TbShaderConstHandleDepend() : reg(static_cast<TbUint32>(-1)) {}
    // ==比較演算子
    TbBool operator==(const TbShaderConstHandleDepend& target) {
        return reg == target.reg;
    }
    // !=比較演算子
    TbBool operator!=(const TbShaderConstHandleDepend& target) {
        return reg != target.reg;
    }
    TbUint32 reg;
};

/*!
 * シェーダ管理クラス
 */
class TbShaderManagerDepend
{
public:

    //コンストラクタ
    TbShaderManagerDepend();

    //デストラクタ
    ~TbShaderManagerDepend();

public:

    // 各シェーダの格納限界を設定
    void SetCapacity( TbUint32 vertexMax , TbUint32 pixelMax ) {
        m_VertexShaderPool.SetCapacity( vertexMax );
        m_PixelShaderPool.SetCapacity( pixelMax );
    }

    // 定数レジスタ取得
    TbUint32 GetConstRegister( TbUint32 index ) 
    {
        return m_RegisterList[index];
    }

private:

    //! 頂点シェーダ
    struct VertexShader
    {
        VertexShader() : shader(nullptr) , format( nullptr ) {}
        LPDIRECT3DVERTEXSHADER9 shader;               // シェーダ
        const TbVertexFormat*         format;         // 頂点フォーマット
    };
    //! 頂点シェーダプール
    typedef TbPool<VertexShader> VertexShaderPool;

    //! ピクセルシェーダ
    typedef LPDIRECT3DPIXELSHADER9 PixelShader;
    //! ピクセルシェーダプール
    typedef TbPool<PixelShader> PixelShaderPool;

private:

    // デバイス設定
    void setDevice( TbRenderDevice* device ) {
        m_Device = device;
    }

    // 頂点シェーダ取得
    TB_INLINE VertexShader* getVertexShader( TbShaderId id )
    { 
        VertexShader* result = m_VertexShaderPool.GetAt(id);
        if( result ){
            return result;
        }
        return nullptr;
    }

    // ピクセルシェーダ取得
    TB_INLINE PixelShader getPixelShader( TbShaderId id )
    {
        PixelShader* result = m_PixelShaderPool.GetAt(id);
        if( result ){
            return *result;
        }
        return nullptr;
    }

    // シェーダプールが一杯か
    TbBool isFullVertexShaderPool() const { 
        return m_VertexShaderPool.IsFull(); 
    }

    // シェーダプールが一杯か
    TbBool isFullPixelShaderPool() const { 
        return m_PixelShaderPool.IsFull(); 
    }

    // 頂点シェーダ追加
    TbShaderId addVertexShader( LPDIRECT3DVERTEXSHADER9 shader );

    // ピクセルシェーダ取得
    TbShaderId addPixelShader( PixelShader shader ){ 
        return m_PixelShaderPool.Add(shader); 
    }

    // シェーダ全削除
    void deleteShaderAll( TbBool isEnableReset );

private:

    static const TbSint32 CONST_HANDLE_MAX = 0xFFFF;

private:

    TbRenderDevice*    m_Device;                         //!< 対応デバイス
    VertexShaderPool   m_VertexShaderPool;               //!< 頂点シェーダプール
    PixelShaderPool    m_PixelShaderPool;                //!< ピクセルシェーダプール
    TbUint32           m_RegisterList[CONST_HANDLE_MAX]; //!< 定数レジスタ番号リスト

private:
    
    friend class TbShaderManager;

};

}

#endif
