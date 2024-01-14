/*!
 * GPUアクセス用バッファ
 * @author teco
 */

#ifndef _INCLUDED_TB_GPU_BUFFER_GFX_H_
#define _INCLUDED_TB_GPU_BUFFER_GFX_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/depend/gfx/tb_memory_pool_gfx.h>

namespace toybox
{

/*!
 * 頂点フォーマット依存コード
 * @author teco
 */
class TbGpuBufferGfx
{
public:

    struct InitParam
    {
        InitParam()
            : cpuAccesible(TB_FALSE)
            , accessFlag(0)
            , bufferSize(0)
        {}
        TbBool      cpuAccesible;
        TbUint32    accessFlag;
        TbUint32    bufferSize;
    };

public:
    
    // コンストラクタ
    TbGpuBufferGfx();
    
    // デストラクタ
    ~TbGpuBufferGfx();
    
public:
    
    // 初期化
    void Intialize( const InitParam& param );

    // 後始末
    void Finalize();

public:

    // マップ
    void* Map();

    // アンマップ
    void Unmap();

    // サイズ取得
    TbSizeT GetSize() const {
        return m_Size;
    }

public:

    // 読み込み
    TbResult Read( void* readPtr, TbSizeT offset ,  TbSizeT size );

    // 書き込み
    TbResult Write( const void* writePtr, TbUint32 offset , TbSizeT size );

public:

    // GPUアドレス取得
    void GetGpuAddress(nn::gfx::GpuAddress* gpuAddress)
    {
        m_Buffer.GetGpuAddress(gpuAddress);
    }
    
public:
    
    nn::gfx::Buffer     m_Buffer;
    TbMemoryPoolGfx     m_MemoryPool;
    InitParam           m_InitParam;
    TbSizeT             m_Size;
    TbSizeT             m_Align;

    void*               m_MappedPtr;
    
};
    

}

#endif