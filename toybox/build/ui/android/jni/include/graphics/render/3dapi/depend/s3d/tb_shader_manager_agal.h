/*!
 * シェーダ管理クラス(AGAL依存)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */
 
#ifndef _INCLUDED_TB_SHADER_MANAGER_AGAL_H_
#define _INCLUDED_TB_SHADER_MANAGER_AGAL_H_

#include <base/container/tb_array.h>
#include <base/math/tb_matrix.h>

namespace toybox
{

//! 定数ハンドル
typedef TbUint32 TbShaderConstHandle;
extern TbShaderConstHandle TB_SHADER_CONST_HANDLE_INVALID;

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
    
    typedef AS3::ui::com::adobe::utils::AGALMiniAssembler Assembler;
    
    //! 頂点シェーダ
    struct VertexShader
    {
        VertexShader() : shader() , format( NULL ) {}
        Assembler shader;               // シェーダ
        const TbVertexFormat*         format;         // 頂点フォーマット
    };

    //! ピクセルシェーダ
    typedef AS3::ui::com::adobe::utils::AGALMiniAssembler PixelShader;
    
public:

    // コンストラクタ
    TbShaderManagerDepend();

    // デストラクタ
    ~TbShaderManagerDepend();

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

    // 頂点属性を利用可能にする
    TbBool EnableVertexAttribute();

    // Model View Projection行列のみはこれを利用して設定 )
    void SetModelViewProjectionMatrix( const TbShaderConstHandle& handle , const TbMatrix& mtx ) {
        m_ModelViewProjMatrixHandle = handle;
        m_ModelViewProjMatrix = mtx;
    }

    // 固定機能のシミュレーション用シェーダが有効かどうか
    const TbShaderProgram* GetSimulatedFixedShader() const {
        return m_SimulatedFixedShader;
    }

    // 固定機能のシミュレーション用シェーダの設定
    void SetSimulatedFixedShader( const TbShaderProgram* program ) {
        m_SimulatedFixedShader = program;
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
    TbShaderId addVertexShader( Assembler& shader );

    // ピクセルシェーダ追加
    TbShaderId addPixelShader( Assembler& shader );

    // 頂点シェーダと頂点フォーマットをバインド
    TbBool bindVertexFormat( const TbVertexFormat* format , const TbShaderId shaderId );

    // セットアップ
    Assembler createShader( TbShaderType type , const char* buffer );

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
    TbShaderConstHandle m_ModelViewProjMatrixHandle;  //!< モデルビュープロジェクション行列を設定するためのハンドル
    TbMatrix            m_ModelViewProjMatrix;        //!< モデルビュープロジェクション行列

private: // 固定機能シェーダシミュレーション用

    const TbShaderProgram* m_SimulatedFixedShader;

private:

    friend class TbRenderDevice;
    friend class TbShaderManager;

};

}

#endif
