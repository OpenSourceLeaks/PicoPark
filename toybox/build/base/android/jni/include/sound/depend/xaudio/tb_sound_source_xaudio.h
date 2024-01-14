/*!
 * XAudio依存のサウンド音源クラス
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_XAUDIO_H_
#define _INCLUDED_TB_SOUND_SOURCE_XAUDIO_H_

#include <base/container/tb_static_array.h>
#include <base/container/tb_static_deque.h>

namespace toybox
{

class TbSoundSourceDepend
{
    friend class TbSoundSource;
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
    IXAudio2SourceVoice* GetVoice() {
        return m_SourceVoice;
    }

    // ボイス取得
    const IXAudio2SourceVoice* GetVoice() const {
        return m_SourceVoice;
    }

private:

    struct BlockBufferInfo {
        const void* buffer;
        TbUint32 size;
    };

    typedef TbStaticDeque<BlockBufferInfo,TbSoundStreamBuffer::RING_BLOCK_COUNT> BlockBufferDeque;

private: // ストリーミング用

    // ストリーム再生終了処理
    void stopStream();

    // ストリーム再生更新
    void updatePlayingStream();

    // 利用可能なブロックバッファを積む
    TbResult pushReadyStreamBlock( const void* buffer , TbUint32 size, TbBool isEnd = TB_FALSE );

    // キューから終了したバッファを取り除く
    TbResult tryPopStreamQueue();

    // ストリームの処理完了数
    TbUint32 getStreamProcessedCount() const {
        return m_StreamProcessedCount;
    }

private:

    IXAudio2SourceVoice* m_SourceVoice;

private:

    TbUint32            m_BufferCount;

    TbUint32            m_StreamQueuedBufferCount;  // 積まれているバッファー数
    BlockBufferDeque    m_StreamBlockDeque;   // 利用可能なブロックバッファ
    TbUint32            m_StreamProcessedCount; // ストリーミングで再生完了したブロック数

};

}

#endif
