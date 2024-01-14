/*!
 * XAudio依存の音源
 * @author teco
 */

#include "tb_fwd.h"
#include "base/io/stream/tb_stream.h"
#include "base/util/tb_bind.h"
#include "sound/tb_sound_source.h"
#include "sound/tb_sound_manager.h"
#include "sound/resource/tb_sound_resource.h"
#include "sound/depend/xaudio/tb_sound_util_xaudio.h"

#if TB_SOUNDAPI_IS_XAUDIO

namespace toybox
{

// 依存部分

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundSourceDepend::TbSoundSourceDepend()
    : m_SourceVoice(nullptr)
    , m_BufferCount( 0 )
    , m_StreamQueuedBufferCount(0)
    , m_StreamProcessedCount(0)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbSoundSourceDepend::~TbSoundSourceDepend()
{
}

/*!
 * リセット
 * 破棄する場合は再利利用する場合に呼ばれます
 * @author teco
 */
void TbSoundSourceDepend::Reset()
{
    TB_ASSERT(m_StreamBlockDeque.IsEmpty()&&0==m_StreamQueuedBufferCount);
    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();

    m_StreamBlockDeque.Clear();
    m_StreamQueuedBufferCount = 0;
    m_StreamProcessedCount = 0;

    if( m_SourceVoice )  {
        m_SourceVoice->DestroyVoice();
        m_SourceVoice = nullptr;
    }
}

/*!
 * セットアップ
 * @param bufferInfo バッファ
 * @author teco
 */
void TbSoundSourceDepend::Setup( const TbSoundResourceInfo& resource )
{
    TB_ASSERT(!m_SourceVoice);
    const TbUint32 BUFFER_COUNT_TABLE[] = 
    {
        1 , // STATIC
        TbSoundStreamBuffer::RING_BLOCK_COUNT , // STREAM
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(BUFFER_COUNT_TABLE) == TB_SOUND_TYPE_MAX );

    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();

    // 音源確保
    WAVEFORMATEX format;
    TbConvertTbSoundFormatTo(format,resource.resource->GetFormat());
    manager.GetXAudio()->CreateSourceVoice(&m_SourceVoice,&format);

    // バッファ
    if( m_BufferCount > 0 ){
        // 確保済みは一旦解放
        m_BufferCount = 0;
    }

    if( 0 <= resource.type && resource.type < TB_SOUND_TYPE_MAX ){
        m_BufferCount = BUFFER_COUNT_TABLE[resource.type];
        m_StreamQueuedBufferCount = 0;
    }

    m_StreamProcessedCount = 0;
}

/*!
 * ストリーム再生更新
 * @author teco
 */
void TbSoundSourceDepend::updatePlayingStream()
{
    // 読みこみ済みバッファがあり、キューにバッファを登録可能できる状態なら追加しておく
    while( m_StreamBlockDeque.GetCount() > 0 && m_StreamQueuedBufferCount < m_BufferCount )
    {
        BlockBufferInfo info = m_StreamBlockDeque.GetFront();
        m_StreamBlockDeque.PopFront();

        XAUDIO2_BUFFER buffer = {0};
        buffer.pAudioData = reinterpret_cast<const BYTE*>(info.buffer);
        buffer.AudioBytes = info.size;
        buffer.Flags = 0;
//        buffer.Flags = XAUDIO2_END_OF_STREAM;

        HRESULT hr = m_SourceVoice->SubmitSourceBuffer(&buffer);
        if( TB_VERIFY(SUCCEEDED(hr)) ) {
            ++m_StreamQueuedBufferCount;
        }
    }
}

/*!
 * キューから終了したバッファを取り除く
 * @author teco
 */
TbResult TbSoundSourceDepend::tryPopStreamQueue()
{
    XAUDIO2_VOICE_STATE state;
    m_SourceVoice->GetState(&state);
    if( m_StreamQueuedBufferCount != state.BuffersQueued ){
        m_StreamProcessedCount += (m_StreamQueuedBufferCount - state.BuffersQueued);
        m_StreamQueuedBufferCount = state.BuffersQueued;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 利用可能なブロックバッファを積む
 * @param buffer ブロックバッファ
 * @author teco
 */
TbResult TbSoundSourceDepend::pushReadyStreamBlock( const void* buffer , TbUint32 size , TbBool isEnd )
{
    BlockBufferInfo info = { buffer , size };
    m_StreamBlockDeque.PushBack(info);
    return TB_S_OK;
}

/*!
 * ストリーム再生終了処理
 * @author teco
 */
void TbSoundSourceDepend::stopStream()
{
    m_StreamBlockDeque.Clear();
    m_StreamQueuedBufferCount = 0;
    m_StreamProcessedCount = 0;
}

////////////////////////////
// 共通部分
////////////////////////////

/*!
 * デストラクタ
 * @author teco
 */
TbSoundSource::~TbSoundSource()
{
    stop();
    reset();
}

/*!
 * 更新
 * @author teco
 */
TbBool TbSoundSource::Update()
{
    State reqState = static_cast<State>(m_ReqState.Load());
    if (reqState != STATE_INVALID)
    {
        m_ReqState = STATE_INVALID;
        switch (reqState)
        {
        case STATE_PLAY:
            if (TB_E_FAIL == play(TB_FALSE)) {
                // リトライ
                m_ReqState = STATE_PLAY;
            }
            break;
        case STATE_STOP:
            stop();
            break;
        case STATE_PAUSE:
            pause();
            break;
        }
    }
    if( STATE_STOP != m_State.Load() )
    {
        XAUDIO2_VOICE_STATE state;
        m_Depend.GetVoice()->GetState(&state);
        switch( m_ResouceInfo->type )
        {
        case TB_SOUND_TYPE_STATIC:
            // 静的再生
            {
                if( 0 == state.BuffersQueued )
                {
                    if( IsEnableLoop() ){
                        stop();
                        play(TB_FALSE);
                    }else{
                        stop();
                    }
                }
            }
            break;
        case TB_SOUND_TYPE_STREAM:
            {
                // バッファキューを見て、終わっていたら新しいバッファを積む
                if( TB_SUCCEEDED( m_Depend.tryPopStreamQueue() ) ){
                    // リングバッファに空きが出来ていたら追加読みこみ
                    TbSint32 processedRingCount = m_Depend.getStreamProcessedCount()-TbSoundStreamBuffer::FIRST_BLOCK_COUNT;
                    TbSint32 count = processedRingCount - static_cast<TbSint32>(m_StreamingParam.readTotalBlockCount -TbSoundStreamBuffer::RING_BLOCK_COUNT);
                    if( count > 0 && TB_FAILED(readStreamRingBlockBuffer(count)) ){
                        if( 0 == state.BuffersQueued ){
                            if( IsEnableLoop() ){
                                stop();
                                play(TB_TRUE);
                            }else{
                                stop();
                            }
                            break;
                        }
                    }
                }
                m_Depend.updatePlayingStream();
            }
            break;
        }
    }
    return STATE_STOP != m_State.Load();
}

/*!
 * 再生
 * @author teco
 */
TbResult TbSoundSource::play(TbBool isFromLoop)
{
    if( !m_ResouceInfo || !IsReady() ){
        return TB_E_FAIL;
    }
    if (IsPlaying()) {
        return TB_S_OK;
    }
    switch( m_ResouceInfo->type )
    {
    case TB_SOUND_TYPE_STATIC:
        // 静的再生
        {
            XAUDIO2_BUFFER buffer = {0};
            buffer.pAudioData = reinterpret_cast<const BYTE*>(m_ResouceInfo->resource->GetStaticBuffer());
            buffer.AudioBytes = m_ResouceInfo->resource->GetBufferSize();
            buffer.Flags = XAUDIO2_END_OF_STREAM;
            m_Depend.GetVoice()->SubmitSourceBuffer(&buffer);
            m_Depend.GetVoice()->Start(0);

            // 静的なものを積んで再生
            setState( STATE_PLAY );
            SetVolume(m_Volume);
        }
        break;
    case TB_SOUND_TYPE_STREAM:
        // ストリーミング再
        {
            // 
            if( m_State.Load() == STATE_STOP )
            {
                if (!activateStream(isFromLoop)) {
                    return TB_E_FAIL;
                }
                TbUint32 seekPos = getFirstStreamBufferReadSize();
                if(isFromLoop){
                    seekPos += getLoopFirstBufferOffset();
                }
                m_StreamingParam.stream->Seek(seekPos,TB_IO_SEEK_TYPE_BEGIN);
                TbUint32 blockBufSize = m_StreamingParam.buffer->GetBlockBufferSize();
                for( TbSizeT i = 0; i < TbSoundStreamBuffer::FIRST_BLOCK_COUNT; ++i ) {
                    m_Depend.pushReadyStreamBlock( m_StreamingParam.buffer->GetFirstBlockBuffer(i),blockBufSize );
                }
                m_Depend.updatePlayingStream();

                m_Depend.GetVoice()->Start(0);
                setState(STATE_PLAY);
                readStreamRingBlockBuffer(TbSoundStreamBuffer::RING_BLOCK_COUNT);
            }
            else
            {
                m_Depend.GetVoice()->Start(0);
            }

            setState( STATE_PLAY ); 
            SetVolume(m_Volume);
        }
        break;
    default:
        return TB_E_FAIL;
        break;
    };
    return TB_S_OK;
}

/*!
 * 一時停止
 * @author teco
 */
TbResult TbSoundSource::pause()
{
    if (IsPlaying())
    {
        m_Depend.GetVoice()->Stop();
        setState(STATE_PAUSE);
    }
    return TB_S_OK;
}

/*!
 * ストップ
 * @author teco
 */
TbResult TbSoundSource::stop()
{
    if( m_State.Load() != STATE_STOP )
    {
        m_Depend.GetVoice()->Stop();
        m_Depend.GetVoice()->FlushSourceBuffers();

        if( TB_SOUND_TYPE_STREAM == GetSoundType() ){
            m_Depend.stopStream();
            if( m_StreamingParam.stream && !m_StreamingParam.stream->IsReading() ){
                // 読みこみ中の場合はコールバック先で呼ぶ
                deactivateStream();
            }
            m_StreamingParam.readTotalBlockCount = 0;
        }
        setState( STATE_STOP );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 位置設定
 * @author teco
 */
TbResult TbSoundSource::SetPos( const TbVector4& pos )
{
    m_Pos = pos;
    TB_ASSERT(!"Unsupported");
    return TB_S_OK;
}

/*!
 * 速度設定
 * @author teco
 */
TbResult TbSoundSource::SetVelo( const TbVector4& velo )
{
    m_Velo = velo;
    TB_ASSERT(!"Unsupported");
    return TB_S_OK;
}

/*!
 * 音量設定
 * @author teco
 */
void TbSoundSource::SetVolume( TbFloat32 volume )
{
    if( m_Depend.GetVoice() ){
        m_Depend.GetVoice()->SetVolume( volume );
    }
    m_Volume = volume;
}

/*!
* 再生済みサンプル数取得
* @author teco
*/
TbUint32 TbSoundSource::getPlayedSampleCount() const
{
    if( m_ResouceInfo->type != TB_SOUND_TYPE_STREAM ){
        // ストリーミング再生以外は無理
        return 0;
    }
    const TbSoundFormat& format = m_ResouceInfo->resource->GetFormat();
    TbUint32 count = m_Depend.getStreamProcessedCount();
    TbUint32 sample = m_StreamingParam.buffer->GetBlockBufferSize() * count; // 再生されたバッファ
    sample = (sample * 8) / (format.channel * format.bitsPerSample);
    return sample;
}

}

#endif