/*!
 * ストリーム再生用バッファ
 * @author Miyake Shunsuke
 * @since 2012.04.01
 */

#ifndef _INCLUDED_TB_SOUND_STREAM_BUFFER_H_
#define _INCLUDED_TB_SOUND_STREAM_BUFFER_H_

namespace toybox
{

class TbSoundStreamBuffer
{
public:

    static const TbUint32 FPS_PER_BLOCK = 60; // 1ブロックのフレームレート
    static const TbUint32 FIRST_BLOCK_COUNT = FPS_PER_BLOCK / 2;
    static const TbUint32 RING_BLOCK_COUNT= FPS_PER_BLOCK / 2;
    static const TbFloat32 SEC_PER_BLOCK;

public:

    // バッファサイズ取得
    static TbUint32 GetBufferSize( TbFloat32 sec , TbUint32 freq , TbUint32 bitPerSample , TbUint32 channel ) {
        TbFloat32 bytePerSecond = static_cast<TbFloat32>(freq * bitPerSample * channel) / 8.0f;
        TbUint32 result = static_cast<TbUint32>(bytePerSecond * sec);
        return result;
    }

    // バッファサイズ取得
    static TbUint32 GetBlockBufferSize( TbUint32 freq , TbUint32 bitPerSample , TbUint32 channel ) {
        TbUint32 bitPerSecond = freq * bitPerSample * channel;
        TbUint32 bytePerBlock = static_cast<TbUint32>(SEC_PER_BLOCK * static_cast<TbFloat32>(bitPerSecond) / 8.0f);
        TbUint32 result = TB_ALIGN_ROUND_UP(bytePerBlock,128);   // 1/60秒のサイズ
        return result;
    }

    // 再生はじめに必要なバッファサイズ
    static TbUint32 GetFirstBufferSize( TbUint32 freq , TbUint32 bitPerSample , TbUint32 channel ) {
        TbUint32 result = GetBlockBufferSize(freq,bitPerSample,channel) * FIRST_BLOCK_COUNT;
        return result;
    }

public:

    // コンストラクタ
    TbSoundStreamBuffer();

    // デストラクタ
    ~TbSoundStreamBuffer();

public:

    // セットアップ
    void Setup( TbUint32 freq , TbUint32 bitPerSample , TbUint32 channel );

public:

    // 現在のブロックバッファサイズ取得
    TbUint32 GetBlockBufferSize() const {
        return m_BlockBufferSize;
    }

    // はじめのブロックバッファ取得
    TbUint8* GetFirstBlockBuffer( TbUint32 index );

    // 現在の循環用ブロックバッファ取得
    TbUint8* GetCurrentRingBlockBuffer();

    //! 現在の循環用ブロックバッファインデックス
    TbUint32 GetCurrentRingBlockIndex() const {
        return m_CurrentIndex;
    }

    // 循環用ブロックインデックスを進める
    void IncreaseRingBlockIndex();

    // 循環用ブロックインデックスをリセット
    void ResetRingBlockIndex() {
        m_CurrentIndex = 0;
    }

private:

    TbUint8* m_FirstBlock[FIRST_BLOCK_COUNT];       // バッファ
    TbUint8* m_RingBlock[RING_BLOCK_COUNT];         // バッファ
    TbUint32 m_BlockBufferSize;                     // ブロックバッファサイズ
    TbUint32 m_FirstBufferSize;                     // 開始バッファサイズ
    TbUint32 m_CurrentIndex;                        // 現在インデックス値

};

}

#endif
