/*!
 * @file
 * @brief TbRenderDeviceの機種依存コード
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_DEVICE_GFX_H_
#define _INCLUDED_TB_RENDER_DEVICE_GFX_H_

#include <graphics/render/3dapi/tb_color.h>
#include <graphics/render/3dapi/tb_texture_sampler.h>
#include "graphics/render/3dapi/depend/gfx/tb_memory_pool_gfx.h"

namespace toybox
{

typedef TbColorF128 TbClearColor;
class TbRenderDevice;

/*!
 * デバイスの機種依存部分
 */
class TbRenderDeviceDepend
{
public:
    
    // コンストラクタ
    TbRenderDeviceDepend();
    
    // デストラクタ
    ~TbRenderDeviceDepend();
    
private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbRenderDeviceDepend);

public:

    // セットアップ
    TbBool Initialize();

    // キュー
    nn::gfx::Queue& GetQueue() {
        return m_Queue;
    }

    //& デバイス取得
    nn::gfx::Device& GetDevice() {
        return m_Device;
    }

    // コマンドバッファ取得
    nn::gfx::CommandBuffer& GetCommandBuffer() {
        return m_CommandBuffer;
    }

    // コマンドレコード中
    TbBool IsRecordingCommand() const {
        return m_IsRecordingCommand;
    }

    // 強制的にコマンド更新
    TbBool IsEnableCommandCache() const {
        return m_IsEnableCommandCache;
    }

private:

    // コマンドバッファ実行
    void executeCommandBuffer();

    // コマンドバッファ実行
    void beginCommandBuffer(TbRenderDevice* common);

public:    // 固有変数

    TbBitArray8             m_BitArray;             //!< ビット配列
    nn::gfx::Device         m_Device;               // 
    nn::gfx::CommandBuffer  m_CommandBuffer;        //!< コマンドバッファ
    nn::gfx::Queue          m_Queue;                //!< キュー
    nn::gfx::Fence          m_Fence;
    TbMemoryPoolGfx         m_CommandMemory;
    TbUint8*                m_ControlMemory;
    TbBool                  m_IsRecordingCommand;
    TbBool                  m_IsEnableCommandCache;

private:

    friend class TbRenderDevice;

};

// nn::gfx::Device取得関数
extern nn::gfx::Device* TbGetGfxDevice();

// コマンドバッファ取得
extern nn::gfx::CommandBuffer* TbGetGfxCommandBuffer();

}

#endif