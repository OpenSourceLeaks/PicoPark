/*!
 * OpenAL依存のサウンド音源クラス
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_AL_H_
#define _INCLUDED_TB_SOUND_SOURCE_AL_H_

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

    // 再生中(AL的な意味で)
    TbBool IsPlayingAL() const;

    // OpenALのソース取得
    ALuint GetSourceId() const { return m_SourceId; }

    // 利用可能なバッファ数取得
    TbUint32 GetReadyBufferCount() const {
        return m_StreamBufferStack.GetCount();
    }

    // OpenALのバッファ数取得
    TbUint32 GetBufferCount() const {
        return m_BufferCount;
    }

    // OpenALのバッファ取得
    ALuint GetBufferId( TbUint32 index) const {
        if( index < m_BufferCount ){
            return m_BufferId[index];
        }
        return 0xFFFFFFFF;
    }

    // フォーマット取得
    ALenum GetFormat() const {
        return m_Format;
    }

    // 周波数取得
    TbUint32 GetFrequency() const {
        return m_Frequency;
    }

private:

    struct BlockBufferInfo {
        const void* buffer;
        TbUint32 size;
    };

    typedef TbStaticArray<ALuint,TbSoundStreamBuffer::RING_BLOCK_COUNT> BufferStack;
    typedef TbStaticDeque<BlockBufferInfo,TbSoundStreamBuffer::RING_BLOCK_COUNT> BlockBufferDeque;

private: // ストリーミング用

    // 再生
    void playStream( const void* firstBuffer , TbUint32 size );

    // ストリーム再生終了処理
    void stopStream();

    // ストリーム再生更新
    void updatePlayingStream();

    // 利用可能なブロックバッファを積む
    TbResult pushReadyStreamBlock( const void* buffer , TbUint32 size, TbBool isEnd = TB_FALSE);

    // キューから終了したバッファを取り除く
    TbResult tryPopStreamQueue();

    // ストリームの処理完了数
    TbUint32 getStreamProcessedCount() const {
        return m_StreamProcessedCount;
    }

private:

    ALuint              m_SourceId;
    ALuint              m_BufferId[ TbSoundStreamBuffer::RING_BLOCK_COUNT ];
    TbUint32            m_BufferCount;
    ALsizei             m_Frequency;          // 周波数
    ALenum              m_Format;

    BufferStack         m_StreamBufferStack;        // 利用可能スタック
    BlockBufferDeque    m_StreamBlockDeque;   // 利用可能なブロックバッファ
    TbUint32            m_StreamProcessedCount; // ストリーミングで再生完了したブロック数

};

}

#endif
