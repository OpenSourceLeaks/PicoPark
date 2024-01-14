/*!
 * NX依存のサウンド音源クラス
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_NX_H_
#define _INCLUDED_TB_SOUND_SOURCE_NX_H_

#include <sound/tb_sound_types.h>
#include <base/container/tb_static_array.h>
#include <base/container/tb_static_deque.h>

namespace toybox
{

class TbSoundSourceDepend
{
public:
    friend class TbSoundSource;

    class Voice
    {
    public:
        // コンストラクタ
        Voice( const TbSoundFormat& format );
        // デストラクタ
        ~Voice();
        // デリーと可能か
        TbBool IsEnableDelete() const;
        // Nativeデータ取得
        nn::audio::VoiceType* GetNative() {
            return &m_Voice;
        }
        // WaveBuffer取得
        nn::audio::WaveBuffer* GetWaveBuffer( TbUint32 index ) {
            if (index < TB_ARRAY_COUNT_OF(m_WaveBuffer)) {
                return &m_WaveBuffer[index];
            }
            return nullptr;
        }
    public:
        TbBool                   m_IsInitialized;
        nn::audio::VoiceType     m_Voice;
        nn::audio::WaveBuffer    m_WaveBuffer[nn::audio::VoiceType::WaveBufferCountMax];
    };

public:

    // コンストラクタ
    TbSoundSourceDepend();

    // デストラクタ
    ~TbSoundSourceDepend();

public:

    // リセット
    void Reset();

    // セットアップ
    void Setup( const TbSoundResourceInfo& bufferInfo );

public:

    // ボイス取得
    nn::audio::VoiceType* GetVoice() {
        if(m_Voice){
            return m_Voice->GetNative();
        }
        return nullptr;
    }

    // ボイス取得
    const nn::audio::VoiceType* GetVoice() const {
        if (m_Voice) {
            return m_Voice->GetNative();
        }
        return nullptr;
    }

private:

    struct BlockBufferInfo {
        const void* buffer;
        TbUint32 size;
        TbBool   isEnd;
    };

    typedef TbStaticDeque<BlockBufferInfo,TbSoundStreamBuffer::RING_BLOCK_COUNT> BlockBufferDeque;
    typedef TbStaticDeque<nn::audio::WaveBuffer*, nn::audio::VoiceType::WaveBufferCountMax> WaveBufferDeque;

private:

    // WaveBuffer取得
    nn::audio::WaveBuffer* createWaveBuffer(const void* buffer, TbUint32 size, TbBool isEnd , const TbSoundFormat& format);

private: // ストリーミング用

    // ストリーム再生終了処理
    void stopStream();

    // ストリーム再生更新
    void updatePlayingStream( const TbSoundFormat& format );

    // 利用可能なブロックバッファを積む
    TbResult pushReadyStreamBlock( const void* buffer , TbUint32 size , TbBool isEnd );

    // キューから終了したバッファを取り除く
    TbResult tryPopStreamQueue();

private:

    Voice*                   m_Voice;
    WaveBufferDeque          m_FreeWaveBufferDeque;

private:

    TbUint32                 m_StreamProcessedCount; // ストリーミングで再生完了したブロック数
    TbSoundFormat            m_Format;
    TbUint32                 m_WaveBufferCount;
    BlockBufferDeque         m_StreamBlockDeque;         // 利用可能なブロックバッファ

};

}

#endif
