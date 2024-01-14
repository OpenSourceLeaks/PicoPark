/*!
 * PMX用マテリアル
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#include <tb_fwd.h>
#include <graphics/material/tb_material_pmx.h>
#include <graphics/render/3dapi/tb_render_device.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_render_pass.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbMaterialPmx::TbMaterialPmx()
    : m_TextureCount(0)
{
    TbMemClear(m_Texture);
    m_BoneConstHandle.Set(TB_SHADER_CONST_INDEX_USER_BEGIN);
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbMaterialPmx::~TbMaterialPmx()
{
}

/*!
 * シェーダーパラメータをGPUに送る
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbMaterialPmx::SendShaderParam( TbUint32 passIndex , 
                                     const TbMatrix* world , 
                                     const TbMatrix43* boneMatrix , 
                                     TbUint32 boneMatrixCount )
{
    ActivateProgram(passIndex);
    
    TbRenderPass* pass = TbGetRenderPass(passIndex);
    if( pass ){
#if 0 // お遊び
        pass->SetEnableBlend(TB_TRUE);
        pass->SetEnableDepthTest(TB_TRUE);

        if( world ) {
            // ワールド更新
            pass->SetWorldMatrix(*world);
        }

        // ボーン行列設定
        pass->SetShaderConstantMatrix(TB_SHADER_TYPE_BIT_VERTEX,
                                      &m_BoneConstHandle,
                                      boneMatrix,boneMatrixCount);
#else
        pass->SetBlendState(TbRenderStatePreset::BLEND_OPACITY);
        pass->SetDepthStencilState(TbRenderStatePreset::DEPTH_STENCIL_3D_NORMAL);

        if( world ) {
            // ワールド更新
            pass->SetWorldMatrix(*world);
        }

        // ボーン行列設定
        pass->SetShaderConstantMatrix(TB_SHADER_TYPE_BIT_VERTEX,
                                      &m_BoneConstHandle,
                                      boneMatrix,boneMatrixCount);

        // テクスチャ設定
        for( TbUint32 i = 0; i < m_TextureCount; ++i ) {
            pass->SetTexture( i , m_Texture[i] );
        }
#endif
    }
}

/*!
 * パラメータ設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbMaterialPmx::SetParam( TbModelDataPmx::MaterialInfo& param )
{
    m_Param = param;
    m_TextureCount = 1; // FIXME
}

/*!
 * テクスチャ設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbMaterialPmx::SetTexture( TbUint32 index , TbTexture* texture )
{
    if( index < m_TextureCount ) {
        m_Texture[index] = texture;
    }
}

}
