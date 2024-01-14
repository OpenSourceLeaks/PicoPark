/*!
 * OpenAL依存の音源
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#include "tb_fwd.h"
#include "base/io/stream/tb_stream.h"
#include "base/util/tb_bind.h"
#include "sound/tb_sound_source.h"
#include "sound/tb_sound_manager.h"
#include "sound/resource/tb_sound_resource.h"
#include "sound/depend/al/tb_sound_util_al.h"

#if TB_SOUNDAPI_IS_AL 

namespace toybox
{

namespace
{

/*!
 * ストリーム読みこみ中にSoundSourceが破棄された時用コールバック関数
 * @author Miyake_Shunsuke
 * @since 2014.06.14
 */
void onReadStreamBufferFinal( TbSoundResource* resource , TbSoundStreamBuffer* buffer )
{
    resource->UnlockStream();
    TB_SAFE_DELETE(buffer);
}

}

// 依存部分

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
TbSoundSourceDepend::TbSoundSourceDepend()
    : m_SourceId( AL_INVALID )
    , m_StreamBufferStack()
    , m_Frequency(0)
    , m_BufferCount( 0 )
    , m_Format( AL_FORMAT_MONO8 )
    , m_StreamProcessedCount(0)
{
    TbFill<ALuint>( m_BufferId ,  AL_INVALID );
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.12
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
    TB_ASSERT(m_StreamBlockDeque.IsEmpty()&&0==m_StreamProcessedCount);
    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();
    if( m_SourceId != AL_INVALID ){
        manager.FreeSource(1,&m_SourceId);
    }
    if( m_BufferCount > 0 ){
        manager.FreeBuffer(m_BufferCount,m_BufferId);
        m_BufferCount = 0;
    }
    m_Frequency = 0;
    m_Format = AL_FORMAT_MONO8;

    m_StreamBufferStack.Clear();
    m_StreamBlockDeque.Clear();
    m_StreamProcessedCount = 0;
}

/*!
 * セットアップ
 * @param bufferInfo バッファ
 * @author teco
 */
void TbSoundSourceDepend::Setup( const TbSoundResourceInfo& resource )
{
    const TbUint32 BUFFER_COUNT_TABLE[] = 
    {
        1 , // STATIC
        TB_ARRAY_COUNT_OF(m_BufferId) , // STREAM
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(BUFFER_COUNT_TABLE) == TB_SOUND_TYPE_MAX );
    TbSoundManagerDepend& manager = TbSoundManager::GetInstance().GetDepend();

    // 音源ID確保
    if( AL_INVALID == m_SourceId ){
        manager.AllocSource(1,&m_SourceId);
    }

    // バッファ
    if( m_BufferCount > 0 ){
        // 確保済みは一旦解放
        manager.FreeBuffer(m_BufferCount,m_BufferId);
        m_BufferCount = 0;
    }
    if( 0 <= resource.type && resource.type < TB_SOUND_TYPE_MAX ){
        m_BufferCount = BUFFER_COUNT_TABLE[resource.type];
        if( TB_SUCCEEDED(manager.AllocBuffer(m_BufferCount,m_BufferId)) ){
            // スタックに積む
            m_StreamBufferStack.PushBackArray(m_BufferCount,m_BufferId);
        }else{
            TB_ASSERT(!"fail to alloc al buffer");
            m_BufferCount = 0;
        }
    }

    m_Frequency = resource.resource->GetFormat().samplesPerSec;
    TbConvertTbSoundFormatToALenum(m_Format,resource.resource->GetFormat());
}

/*!
 * 再生中かどうか
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
TbBool TbSoundSourceDepend::IsPlayingAL() const
{
    ALint state;
    alGetSourcei(GetSourceId(), AL_SOURCE_STATE, &state );
    return ( state == AL_PLAYING );
}

/*!
 * 再生
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbSoundSourceDepend::playStream( const void* firstBuffer , TbUint32 size )
{
    if(firstBuffer)
    {
        pushReadyStreamBlock( firstBuffer , size );
        updatePlayingStream();
    }
    alSourcePlay( m_SourceId );
//    IsPlaying();
}

/*!
 * ストリーム再生更新
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
void TbSoundSourceDepend::updatePlayingStream()
{
    ALenum error;
    
    // 読みこみ済みバッファがあり、キューにバッファを登録可能できる状態なら追加しておく
    while( m_StreamBlockDeque.GetCount() > 0 && m_StreamBufferStack.GetCount() > 0 )
    {
        ALuint bufferId = m_StreamBufferStack.GetBack();
        BlockBufferInfo info = m_StreamBlockDeque.GetFront();
        m_StreamBufferStack.PopBack();
        m_StreamBlockDeque.PopFront();

        // バッファ設定
        alBufferData( bufferId ,
                      m_Format ,
                      info.buffer , 
                      info.size , 
                      m_Frequency );
        error = alGetError();
        TB_ASSERT_MSG_FORMAT(AL_NO_ERROR==error,"alBufferData error=%d",error);

        // キューに積む
        alSourceQueueBuffers( m_SourceId ,
                              1 , 
                              &bufferId );
        error = alGetError();
        TB_ASSERT_MSG_FORMAT(AL_NO_ERROR==error,"alSourceQueueBuffers error=%d",error);
    }
}

/*!
 * キューから終了したバッファを取り除く
 * @author Miyake_Shunsuke
 * @since 2012.04.10
 */
