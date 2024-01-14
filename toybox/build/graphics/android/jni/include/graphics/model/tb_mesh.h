/*!
 * メッシュクラス
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#ifndef _INCLUDED_TB_MESH_H_
#define _INCLUDED_TB_MESH_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{

class TbVertexFormat;
class TbVertexBuffer;
class TbIndexBuffer;

class TbMesh
{
public:

    struct InitlaizeParam
    {
        InitlaizeParam() 
            : vertexFormat(nullptr) 
            , indexBufferFormat(TB_INDEX_BUFFER_FORMAT_U16) 
            , vertexBufferCount(0)
            , vertexBufferSizeList(nullptr)
            , indexBufferSize(0)
        {}
        TbVertexFormat*     vertexFormat;
        TbIndexBufferFormat indexBufferFormat;
        TbUint32            vertexBufferCount;
        TbUint32*           vertexBufferSizeList;
        TbUint32            indexBufferSize;
    };

public:

    // コンストラクタ
    TbMesh();

    // デストラクタ
    ~TbMesh();

public:

    // 初期化
    void Initialize( const InitlaizeParam& param );

public:

    // 頂点フォーマット取得
    TbVertexFormat* GetVertexFormat() {
        return m_VertexFormat;
    }

    // 頂点バッファ数取得
    TbUint32 GetVertexBufferCount() const {
        return m_VertexBufferCount;
    }

    // 頂点バッファ取得
    TbVertexBuffer* GetVertexBuffer( TbUint32 index = 0 ) {
        if( index < m_VertexBufferCount ) {
            return m_VertexBuffer[index];
        }
        return nullptr;
    }

    // インデックスバッファ取得
    TbIndexBuffer* GetIndexBuffer() {
        return m_IndexBuffer;
    }

private:

    TbVertexFormat*             m_VertexFormat;      // 頂点フォーマット
    TbUint32                    m_VertexBufferCount; // 頂点バッファ数
    TbVertexBuffer**            m_VertexBuffer;      // 頂点バッファ
    TbIndexBuffer*              m_IndexBuffer;       // インデックスバッファ

};

}

#endif
