 /*!
* Stage3Dの頂点バッファ(Stage3D依存)
 * @author Miyake Shunsuke
 * @since 2013.04.05
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_S3D_H_
#define _INCLUDED_TB_VERTEX_BUFFER_S3D_H_

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
public:
    
    // コンストラクタ
    TbVertexBufferDepend( const TbVertexFormat* format , const TbUint32 size );
    
    // コンストラクタ
    TbVertexBufferDepend( TbUint32 vertexCount , TbUint32 sizePerVertex );
    
public:
    
    //! バッファ取得
    AS3::ui::flash::display3D::VertexBuffer3D& GetVertexBuffer() { 
        return m_VertexBuffer; 
    }

private:
    AS3::ui::flash::display3D::VertexBuffer3D  m_VertexBuffer;
    TbUint32                                   m_SizePerVertex;
private:
    friend class TbVertexBuffer;
};

}

#endif