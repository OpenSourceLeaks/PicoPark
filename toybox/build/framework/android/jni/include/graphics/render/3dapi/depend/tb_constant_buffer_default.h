/*!
 * @file
 * @brief 定数バッファ依存コード
 */

#ifndef _INCLUDED_TB_CONSTANT_BUFFER_DEFAULT_H_
#define _INCLUDED_TB_CONSTANT_BUFFER_DEFAULT_H_

namespace toybox
{

class TbConstantBufferDepend
{
    friend class TbConstantBuffer;
public:
    // コンストラクタ
    TbConstantBufferDepend(const TbSint32 size)
        : m_Buffer(nullptr) 
        , m_Size(size)
    {
        m_Buffer = TB_NEW TbUint8[m_Size];
    }
    // デストラクタ
    ~TbConstantBufferDepend() {
        TB_SAFE_DELETE_ARRAY(m_Buffer);
    }
public:

    //! Buffer取得
    TbUint8*   GetBuffer() {
        return m_Buffer;
    }
    
private:
    TbUint8*    m_Buffer;
    TbSint32    m_Size;
};

}

#endif