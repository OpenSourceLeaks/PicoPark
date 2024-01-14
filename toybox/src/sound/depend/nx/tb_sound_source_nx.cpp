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
#include "sound/depend/nx/tb_sound_util_nx.h"

#if TB_SOUNDAPI_IS_NX

namespace toybox
{

static TbUint32 CHANNEL_MAX = 2; // Managerを参照スベキ

// 依存部分

// コンストラクタ
TbSoundSourceDepend::Voice::Voice( const TbSoundFormat& format )
    : m_IsInitialized(TB_FALSE)
{
    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();

    // 音源確保
    nn::audio::SampleFormat sampleFormat;
    TbSoundFormatToNX(&sampleFormat, format);
    m_IsInitialized = nn::audio::AcquireVoiceSlot(  manager.GetConfig(),
                                                &m_Voice,
                                                format.samplesPerSec,
                                                format.channel,
                                                sampleFormat,
                                                120,
                                                nullptr,
                                                0);
    TB_ASSERT(m_IsInitialized);
    nn::audio::SetVoiceDestination(manager.GetConfig(), &m_Voice, manager.GetFinalMix());
}

// デストラクタ
TbSoundSourceDepend::Voice::~Voice()
{
    if (m_IsInitialized) {
        TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();
        nn::audio::ReleaseVoiceSlot(manager.GetConfig(),&m_Voice);
    }
}

// 破棄可能か
TbBool TbSoundSourceDepend::Voice::IsEnableDelete() const
{
    int count = nn::audio::GetWaveBufferCount(&m_Voice);
    return count == 0;
}

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundSourceDepend::TbSoundSourceDepend()
    : m_Voice(nullptr)
    , m_StreamProcessedCount(0)
    , m_WaveBufferCount( 0 )
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
 * @author teco
 */
void TbSoundSourceDepend::Reset()
{
    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();

    m_StreamBlockDeque.Clear();
    m_StreamProcessedCount = 0;

    if(m_Voice) 
    {
        if (m_Voice->IsEnableDelete())
        {
            TB_SAFE_DELETE(m_Voice);
        }
        else
        {
            TB_ASSERT(nn::audio::GetVoicePlayState(m_Voice->GetNative())==nn::audio::VoiceType::PlayState_Stop);
            // 遅延解放
            TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();
            manager.RequestDeleteVoice(m_Voice);
        }
    }
}

/*!
 * セットアップ
 * @param bufferInfo バッファ
 * @author teco
 */
void TbSoundSourceDepend::Setup( const TbSoundResourceInfo& resource )
{
    TB_ASSERT(!m_Voice);
    const TbUint32 BUFFER_COUNT_TABLE[] = 
    {
        1 , // STATIC
        nn::audio::VoiceType::WaveBufferCountMax , // STREAM
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(BUFFER_COUNT_TABLE) == TB_SOUND_TYPE_MAX );

    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();

    // 音源確保
    m_Format = resource.resource->GetFormat();
    m_Voice = TB_NEW Voice(m_Format);

    if( 0 <= resource.type && resource.type < TB_SOUND_TYPE_MAX ){
        m_WaveBufferCount = BUFFER_COUNT_TABLE[resource.type];
        for (TbUint32 i = 0; i < m_WaveBufferCount; ++i)
        {
            m_FreeWaveBufferDeque.PushBack(m_Voice->GetWaveBuffer(i));
        }
    }
    m_StreamProcessedCount = 0;
}

/*!
 * WaveBuffer取得
 * @author teco
 */
nn::audio::WaveBuffer* TbSoundSourceDepend::createWaveBuffer(const void* buffer, TbUint32 size, TbBool isEnd , const TbSoundFormat& format)
{
    if (m_FreeWaveBufferDeque.IsEmpty()) {
        return nullptr;
    }
    nn::audio::WaveBuffer* waveBuffer = m_FreeWaveBufferDeque.GetFront();
    m_FreeWaveBufferDeque.PopFront();

    int32_t bytePerSample = format.bitsPerSample / 8;
    waveBuffer->buffer = buffer;
    waveBuffer->size = size;
    waveBuffer->startSampleOffset = 0;
    waveBuffer->endSampleOffset = static_cast<int32_t>(waveBuffer->size / bytePerSample) / format.channel;
    waveBuffer->loop = false;
    waveBuffer->isEndOfStream = isEnd;
    waveBuffer->pContext = nullptr;
    waveBuffer->contextSize = 0;
    return waveBuffer;
}

/*!
 * ストリーム再生更新
 * @author teco
 */
void TbSoundSourceDepend::updatePlayingStream(const TbSoundFormat& format)
{
    // 読みこみ済みバッファがあり、キューにバッファを登録可能できる状態なら追加しておく
    while( m_StreamBlockDeque.GetCount() > 0 && !m_FreeWaveBufferDeque.IsEmpty() )
    {
        BlockBufferInfo info = m_StreamBlockDeque.GetFront();
        nn::audio::WaveBuffer* waveBuffer = createWaveBuffer(info.buffer,info.size,info.isEnd, format);
        if(nn::audio::AppendWaveBuffer(m_Voice->GetNative(),waveBuffer) ) {
            m_StreamBlockDeque.PopFront();
        } else {
            m_FreeWaveBufferDeque.PushBack(waveBuffer);
            break;
        }
    }
}

/*!
 * キューから終了したバッファを取り除く
 * @author teco
 */
TbResult TbSoundSourceDepend::tryPopStreamQueue()
{
    TbResult result = TB_E_FAIL;
    while (const nn::audio::WaveBuffer* waveBuffer = nn::audio::GetReleasedWaveBuffer(m_Voice->GetNative()))
    {
        ++m_StreamProcessedCount;
        m_FreeWaveBufferDeque.PushBack(const_cast<nn::audio::WaveBuffer*>(waveBuffer));
        result = TB_S_OK;
    }
//    TB_ASSERT(!m_FreeWaveBufferDeque.IsFull());
    return result;
}

/*!
 * 利用可能なブロックバッファを積む
 * @param buffer ブロックバッファ
 * @author teco
 */
TbResult TbSoundSourceDepend::pushReadyStreamBlock( const void* buffer , TbUint32 size , TbBool isEnd )
{
    BlockBufferInfo info = { buffer , size , isEnd };
    TbBool result = m_StreamBlockDeque.PushBack(info);
    TB_ASSERT(result);
    return TB_S_OK;
}

/*!
 * ストリーム再生終了処理
 * @author teco
 */
void TbSoundSourceDepend::stopStream()
{
    m_StreamBlockDeque.Clear();
    m_StreamProcessedCount = 0;
}

// 共通部分

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
        nn::audio::VoiceType::PlayState state = nn::audio::GetVoicePlayState(m_Depend.GetVoice());
        switch( m_ResouceInfo->type )
        {
        case TB_SOUND_TYPE_STATIC:
            // 静的再生
            {
                const nn::audio::WaveBuffer* waveBuffer = nn::audio::GetReleasedWaveBuffer(m_Depend.GetVoice());
                if(waveBuffer)
                {
                    m_Depend.m_FreeWaveBufferDeque.PushBack(const_cast<nn::audio::WaveBuffer*>(waveBuffer));
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
                    TbSint32 processedCount = m_Depend.m_StreamProcessedCount - TbSoundStreamBuffer::FIRST_BLOCK_COUNT;
                    TbSint32 count = processedCount - static_cast<TbSint32>(m_StreamingParam.readTotalBlockCount - TbSoundStreamBuffer::RING_BLOCK_COUNT);
                    if( count > 0 && TB_FAILED(readStreamRingBlockBuffer(count)) ){
                        // if (m_Depend.m_StreamBlockDeque.IsEmpty() )
                        if (m_Depend.m_FreeWaveBufferDeque.IsFull())
                        {
                            if (IsEnableLoop()) {
                                stop();
                                play(TB_TRUE);
                            }
                            else {
                                stop();
                            }
                            break;
                        }
                    }
                }
                m_Depend.updatePlayingStream(m_ResouceInfo->resource->GetFormat());
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
            nn::audio::WaveBuffer* waveBuffer = m_Depend.createWaveBuffer(  m_ResouceInfo->resource->GetStaticBuffer(),
                                                                            m_ResouceInfo->resource->GetBufferSize(),
                                                                            TB_TRUE,
                                                                            m_ResouceInfo->resource->GetFormat());
            nn::audio::AppendWaveBuffer(m_Depend.GetVoice(),waveBuffer);
            nn::audio::SetVoicePlayState(m_Depend.GetVoice(),nn::audio::VoiceType::PlayState_Play);

            // 静的なものを積んで再生
            setState( STATE_PLAY );
            if (m_Volume != 1.0f)
            {
                SetVolume(m_Volume);
            }
        }
        break;
    case TB_SOUND_TYPE_STREAM:
        // ストリーミング再
        {
            if( !activateStream(isFromLoop) ){
                return TB_E_FAIL;
            }

            if( m_State.Load() == STATE_STOP )
            {
                TbUint32 seekPos = getFirstStreamBufferReadSize();
                if (isFromLoop) {
                    seekPos += getLoopFirstBufferOffset();
                }
                m_StreamingParam.stream->Seek(seekPos, TB_IO_SEEK_TYPE_BEGIN);
                TbUint32 blockBufSize = m_StreamingParam.buffer->GetBlockBufferSize();
                for(TbUint32 i = 0; i < TbSoundStreamBuffer::FIRST_BLOCK_COUNT; ++i ) {
                    m_Depend.pushReadyStreamBlock( m_StreamingParam.buffer->GetFirstBlockBuffer(i),blockBufSize , TB_FALSE );
                }
                m_Depend.updatePlayingStream(m_ResouceInfo->resource->GetFormat());
            }

            nn::audio::SetVoicePlayState(m_Depend.GetVoice(), nn::audio::VoiceType::PlayState_Play);

            if( m_State.Load() == STATE_STOP )
            {
                setState( STATE_PLAY );
                TbSint32 count = m_Depend.m_StreamBlockDeque.GetCapacity() - m_Depend.m_StreamBlockDeque.GetCount();
                readStreamRingBlockBuffer(count);
            }
            setState( STATE_PLAY );
            if (m_Volume != 1.0f) {
                SetVolume(m_Volume);
            }
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
    if( IsReady() && IsPlaying() ){
        nn::audio::SetVoicePlayState(m_Depend.GetVoice(), nn::audio::VoiceType::PlayState_Pause);
        setState( STATE_PAUSE );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ストップ
 * @author teco
 */
TbResult TbSoundSource::stop()
{
    if( m_State.Load() != STATE_STOP ){
        nn::audio::SetVoicePlayState(m_Depend.GetVoice(),nn::audio::VoiceType::PlayState_Stop);
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
    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();
    float resultVolume = 1.0f / static_cast<TbFloat32>(m_Depend.m_Format.channel);
    resultVolume *= volume;
    if ( 1 == m_Depend.m_Format.channel)
    {
        for (TbUint32 i = 0; i < CHANNEL_MAX; ++i)
        {
            nn::audio::SetVoiceMixVolume(m_Depend.GetVoice(), manager.GetFinalMix(), resultVolume, 0, i);
        }
    }
    else
    {
        for (TbUint32 i = 0; i < m_Depend.m_Format.channel; ++i)
        {
            nn::audio::SetVoiceMixVolume(m_Depend.GetVoice(), manager.GetFinalMix(), resultVolume , i, i);
        }
    }
    m_Volume = volume;
}

/*!
 * 再生済みサンプル数取得
 * @author teco
 */
TbUint32 TbSoundSource::getPlayedSampleCount() const
{
    if (m_Depend.m_Voice) {
        TbUint32 sample = nn::audio::GetVoicePlayedSampleCount(m_Depend.GetVoice());
        return sample;
    }
    return 0;
}

}

#endif