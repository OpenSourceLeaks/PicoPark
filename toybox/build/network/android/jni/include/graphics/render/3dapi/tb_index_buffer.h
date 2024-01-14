/*!
 * @file
 * @brief インデックスバッファのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_INDEX_BUFFER_H_
#define _INCLUDED_TB_INDEX_BUFFER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

// 依存コード
#include "./depend/tb_index_buffer_depend.h"

namespace toybox
{

class TbRenderDevice;

/*!
 * 頂点バッファクラス
 * @author teco
 */
class TbIndexBuffer
{
public:

    typedef TbIndexBuffer Self;

public:

    // フォーマットからバイト数取得
    static TbResult GetByteCountFromFormat(TbUint32& dst, const TbIndexBufferFormat format)
    {
        static const TbUint32 TABLE[] =
        {
            2 ,
            4 ,
        };
        if (0 <= format && format < TB_INDEX_BUFFER_FORMAT_MAX) {
            dst = TABLE[format];
            return TB_S_OK;
        }
        return TB_E_FAIL;

    }

public:
    
    // コンストラクタ( デバイス直接指定 )
    TbIndexBuffer( const TbIndexBufferFormat format , const TbSint32 size );

    //デストラクタ
    ~TbIndexBuffer();

public:

    // フォーマット取得
    TbIndexBufferFormat GetFormat() const { 
        return m_Format; 
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
    TB_INLINE TbIndexBufferDepend& GetDepend() { 
        return m_Depend; 
    }

    // 依存コード取得
    TB_INLINE const TbIndexBufferDepend& GetDepend() const { 
        return m_Depend; 
    }

private:
    
    TbSint32 m_Size;                           //!< サイズ
    TbIndexBufferFormat m_Format;              //!< フォーマット 
    TbIndexBufferDepend m_Depend;              //!< 依存コード

};

}

#endif