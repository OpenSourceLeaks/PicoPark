/*!
 * @file
 * @brief GX2の頂点バッファ依存コード
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_GX2_H_
#define _INCLUDED_TB_VERTEX_BUFFER_GX2_H_

namespace toybox
{

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
    const GX2RBuffer*   GetBuffer() const { return &m_Buffer; }
private:
    GX2RBuffer   m_Buffer;
    TbUint32     m_ElementSize;
};

}

#endif