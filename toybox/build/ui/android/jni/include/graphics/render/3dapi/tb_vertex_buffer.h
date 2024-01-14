/*!
 * @file
 * @brief 頂点バッファのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_H_
#define _INCLUDED_TB_VERTEX_BUFFER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

// 依存コード
#include "./depend/tb_vertex_buffer_depend.h"

namespace toybox
{

class TbRenderDevice;
class TbVertexFormat;

/*!
 * 頂点バッファクラス
 * @author Miyake Shunsuke
 * @since 2010.06.16
 */
class TbVertexBuffer
{
public:
    
    // コンストラクタ(フォーマット指定バージョン、参照先にストリーム番号が全て0である必要がある)
    TbVertexBuffer( const TbVertexFormat* format , const TbSint32 size );
    
    // コンストラクタ(頂点数と頂点毎のサイズ指定バージョン)
    TbVertexBuffer( TbUint32 vertexCount , TbUint32 sizePerVertex );

    //デストラクタ
    ~TbVertexBuffer();

public:

    // サイズ取得
    TbSint32 GetSize() const {
        return m_Size;
    }

public:

    // 書き込み
    TbBool Write( const TbUint32 size , const void* src );

public:

    // 依存コード取得
    TB_INLINE TbVertexBufferDepend& GetDepend() { 
        return m_Depend; 
    }
    
    // 依存コード取得
    TB_INLINE const TbVertexBufferDepend& GetDepend() const { 
        return m_Depend; 
    }

private:
    
    TbSint32 m_Size;                           //!< サイズ
    TbVertexBufferDepend m_Depend;             //!< 依存コード

};

}

#endif