/*!
 * @file
 * @brief インデックスバッファのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_INDEX_BUFFER_STD_DX9_H_
#define _INCLUDED_TB_INDEX_BUFFER_STD_DX9_H_

#include "./tb_lost_resource_dx9.h"

namespace toybox
{

class TbRenderDevice;

/*!
 * 頂点バッファ
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
class TbIndexBufferDepend : public TbLostResourceDx9
{
public:

    // フォーマットをD3DFORMATへ変換
    static TbResult ConvertFormatToD3DFORMAT( D3DFORMAT& dst , const TbIndexBufferFormat format );

public:
    
    // コンストラクタ
    TbIndexBufferDepend( const TbIndexBufferFormat format , const TbSint32 size );

    //デストラクタ
    ~TbIndexBufferDepend();

public: // 依存関数(アプリケーション層からは原則呼ばない)

    //! 生データ取得
    IDirect3DIndexBuffer9* GetRawData() const { 
        return m_IndexBufferDx9; 
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
    
    IDirect3DIndexBuffer9*  m_IndexBufferDx9;   //!< DirectX用生インデックスバッファ
    TbSint32                m_Size;             //!< サイズ
    D3DFORMAT               m_Format;              //!< FVF
    TbUint8*                m_BackupBuffer;     //!< デバイスロスト用バックアップバッファ

private:
    friend class TbRenderDevice;
    friend class TbIndexBuffer;

};

}

#endif