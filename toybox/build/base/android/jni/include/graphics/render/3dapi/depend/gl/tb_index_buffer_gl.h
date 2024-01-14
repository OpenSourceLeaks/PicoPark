/*!
 * @file
 * @brief OpenGLのインデックスバッファ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_INDEX_BUFFER_GL_H_
#define _INCLUDED_TB_INDEX_BUFFER_GL_H_

namespace toybox
{

/*!
 * 頂点バッファ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbIndexBufferDepend
{
public:

    // フォーマットをGLenumへ変換
    static TbResult ConvertFormatToGLenum( GLenum& dst , const TbIndexBufferFormat format );
 
public:

    // コンストラクタ
    TbIndexBufferDepend( const TbSint32 size );
    // デストラクタ
    ~TbIndexBufferDepend();

public:

    //! バッファ取得
    TbUint8* GetBuffer() { return m_Buffer; }

    //! VBO(Vertex Buffer Object)のID取得
    GLuint   GetVboId() const { return m_VboId; }

private:

    TbUint8* m_Buffer;  //!< バッファ
    TbUint32 m_LockBufferSize; //!< ロックしたバッファサイズ
    GLuint   m_VboId;     //!< VBO

private:

    friend class TbIndexBuffer;

};

}

#endif