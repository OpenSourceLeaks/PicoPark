/*!
 * @brief constant buffer
 * @author teco
 */

#ifndef _INCLUDED_TB_CONSANT_BUFFER_H_
#define _INCLUDED_TB_CONSANT_BUFFER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <base/math/tb_matrix.h>

// 依存コード
#include "./depend/tb_constant_buffer_depend.h"

namespace toybox
{

class TbRenderDevice;

/*!
 * 定数バッファ
 * @author teco
 */
class TbConstantBuffer
{
public:

    typedef TbConstantBuffer Self;

public:
    
    // コンストラクタ( デバイス直接指定 )
    TbConstantBuffer( const TbSizeT size );

    //デストラクタ
    ~TbConstantBuffer();

public:

    // 書き込み
    TbBool Write(const TbUint32 offset, const TbMatrix& matrix) {
        TbMatrix mtx = matrix;
#if TB_SHADER_MATRIX_IS_TRANSPOSE
        mtx.Transpose();
#endif
        return Write(offset,sizeof(mtx),&mtx);
    }

    // 書き込み
    TbBool Write( const TbUint32 offset , const TbUint32 size , const void* src );

    // ロック
    TbBool Lock( const TbUint32 offset , const TbUint32 size , void** dst );

    // ロック解除
    void Unlock();
    
    // サイズ取得
    TbSint32 GetSize() const {
        return m_Size;
    }

public:

    // 依存コード取得
    TB_INLINE TbConstantBufferDepend& GetDepend() {
        return m_Depend; 
    }

    // 依存コード取得
    TB_INLINE const TbConstantBufferDepend& GetDepend() const {
        return m_Depend; 
    }

private:
    
    TbSint32                m_Size;     //!< サイズ
    TbConstantBufferDepend  m_Depend;   //!< 依存コード

};

}

#endif