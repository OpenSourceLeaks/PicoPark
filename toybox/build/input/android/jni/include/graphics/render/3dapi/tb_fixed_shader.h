/*!
 * @file
 * @brief 固定機能シェーダクラス
 * @author Miyake Shunsuke
 * @since 2010.9.9
 */
 
#ifndef _INCLUDED_TB_FIXED_SHADER_H_
#define _INCLUDED_TB_FIXED_SHADER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <base/math/tb_matrix.h>

namespace toybox
{
    enum TbFixedShaderMatrixType
    {
        TB_FIXED_SHADER_MATRIX_WORLD ,      // ワールド座標変換行列 
        TB_FIXED_SHADER_MATRIX_VIEW ,       // ビュー変換行列
        TB_FIXED_SHADER_MATRIX_PROJECTION , // 射影行列設定
        TB_FIXED_SHADER_MATRIX_TEXTURE0 ,   // テクスチャ行列
        TB_FIXED_SHADER_MATRIX_MAX , 
    };

    enum TbFixedShaderShadeMode
    {
        TB_FIXED_SHADER_SHADE_MODE_FLAT ,   // フラットシェーダ
        TB_FIXED_SHADER_SHADE_MODE_GOURAUD , // グーローシェーディング
        TB_FIXED_SHADER_SHADE_MODE_MAX , 
    };
}

// 依存コード
#include "./depend/tb_fixed_shader_depend.h"

namespace toybox
{

class TbRenderDevice;
class TbVertexFormat;
class TbFixedLight;
class TbFixedMaterial;

/*!
 * 固定機能シェーダクラス
 */
class TbFixedShader
{
    friend class TbFixedShaderDepend;
public:

    typedef TbFixedShader Self;

public:

    // コンストラクタ
    TbFixedShader();

    // デストラクタ
    ~TbFixedShader();

public:

    // 頂点フォーマット設定
    void SetVertexFormat( const TbVertexFormat* vertexFormat );

    // 頂点フォーマット取得
    const TbVertexFormat* GetVertexFormat() const { 
        return m_VertexFormat; 
    }

    // 固定機能シェーダに渡す行列設定
    TbResult SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix44& matrix );

    // 固定機能シェーダに渡す行列取得
    TbResult GetMatrix( TbFixedShaderMatrixType matrixType , TB_OUT TbMatrix44& matrix );

    // シェーディングモード設定
    TbResult SetShadeMode( TbFixedShaderShadeMode shadeMode );

    // ライト計算有効設定
    TbResult SetEnableLighting( TbBool isEnable );

    // ライト有効設定
    TbResult SetEnableLight( TbUint32 index , TbBool isEnable );

    // ライト設定
    TbResult SetLight( TbUint32 index , const TbFixedLight* light );

    // マテリアル設定
    TbResult SetMaterial( const TbFixedMaterial* material );

    // テクスチャ合成方法設定( 入力RGB )
    TbResult SetTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg );

    // テクスチャ合成方法設定( RGB演算方式 )
    TbResult SetTextureColorOp( TbUint32 stage , TbTextureOp op );

    // テクスチャ合成方法設定( 入力アルファ )
    TbResult SetTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg );

    // テクスチャ合成方法設定( アルファ演算方式 )
    TbResult SetTextureAlphaOp( TbUint32 stage , TbTextureOp op );

public:

    // 依存コードクラス取得
    TbFixedShaderDepend& GetDepend(){ 
        return m_Depend; 
    } 
    
    // 依存コードクラス取得
    const TbFixedShaderDepend& GetDepend() const { 
        return m_Depend; 
    } 

private:

    const TbVertexFormat* m_VertexFormat;   //!< 頂点フォーマット
    TbFixedShaderDepend   m_Depend;         //!< 依存クラス

};

}

#endif
