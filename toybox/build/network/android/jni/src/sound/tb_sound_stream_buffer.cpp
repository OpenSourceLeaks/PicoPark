/*!
 * ストリーム再生用バッファ
 * @author Miyake Shunsuke
 * @since 2012.04.01
 */

#include "tb_fwd.h"
#include "sound/tb_sound_stream_buffer.h"
#include "sound/tb_sound_manager.h"

namespace toybox
{

const TbFloat32 TbSoundStreamBuffer::SEC_PER_BLOCK = 1.0f/static_cast<TbFloat32>(TbSoundStreamBuffer::FPS_PER_BLOCK);

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundStreamBuffer::TbSoundStreamBuffer()
    : m_RingBuffer(nullptr)
    , m_BlockBufferSize(0U)
    , m_CurrentIndex(0U)
{
    TbFillNull(m_FirstBlock);
    TbFillNull(m_RingBlock);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundStreamBuffer::~TbSoundStreamBuffer()
{
    TB_SAFE_DELETE(m_RingBuffer);
    TbSoundManager& manager = TbSoundManager::GetInstance();
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_FirstBlock); ++i) {
        if(m_FirstBlock[i]){
            manager.FreeBuffer(m_FirstBlock[i]);
        }
    }
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_RingBlock); ++i) {
        if (m_RingBlock[i]) {
            manager.FreeBuffer(m_RingBlock[i]);
        }
    }
}

/*!
 * セットアップ_
 * @param freq    周波数
 * @param bps     bit per sample
 * @param channel チャンネル数
 * @author teco
 */
void TbSoundStreamBuffer::Setup( TbUint32 freq , TbUint32 bitPerSample , TbUint32 channel )
{
    m_BlockBufferSize = GetBlockBufferSize(freq,bitPerSample,channel);
    TbSoundManager& manager = TbSoundManager::GetInstance();
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_FirstBlock); ++i) {
        m_FirstBlock[i] = reinterpret_cast<TbUint8*>(manager.AllocBuffer(m_BlockBufferSize, TB_SOUND_BUFFER_ALIGN_SIZE));
    }
    m_RingBuffer = new TbUint8[m_BlockBufferSize*RING_BLOCK_COUNT];
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_RingBlock); ++i) {
        m_RingBlock[i] = reinterpret_cast<TbUint8*>(manager.AllocBuffer(m_BlockBufferSize, TB_SOUND_BUFFER_ALIGN_SIZE));
    }
}

/*!
 * はじめのブロックバッファ取得
 * @author teco
 */
TbUint8* TbSoundStreamBuffer::GetFirstBlockBuffer( TbUint32 index )
{
    if (TB_ARRAY_COUNT_OF(m_FirstBlock) <= index) {
        return nullptr;
    }
    return m_FirstBlock[index];
}

/*!
 * 現在のブロックバッファ取得
 * @author teco
 */
TbUint8* TbSoundStreamBuffer::GetCurrentRingBlockBuffer()
{
    if (TB_ARRAY_COUNT_OF(m_RingBlock) <= m_CurrentIndex) {
        return nullptr;
    }
    return m_RingBlock[m_CurrentIndex];
}

/*!
 * ブロックインデックスを進める
 * @author teco
 */
void TbSoundStreamBuffer::IncreaseRingBlockIndex()
{
    m_CurrentIndex = (m_CurrentIndex+1) % RING_BLOCK_COUNT;
}

}

