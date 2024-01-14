/*!
 * PMX用メッシュ
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#include <tb_fwd.h>
#include "tb_mesh_pmx.h"
#include "tb_pmx_data.h"
#include <graphics/render/3dapi/tb_vertex_buffer.h>
#include <graphics/render/3dapi/tb_index_buffer.h>
#include <graphics/render/3dapi/tb_vertex_format.h>
#include <base/io/tb_file_util.h>
#include <base/io/tb_path.h>

namespace toybox
{

namespace
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

}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbMeshPmx::TbMeshPmx()
    : m_VertexFormat(NULL)
    , m_VertexBufferCount(0)
    , m_VertexBuffer(NULL) // 頂点バッファ
    , m_IndexBuffer(NULL)  // インデックスバッファ
    , m_Texture(NULL)      // テクスチャ
    , m_ModelData(NULL)    // モデルデータ
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbMeshPmx::~TbMeshPmx()
{
    TB_SAFE_DELETE(m_VertexFormat);
    if( m_ModelData ) {
        for( TbUint32 i = 0; i < m_VertexBufferCount; ++i ){
            TB_SAFE_DELETE(m_VertexBuffer);
        }
        TB_SAFE_DELETE_ARRAY(m_VertexBuffer);
    }else{
        TB_ASSERT(!m_VertexBuffer);
    }
    TB_SAFE_DELETE(m_IndexBuffer);
    TB_SAFE_DELETE_ARRAY(m_Texture);
}

/*!
 * 初期化
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbBool TbMeshPmx::Initialize( TbPmxData* data , const char* directory )
{
    if( !TB_VERIFY(!m_ModelData && data) ){
        // 多重初期化
        return TB_FALSE;
    }

    m_ModelData = data;

    // テクスチャ生成
    m_Texture = TB_NEW TbResourceHandleTexture[m_ModelData->material.count];
    for( TbUint32 i = 0; i < m_ModelData->material.count; ++i ){
        TbUint32 texIndex = m_ModelData->material.list[i].textureIndex;
        if( m_ModelData->IsValidTexIndex(texIndex) )
        {
            TbStaticPath256 path;
            path.Set( directory , m_ModelData->GetTexFileName(texIndex));
            // pngで登録されているものはtgaとして扱う
            if( path.EqualsExtension("png") ){
                path.SetExtension("tga");
            }
            m_Texture[i].Create( path.GetBuffer() );
            m_Texture[i].Activate();
        }
    }

    // 頂点フォーマット、頂点バッファ、インデックスバッファ作成
    {
        // todo:PMX共通なのだが、どこで共通化すべきか。。。
        m_VertexFormat = TB_NEW TbVertexFormat(PMX_VERTEX_ELEMENT,TB_ARRAY_COUNT_OF(PMX_VERTEX_ELEMENT));

#if !TB_PMX_BUILD_FOR_SM_3_0
        {
            TbPmxVertex* vertex = TB_NEW TbPmxVertex[m_ModelData->vertex.vertexCount];
            for( TbUint32 i = 0; i < m_ModelData->vertex.vertexCount; ++i ) {
                vertex[i].pos = m_ModelData->vertex.vertex[i].pos;
                vertex[i].norm = m_ModelData->vertex.vertex[i].normal;
                vertex[i].uv = m_ModelData->vertex.vertex[i].uv;
                TbUint32 boneCount = m_ModelData->vertex.vertex[i].GetValidBoneCount();
                for( size_t j = 0; j < TB_ARRAY_COUNT_OF(m_ModelData->vertex.vertex[i].bone); ++j ){
                    if( j < boneCount ){
                        vertex[i].boneIndex[j]  = static_cast<TbUint8>(m_ModelData->vertex.vertex[i].bone[j]);
                        vertex[i].boneWeight[j] = m_ModelData->vertex.vertex[i].weight[j];
                    }else{
                        vertex[i].boneIndex[j] = 0;
                        vertex[i].boneWeight[j] = 0.0f;
                    }
                }
            }
            TbUint32 bufSize = sizeof(*vertex)*m_ModelData->vertex.vertexCount;
            m_VertexBuffer = TB_NEW TbVertexBuffer*[1];
            m_VertexBuffer[0] = TB_NEW TbVertexBuffer( m_VertexFormat , bufSize );
            m_VertexBuffer[0]->Write(0,bufSize,vertex);
            TB_SAFE_DELETE_ARRAY(vertex);
        }
        {
            TbUint32 bufSize = sizeof(*m_ModelData->index.list)*m_ModelData->index.count;
            m_IndexBuffer = TB_NEW TbIndexBuffer(TB_INDEX_BUFFER_FORMAT_U16,bufSize);
            m_IndexBuffer->Write(0,bufSize,m_ModelData->index.list);
        }
#else
        // マテリアル毎に頂点バッファを
        TbUint32 startIndex = 0;
        m_VertexBufferCount = m_ModelData->material.count;
        m_VertexBuffer = TB_NEW TbVertexBuffer*[m_VertexBufferCount];
        for( TbUint32 materialIndex = 0; materialIndex < m_ModelData->material.count; ++materialIndex ) {
            TbPmxMaterialInfo& material = m_ModelData->material.list[materialIndex];
            TbPmxVertex* vertex = TB_NEW TbPmxVertex[material.indexCount];
            for( TbUint32 i = 0; i < material.indexCount; ++i ){
                TbUint32 vertexIndex = m_ModelData->index.list[startIndex+i];
                TbPmxVertexInfo& vertexSrc = m_ModelData->vertex.vertex[vertexIndex];
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
            m_VertexBuffer[materialIndex] = new TbVertexBuffer( m_VertexFormat , bufSize );
            m_VertexBuffer[materialIndex]->Write(0,bufSize,vertex);
            TB_SAFE_DELETE_ARRAY(vertex);
            startIndex += material.indexCount;
        }
#endif
    }

    return TB_TRUE;
}

/*!
 * ビジー状態か
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbBool TbMeshPmx::IsBusy() const
{
    if( !m_ModelData ){
        return TB_FALSE;
    }
    for( TbUint32 i = 0; i < m_ModelData->material.count; ++i ){
        if( !m_Texture[i].IsBusy() ){
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 初期化完了しているか
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbBool TbMeshPmx::IsReady() const 
{
    if( !m_ModelData ){
        return TB_FALSE;
    }
    for( TbUint32 i = 0; i < m_ModelData->material.count; ++i ){
        if( !m_Texture[i].IsReady() ){
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}

/*!
 * 頂点バッファ取得
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbVertexBuffer* TbMeshPmx::GetVertexBuffer( TbUint32 materialIndex )
{
    if( !m_ModelData ){
        return NULL;
    }
    TbVertexBuffer* result = NULL;
    if( materialIndex < m_ModelData->material.count ){
        result = m_VertexBuffer[materialIndex];
    }
    return result;
}

/*!
 * テクスチャ取得
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbTexture* TbMeshPmx::GetTexture( TbUint32 index )
{
    TbTexture* result = NULL;
    if( index < GetTextureCount() ){
        result = m_Texture[index].GetTexture();
    }
    return result;
}

/*!
 * テクスチャ数取得
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbUint32 TbMeshPmx::GetTextureCount()
{
    if( !m_ModelData ){
        return 0;
    }
    return m_ModelData->tex.count;
}

}
