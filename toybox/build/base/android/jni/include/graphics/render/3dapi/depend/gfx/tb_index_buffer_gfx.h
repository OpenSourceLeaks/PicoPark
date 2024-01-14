/*!
 * @file
 * @brief GX2のインデックスバッファ依存コード
 */

#ifndef _INCLUDED_TB_INDEX_BUFFER_GFX_H_
#define _INCLUDED_TB_INDEX_BUFFER_GFX_H_

#include "tb_gpu_buffer_gfx.h"

namespace toybox
{

class TbIndexBufferDepend
{
    friend class TbIndexBuffer;
public:
    // コンストラクタ
    TbIndexBufferDepend(TbIndexBufferFormat format, const TbSint32 size);
    // デストラクタ
    ~TbIndexBufferDepend();
public:
    //! Buffer取得
    TbGpuBufferGfx*   GetBuffer() {
        return &m_Buffer;
    }
    nn::gfx::IndexFormat GetFormat() const {
        return m_Format;
    }
private:
    TbGpuBufferGfx m_Buffer;
    nn::gfx::IndexFormat    m_Format;
};

}

#endif