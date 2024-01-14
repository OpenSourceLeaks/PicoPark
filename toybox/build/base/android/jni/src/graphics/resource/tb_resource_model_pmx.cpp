/*!
 * PMXモデル用リソース
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#include <tb_fwd.h>
#include <graphics/resource/tb_resource_model_pmx.h>
#include <graphics/render/3dapi/tb_vertex_format.h>
#include <graphics/render/3dapi/tb_vertex_buffer.h>
#include <graphics/render/3dapi/tb_index_buffer.h>
#include <base/io/tb_path.h>
#include <base/io/tb_file_util.h>

namespace toybox
{

// 頂点フォーマット要素
static const TbVertexElement PMX_VERTEX_ELEMENT[] = 
{
    { 0 , 0   , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12  , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } ,
    { 0 , 24  , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 } ,
    { 0 , 32  , TbVertexElement::TYPE_UBYTE4 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_BLENDINDICES , 0 } ,
    { 0 , 36  , TbVertexElement::TYPE_FLOAT4 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_BLENDWEIGHT , 0 } ,
};

enum Phase
{
    PHASE_INIT , 
    PHASE_WAIT_TEX ,
};

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbResourceDataModelPmx::TbResourceDataModelPmx()
    : m_Texture(nullptr)
    , m_VertexFormat(nullptr)
    , m_Phase(PHASE_INIT)
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbResourceDataModelPmx::~TbResourceDataModelPmx()
{
    TB_SAFE_DELETE_ARRAY(m_Texture);
    TB_SAFE_DELETE_ARRAY(m_VertexFormat);
}

/*!
 * セットアップ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbResourceData::Result TbResourceDataModelPmx::Setup( const char* filePath , 
                                                      TbUint32 listType , 
                                                      TbFileSyncMode syncMode , 
                                                      const void* buf , 
                                                      TbUint32 bufSize )
{
    Result result = RESULT_INCOMPLETE;
    switch( m_Phase )
    {
    case PHASE_INIT:
        {
            m_ModelData.Load(buf,bufSize);

            // スケルトン設定
            m_Skeleton.SetData(&m_ModelData.bone);

            // テクスチャ生成
            m_Texture = TB_NEW TbResourceHandleTexture[m_ModelData.tex.count];
            for( TbUint32 i = 0; i < m_ModelData.tex.count; ++i ){
                TbStaticString256 directory;
                TbFileUtil::GetDirectoryPath(directory,filePath);
                TbStaticPath512 path;
                path.Set( directory.GetBuffer() , m_ModelData.GetTexFileName(i));
                // pngで登録されているものはtgaとして扱う
                if( path.EqualsExtension("png") ){
                    path.SetExtension("tga");
                }
                m_Texture[i].Create( path.GetBuffer() , listType ,  syncMode );
                m_Texture[i].Activate();
            }

            // 頂点フォーマット、頂点バッファ、インデックスバッファ作成
            {
                // todo:PMX共通なのだが、どこで共通化すべきか。。。
                m_VertexFormat = TB_NEW TbVertexFormat(PMX_VERTEX_ELEMENT,TB_ARRAY_COUNT_OF(PMX_VERTEX_ELEMENT));
                TbMesh::InitlaizeParam meshParam;
                meshParam.vertexFormat = m_VertexFormat;

#if !TB_PMX_BUILD_FOR_SM_3_0
                meshParam.vertexBufferCount = 1;
                meshParam.vertexBufferSizeList = &m_ModelData.vertex.vertexCount;
                meshParam.indexBufferFormat = TB_INDEX_BUFFER_FORMAT_U16;
                meshParam.indexBufferSize = sizeof(*m_ModelData.index.list)*m_ModelData.index.count;;
                m_Mesh.Initialize(meshParam);
                {
                    TbVertexPmx* vertex = TB_NEW TbVertexPmx[m_ModelData.vertex.vertexCount];
                    for( TbUint32 i = 0; i < m_ModelData.vertex.vertexCount; ++i ) {
                        vertex[i].pos = m_ModelData.vertex.list[i].pos;
                        vertex[i].norm = m_ModelData.vertex.list[i].normal;
                        vertex[i].uv = m_ModelData.vertex.list[i].uv;
                        TbUint32 boneCount = m_ModelData.vertex.list[i].GetValidBoneCount();
                        for( size_t j = 0; j < TB_ARRAY_COUNT_OF(m_ModelData.vertex.list[i].bone); ++j ){
                            if( j < boneCount ){
                                vertex[i].boneIndex[j]  = static_cast<TbUint8>(m_ModelData.vertex.list[i].bone[j]);
                                vertex[i].boneWeight[j] = m_ModelData.vertex.list[i].weight[j];
                            }else{
                                vertex[i].boneIndex[j] = 0;
                                vertex[i].boneWeight[j] = 0.0f;
                            }
                        }
                    }
                    TbUint32 bufSize = sizeof(*vertex)*m_ModelData.vertex.vertexCount;
                    m_Mesh.GetVertexBuffer(0)->Write(0,bufSize,vertex);
                    TB_SAFE_DELETE_ARRAY(vertex);
                }
                {
                    m_Mesh.GetIndexBuffer()->Write(0,bufSize,m_ModelData.index.list);
                }
#else
                // マテリアル毎に頂点バッファを
                TbUint32 startIndex = 0;
                meshParam.vertexBufferCount = m_ModelData.material.count;
                meshParam.vertexBufferSizeList = TB_NEW TbUint32[meshParam.vertexBufferCount];
                for( TbUint32 materialIndex = 0; materialIndex < m_ModelData.material.count; ++materialIndex ) {
                    TbModelDataPmx::MaterialInfo& material = m_ModelData.material.list[materialIndex];
                    meshParam.vertexBufferSizeList[materialIndex] = sizeof(TbVertexPmx)*material.indexCount;
                }
                m_Mesh.Initialize(meshParam);
                TB_SAFE_DELETE_ARRAY(meshParam.vertexBufferSizeList);

                for( TbUint32 materialIndex = 0; materialIndex < m_ModelData.material.count; ++materialIndex ) {
                    TbModelDataPmx::MaterialInfo& material = m_ModelData.material.list[materialIndex];
                    TbVertexPmx* vertex = TB_NEW TbVertexPmx[material.indexCount];
                    for( TbUint32 i = 0; i < material.indexCount; ++i ){
                        TbUint32 vertexIndex = m_ModelData.index.list[startIndex+i];
                        TbModelDataPmx::VertexInfo& vertexSrc = m_ModelData.vertex.list[vertexIndex];
                        vertex[i].pos = vertexSrc.pos;
                        vertex[i].norm = vertexSrc.normal;
                        vertex[i].uv = vertexSrc.uv;
                        TbUint32 boneCount = vertexSrc.GetValidBoneCount();
                        for( size_t j= 0; j < TB_ARRAY_COUNT_OF(vertexSrc.bone); ++j ){
                            if( j < boneCount ){
                                vertex[i].boneIndex[j] = material.GetWeigtedBoneIndex(vertexSrc.bone[j]);
                                vertex[i].boneWeight[j] = vertexSrc.weight[j];
                            }else{
                                vertex[i].boneIndex[j] = 0;
                                vertex[i].boneWeight[j] = 0.0f;
                            }
                        }
                    }
                    TbUint32 bufSize = sizeof(*vertex)*material.indexCount;
                    m_Mesh.GetVertexBuffer(materialIndex)->Write(bufSize,vertex);
                    TB_SAFE_DELETE_ARRAY(vertex);
                    startIndex += material.indexCount;
                }
#endif
            }
            m_Phase = PHASE_WAIT_TEX;
        }
        // break through
    case PHASE_WAIT_TEX:
        result = RESULT_COMPLETE;
        for( TbUint32 i = 0; i < m_ModelData.tex.count; ++i ){
            if( !m_Texture[i].IsReady() ) {
                result = RESULT_INCOMPLETE;
                break;
            }
        }
        break;
    };
    return result;
}

/*!
 * マテリアル取得(コピー取得)
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbBool TbResourceDataModelPmx::GetMaterial( TbMaterialPmx& dst , TbUint32 index )
{
    if( index < m_ModelData.material.count ) {
        dst.SetParam( m_ModelData.material.list[index] );
        dst.SetTexture( 0 , GetMaterialTexture(index) );
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * マテリアルに設定してあるテクスチャ取得
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
TbTexture* TbResourceDataModelPmx::GetMaterialTexture( TbUint32 materialIndex )
{
    TbTexture* result = nullptr;
    if( materialIndex < m_ModelData.material.count ) {
        TbUint32 texIndex = m_ModelData.material.list[materialIndex].textureIndex;
        if( texIndex < m_ModelData.GetTexCount() ){
            result = m_Texture[texIndex].GetTexture();
        }
    }
    return result;
}

}
