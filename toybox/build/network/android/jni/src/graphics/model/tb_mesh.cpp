/*!
 * PMX用メッシュ
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#include <tb_fwd.h>
#include <graphics/model/tb_mesh.h>
#include <graphics/render/3dapi/tb_vertex_buffer.h>
#include <graphics/render/3dapi/tb_index_buffer.h>
#include <graphics/render/3dapi/tb_vertex_format.h>
#include <base/io/tb_file_util.h>
#include <base/io/tb_path.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbMesh::TbMesh()
    : m_VertexFormat(nullptr)
    , m_VertexBufferCount(0)
    , m_VertexBuffer(nullptr) // 頂点バッファ
    , m_IndexBuffer(nullptr)  // インデックスバッファ
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.07
 */
TbMesh::~TbMesh()
{
    TB_SAFE_DELETE(m_VertexFormat);
    for( TbUint32 i = 0; i < m_VertexBufferCount; ++i ){
        TB_SAFE_DELETE(m_VertexBuffer);
    }
    TB_SAFE_DELETE_ARRAY(m_VertexBuffer);
    TB_SAFE_DELETE(m_IndexBuffer);
}

/*!
 * 初期化
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbMesh::Initialize( const InitlaizeParam& param )
{
    if( !TB_VERIFY(!m_VertexBuffer&&!m_IndexBuffer) ) {
        return;
    }
    m_VertexFormat = param.vertexFormat;
    TB_ASSERT(param.vertexBufferCount>0);
    m_VertexBufferCount = param.vertexBufferCount;
    m_VertexBuffer = TB_NEW TbVertexBuffer*[m_VertexBufferCount];
    for( TbUint32 i = 0; i < m_VertexBufferCount; ++i ) {
        m_VertexBuffer[i] = TB_NEW TbVertexBuffer( param.vertexFormat , param.vertexBufferSizeList[i] );
    }
    if( param.indexBufferSize > 0 ){
        m_IndexBuffer = TB_NEW TbIndexBuffer( param.indexBufferFormat , param.indexBufferSize );
    }
}

}
