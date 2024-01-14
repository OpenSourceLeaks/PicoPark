/*!
 * @file
 * @brief GX2のインデックスバッファ依存コード
 */

#ifndef _INCLUDED_TB_INDEX_BUFFER_GX2_H_
#define _INCLUDED_TB_INDEX_BUFFER_GX2_H_

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

    // フォーマットからバイト数取得
    static TbResult GetByteCountFromFormat( TbUint32& dst , const TbIndexBufferFormat format );
 
public:

    // コンストラクタ
    TbIndexBufferDepend( TbIndexBufferFormat format , const TbSint32 size );
    // デストラクタ
    ~TbIndexBufferDepend();

public:

    //! バッファ取得
    GX2RBuffer* GetBuffer() { return m_Buffer; }

private:

    GX2RBuffer   m_Buffer;

private:

    friend class TbIndexBuffer;

};

}

#endif