/*!
 * MMD用描画オブジェクト
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#include <tb_fwd.h>
#include <graphics/scene/tb_render_object_mmd.h>
#include <graphics/render/tb_render_pass.h>
#include <graphics/render/tb_render_pass_manager.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbRenderObjectMmd::TbRenderObjectMmd()
    : m_MaterialCount(0)
    , m_MaterialState(nullptr)
    , m_Mesh(nullptr)
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbRenderObjectMmd::~TbRenderObjectMmd()
{
    TB_SAFE_DELETE_ARRAY(m_MaterialState);
}

/*!
 * 更新
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::OnStepImpl( TbFloat32 deltatime )
{
    m_Motion.StepFrame(deltatime*60.0f);
}

/*!
 * 描画
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::OnDrawImpl( TbUint32 passIndex )
{
    if( !m_Mesh ){
        return;
    }
    TbRenderPass* pass = TbGetRenderPass(passIndex);
    TbUint32 startIndex = 0;

#if !TB_PMX_BUILD_FOR_SM_3_0
    pass->SetVertexBuffer( 0 , m_Mesh->GetVertexBuffer() , 0 , sizeof(TbVertexPmx) );
    pass->SetIndexBuffer( m_Mesh->GetIndexBuffer() );
#endif

    for( TbUint32  i = 0; i < m_MaterialCount; ++i )
    {
        if( !m_MaterialState[i].bitArray.Test(MaterialState::FLAG_VISIBLE) ){
            continue;
        }

        const TbModelDataPmx::MaterialInfo& materialParam = m_MaterialState[i].material.GetParam();

        // ボーン行列
        TbMatrix43 boneMatrix[128];
        for( TbUint32 j = 0; j < materialParam.weightedBoneCount; ++j) {
            TbUint32 boneIndex = materialParam.weightedBoneList[j];
            m_Motion.GetTransformMatrix(boneMatrix[j],boneIndex);
            boneMatrix[j].Transpose();
        }

        m_MaterialState[i].material.SendShaderParam( passIndex , 
                                                     &GetPoseMatrix() ,
                                                     boneMatrix ,
                                                     materialParam.weightedBoneCount );
#if !TB_PMX_BUILD_FOR_SM_3_0
        pass->DrawIndexedPrimitive( TB_PT_TRIANGLELIST , 
                                    0 ,
                                    materialParam.vertex.vertexCount ,
                                    startIndex , 
                                    materialParam.indexCount/3 );
#else
        pass->SetVertexBuffer( 0 , m_Mesh->GetVertexBuffer(i) , 0 , sizeof(TbVertexPmx) );        
        pass->DrawPrimitive( TB_PT_TRIANGLELIST , 
                             0 ,
                             materialParam.indexCount/3 );
#endif

        startIndex += materialParam.indexCount;
    }
}

/*!
 * モデルリソースから設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::Setup( TbResourceHandleModelPmx& model , TbShaderProgram* materialShader )
{
    SetMesh( model.GetMesh() );
    SetSkeleton( model.GetSkeleton() );

    // マテリアル設定
    m_MaterialCount = model.GetMaterialCount();
    m_MaterialState = TB_NEW MaterialState[m_MaterialCount];
    for( TbUint32 i = 0; i < m_MaterialCount; ++i ) {
        model.GetMaterial( m_MaterialState[i].material , i );
    }

    // シェーダ設定
    SetMaterialShader( materialShader );
}

/*!
 * セットアップ(手動設定用)
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::Setup( TbUint32 materialCount )
{
    TB_SAFE_DELETE_ARRAY(m_MaterialState);
    m_MaterialCount = materialCount;
    m_MaterialState = TB_NEW MaterialState[m_MaterialCount];
}

/*!
 * メッシュ設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::SetMesh( TbMesh* mesh )
{
    m_Mesh = mesh;
}

/*!
 * マテリアル用パラメータ設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::SetMaterialShader( TbShaderProgram* materialShader )
{
    // シェーダ一括設定
    for( TbUint32 i = 0; i < m_MaterialCount; ++i ) {
        m_MaterialState[i].material.SetShaderProgram( materialShader );
    }
}

/*!
 * マテリアル用シェーダ設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbRenderObjectMmd::SetMaterialShader( TbUint32 index , TbShaderProgram* materialShader )
{
    if( index < m_MaterialCount ) {
        m_MaterialState[index].material.SetShaderProgram( materialShader );
    }
}

/*!
 * マテリアル毎に可否設定する
 * @author Miyake_Shunsuke
 * @since 2014.05.01
 */
void TbRenderObjectMmd::SetVisibleMaterial( TbBool isVisible )
{
    for( TbUint32 i = 0; i < m_MaterialCount; ++i ) {
        m_MaterialState[i].bitArray.SetBit(MaterialState::FLAG_VISIBLE,isVisible);
    }
}

/*!
 * マテリアル毎に可否設定する
 * @author Miyake_Shunsuke
 * @since 2014.05.01
 */
void TbRenderObjectMmd::SetVisibleMaterial( TbUint32 index , TbBool isVisible )
{
    if( index < m_MaterialCount ) {
        m_MaterialState[index].bitArray.SetBit(MaterialState::FLAG_VISIBLE,isVisible);
    }
}

}
