/*!
 * @file
 * @brief OpenGLの頂点バッファ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_STD_GL_H_
#define _INCLUDED_TB_VERTEX_BUFFER_STD_GL_H_

namespace toybox
{

/*!
 * 頂点バッファ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbVertexBufferDepend
{
public:
    // コンストラクタ
    TbVertexBufferDepend( const TbSint32 size );
    // デストラクタ
    ~TbVertexBufferDepend();
public:
    //! VBO(Vertex Buffer Object)のID取得
    GLuint   GetVboId() const { return m_VboId; }
private:
    GLuint   m_VboId;     //!< VBO
private:
    friend class TbVertexBuffer;
};

}

#endif