/*!
 * @file
 * @brief 頂点バッファのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_DX9_H_
#define _INCLUDED_TB_VERTEX_BUFFER_DX9_H_

#include "./tb_lost_resource_dx9.h"

namespace toybox
{

class TbRenderDevice;
class TbVertexFormat;

/*!
 * 頂点バッファ
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
class TbVertexBufferDepend : public TbLostResourceDx9
{
public:
    
    // コンストラクタ
    TbVertexBufferDepend( const TbVertexFormat* format , const TbSint32 size );

    //デストラクタ
    ~TbVertexBufferDepend();

public: // 依存関数(アプリケーション層からは原則呼ばない)

    //! 生データ取得
    LPDIRECT3DVERTEXBUFFER9 GetRawData() const { 
        return m_VertexBuffer; 
    }

    //! バックアップバッファ取得
    TbUint8* GetBackupBuffer() const { 
        return m_BackupBuffer; 
    }

public: // 継承関数

    // 解放
    virtual void Release();

    // 再構築
    virtual void Recover();

private:

    // 書き込み
    void write( const TbUint32 offset , const TbUint32 size , const void* src );

private:
    
    TbRenderDevice*         m_Device;           //!< 所属デバイス
    LPDIRECT3DVERTEXBUFFER9 m_VertexBuffer;     //!< DirectX用生頂点バッファ
    TbSint32                m_Size;             //!< サイズ
    TbSint32                m_FVF;              //!< FVF
    TbUint8*                m_BackupBuffer;     //!< デバイスロスト用バックアップバッファ

private:
    friend class TbRenderDevice;
    friend class TbVertexBuffer;
};

}

#endif