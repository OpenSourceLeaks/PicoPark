/*!
 * @file
 * @brief GX2の頂点バッファ依存コード
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_GFX_H_
#define _INCLUDED_TB_VERTEX_BUFFER_GFX_H_

#include "tb_gpu_buffer_gfx.h"

namespace toybox
{

class TbVertexFormat;

/*!
 * 頂点バッファ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbVertexBufferDepend
{
    friend class TbVertexBuffer;
public:
    // コンストラクタ
    TbVertexBufferDepend( const TbVertexFormat* format ,  const TbSint32 size );
    // デストラクタ
    ~TbVertexBufferDepend();
public:
    //! Buffer取得
    TbGpuBufferGfx*   GetBuffer() { 
        return &m_Buffer; 
    }
private:
    TbGpuBufferGfx m_Buffer; 
};

}

#endif