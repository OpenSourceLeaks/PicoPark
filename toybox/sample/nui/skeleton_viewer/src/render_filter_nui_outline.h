/*!
 * Nui用シルエットの輪郭線描画フィルター
 * @author Miyake Shunsuke
 * @since 2011.10.21
 */

#ifndef _INCLUDED_GR_FILETER_NUI_OUTLINE_H_
#define _INCLUDED_GR_FILETER_NUI_OUTLINE_H_

#include <graphics/render/3dapi/tb_3dapi.h>
#include <base/math/tb_geometry.h>

namespace toybox
{
    class TbTexture;
    class TbTextureSampler;
    class TbVertexBuffer;
};

class RenderFilterNuiOutline
{
public:

    // コンストラクタ
    RenderFilterNuiOutline( const toybox::TbRectF32& uv  );

    // デストラクタ
    ~RenderFilterNuiOutline();

public:

    // 描画
    TbResult Render( toybox::TbTexture* texture , const toybox::TbRectF32& uv );

private:

    toybox::TbTextureSampler*   m_TextureSampler;           // テクスチャサンプラ
    toybox::TbShaderConstHandle m_ConstHandleTexWidth;
    toybox::TbShaderConstHandle m_ConstHandleTexHeight;
    toybox::TbShaderConstHandle m_ConstHandleOffsetMatrix;

private:    // レンダリングパス1用 ( ぼかしシルエット抽出 )

    toybox::TbShaderId       m_VertexShaderIdSilhouette; // 頂点シェーダ
    toybox::TbShaderId       m_PixelShaderIdSilhouette;  // ピクセルシェーダ
    toybox::TbVertexFormat*  m_VertexFormatSilhouette;   // 頂点フォーマット
    toybox::TbVertexBuffer*  m_VertexBufferSilhouette;   // 頂点バッファ
    toybox::TbTexture*       m_RenderTextureSilhouette;  // レンダーターゲット 
    toybox::TbShaderProgram* m_ProgramSilhouette;

private: // レンダリングパス2用 ( 輪郭線を抽出してカラーバッファと合成  )

    toybox::TbShaderId       m_VertexShaderIdOutline; // 頂点シェーダ
    toybox::TbShaderId       m_PixelShaderIdOutline;  // ピクセルシェーダ
    toybox::TbVertexFormat*  m_VertexFormatOutline;   // 頂点フォーマット
    toybox::TbVertexBuffer*  m_VertexBufferOutline;   // 頂点バッファ
    toybox::TbShaderProgram* m_ProgramOutline;

};

#endif