TbResult TbSoundSourceDepend::tryPopStreamQueue()
{
    ALint processedCount = 0;
    alGetSourcei( m_SourceId , AL_BUFFERS_PROCESSED , &processedCount );

#if 0 // for debug
    ALint val = 0;
    alGetSourcei( m_SourceId , AL_BUFFERS_QUEUED , &val );
    alGetSourcei( m_SourceId , AL_SOURCE_STATE , &val );
#endif
    
    if( processedCount > 0 ){
        m_StreamProcessedCount += processedCount;
        while( processedCount > 0 ) {
            // 終了したバッファが存在したら、バッファスタックにつみなおす
            ALuint buf;
            alSourceUnqueueBuffers( m_SourceId , 1 , &buf );
            m_StreamBufferStack.PushBack( buf );
            --processedCount;
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 利用可能なブロックバッファを積む
 * @param buffer ブロックバッファ
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
TbResult TbSoundSourceDepend::pushReadyStreamBlock( const void* buffer , TbUint32 size , TbBool isEnd )
{
    BlockBufferInfo info = { buffer , size };
    m_StreamBlockDeque.PushBack(info);
    return TB_S_OK;
}

/*!
 * ストリーム再生終了処理
 * @author Miyake_Shunsuke
 * @since 2012.04.12
 */
void TbSoundSourceDepend::stopStream()
{
    m_StreamBlockDeque.Clear();
    ALint bufferCount = 0;
    ALuint unqueuedBuffer = 0;
    alGetSourcei(m_SourceId,AL_BUFFERS_QUEUED,&bufferCount);
    // 積んでいるバッファを全て開放
    for( ALint i = 0; i < bufferCount; ++i ){
        alSourceUnqueueBuffers(m_SourceId,1,&unqueuedBuffer);
        m_StreamBufferStack.PushBack(unqueuedBuffer);
    }
    m_StreamProcessedCount = 0;
}

// 共通部分

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbSoundSource::~TbSoundSource()
{
    Reset();
}

/*!
 * リセット
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbResult TbSoundSource::Reset()
{
    Stop();
    if(m_StreamingParam.stream){
        // 完全に止まりきっていない場合はコールバック設定でコールバック先で後処理
        m_StreamingParam.stream->SetCompletedReadCallback(TbBind(onReadStreamBufferFinal,
                                                          m_ResouceInfo->resource,
                                                          m_StreamingParam.buffer));
    }else{
        TB_SAFE_DELETE(m_StreamingParam.buffer);
    }
    m_ResouceInfo = NULL;
    m_Depend.Reset();
    m_StreamingParam = StreamingParam();
    m_BitArray.ResetBit(FLAG_READY);
    return TB_S_OK;
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
void TbSoundSource::Update()
{
    if( TB_SOUND_TYPE_STREAM == m_ResouceInfo->type )
    {
        switch( m_State )
        {
        case STATE_PLAY:
            // バッファキューを見て、終わっていたら新しいバッファを積む
            if( TB_SUCCEEDED( m_Depend.tryPopStreamQueue() ) ){
                // リングバッファに空きが出来ていたら追加読みこみ
                TbSint32 processedRingCount = m_Depend.getStreamProcessedCount()-TbSoundStreamBuffer::FIRST_BLOCK_COUNT;
                TbSint32 count = processedRingCount - static_cast<TbSint32>(m_StreamingParam.readTotalBlockCount -TbSoundStreamBuffer::RING_BLOCK_COUNT);
                if( count > 0 && TB_FAILED(readStreamRingBlockBuffer(count)) ){
                    if( !m_Depend.IsPlayingAL() ){
                        if( IsEnableLoop() ){
                            Stop();
                            Play();
                        }else{
                            setState(STATE_STOP);
                        }
                        break;
                    }
                }
            }else{
                // もしもバッファが積まれているのに再生状態でなくなったら再生
                if( !m_Depend.IsPlayingAL() ){
                    ALint val = 0;
                    alGetSourcei( m_Depend.GetSourceId() , AL_BUFFERS_QUEUED , &val );
                    if( val > 0 ) {
                        alSourcePlay( m_Depend.GetSourceId() );
                    }
                }
            }
            m_Depend.updatePlayingStream();
            break;
        case STATE_PAUSE:
        case STATE_STOP:
            break;
        }
    }else{
        if( STATE_PLAY == m_State && !m_Depend.IsPlayingAL() ){
            if( IsEnableLoop() ){
                Play();
            }else{
                Stop();
            }
        }
    }
}

/*!
 * 再生
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbResult TbSoundSource::Play(TbBool isFromLoop)
{
    if( !m_ResouceInfo || !IsReady() ){
        return TB_E_FAIL;
    }
    switch( m_ResouceInfo->type )
    {
    case TB_SOUND_TYPE_STATIC:
        // 静的再生
        {
            alSourcePlay(m_Depend.GetSourceId());
            setState( STATE_PLAY );
        }
        break;
    case TB_SOUND_TYPE_STREAM:
        // ストリーミング再
        if( m_Depend.GetSourceId() > 0 && !IsPlaying() ){
            if(STATE_PAUSE == m_State)
            {
                m_Depend.playStream(nullptr,0);
            }
            else
            {
                if (!activateStream(TB_FALSE)) {
                    return TB_E_FAIL;
                }
                m_StreamingParam.stream->Seek( getFirstStreamBufferReadSize() ,
                                TB_IO_SEEK_TYPE_BEGIN);
                TbUint32 blockBufSize = m_StreamingParam.buffer->GetBlockBufferSize();
                for( TbSizeT i = 0; i < TbSoundStreamBuffer::FIRST_BLOCK_COUNT-1; ++i ) {
                    m_Depend.pushReadyStreamBlock( m_StreamingParam.buffer->GetFirstBlockBuffer(i),blockBufSize );
                }
                m_Depend.playStream(m_StreamingParam.buffer->GetFirstBlockBuffer(TbSoundStreamBuffer::FIRST_BLOCK_COUNT-1),blockBufSize);
            }
            
            // firstBlockBufferを使い切った後に読み込む用があるので-1にしておく
            setState( STATE_PLAY );
            readStreamRingBlockBuffer( TbSoundStreamBuffer::RING_BLOCK_COUNT );
        }
        break;
    default:
        return TB_E_FAIL;
        break;
    };
    TbSoundManager::GetInstance().AddSource(this,m_LayerIndex);
    return TB_S_OK;
}

/*!
 * 一時停止
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbResult TbSoundSource::Pause()
{
    if( IsReady() && m_Depend.GetSourceId() > 0 && IsPlaying() ){
        alSourcePause( m_Depend.GetSourceId() );
        setState( STATE_PAUSE );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ストップ
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbResult TbSoundSource::Stop()
{
    if( m_Depend.GetSourceId() > 0 && m_State != STATE_STOP ){
        alSourceStop( m_Depend.GetSourceId() );
#if 1
        // TbBool isPlaying =  m_Depend.IsPlayingAL();
#endif
        if( TB_SOUND_TYPE_STREAM == GetSoundType() ){
            m_Depend.stopStream();
            if( m_StreamingParam.stream && !m_StreamingParam.stream->IsReading() ){
                // 読みこみ中の場合はコールバック先で呼ぶ
                deactivateStream();
            }
            m_StreamingParam.readTotalBlockCount = 0;
        }
        setState( STATE_STOP );
        TbSoundManager::GetInstance().RemoveSource(this);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 位置設定
 * @author Miyake Shunsuke
 * @since 2011.07.25
 */
TbResult TbSoundSource::SetPos( const TbVector4& pos )
{
    m_Pos = pos;
    alSourcefv( m_Depend.GetSourceId() , AL_VELOCITY , reinterpret_cast<TbFloat32*>(&m_Pos) );
    return TB_S_OK;
}

/*!
 * 速度設定
 * @author Miyake Shunsuke
 * @since 2011.07.25
 */
TbResult TbSoundSource::SetVelo( const TbVector4& velo )
{
    m_Velo = velo;
    alSourcefv( m_Depend.GetSourceId() , AL_VELOCITY , reinterpret_cast<TbFloat32*>(&m_Velo) );
    return TB_S_OK;
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2012.04.04
 */
void TbSoundSource::setup()
{
    if( m_BitArray.Test(FLAG_READY) ){
        return;
    }
    if( !m_ResouceInfo ){
        return;
    }
    const TbSoundResource* resource = m_ResouceInfo->resource;
    if( !resource || !resource->IsReady() ){
        return;
    }

    // 依存部分セットアップ
    m_Depend.Setup( *m_ResouceInfo );

    TbSoundManager& manager = TbSoundManager::GetInstance();
    switch( m_ResouceInfo->type )
    {
    case TB_SOUND_TYPE_STATIC: // 静的再生
        {
            // バッファを設定する
            alBufferData( m_Depend.GetBufferId(0) ,
                          m_Depend.GetFormat() ,
                          resource->GetStaticBuffer() ,
                          resource->GetBufferSize() ,
                          resource->GetFormat().samplesPerSec );
            alSourcei( m_Depend.GetSourceId() , AL_BUFFER , m_Depend.GetBufferId(0) );
        }
        break;
    case TB_SOUND_TYPE_STREAM: // ストリーミング再生
        {
            // ストリームバッファを獲得
            const TbSoundFormat& format = resource->GetFormat(); 
            if( !m_StreamingParam.buffer ) {
                m_StreamingParam.buffer = TB_NEW TbSoundStreamBuffer();
                m_StreamingParam.buffer->Setup( format.samplesPerSec , format.bitsPerSample , format.channel ); 
            }
        }
        break;
    };
    m_BitArray.SetBit(FLAG_READY);
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