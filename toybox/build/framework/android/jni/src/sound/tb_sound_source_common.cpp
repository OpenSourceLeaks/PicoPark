/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.17
 */

#include "tb_fwd.h"
#include "base/io/stream/tb_stream.h"
#include "base/math/tb_math_util.h"
#include "sound/tb_sound_source.h"
#include "sound/tb_sound_stream_buffer.h"
#include "sound/tb_sound_manager.h"
#include "sound/resource/tb_sound_resource.h"
#include "sound/tb_sound_source_fade.h"

namespace toybox
{

namespace
{

    /*!
    * ストリーム読みこみ中にSoundSourceが破棄された時用コールバック関数
    * @author teco
    */
    void onReadStreamBufferFinal(TbSoundResource* resource, TbSoundStreamBuffer* buffer)
    {
        resource->UnlockStream();
        TB_SAFE_DELETE(buffer);
    }

}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.06.14
 */
TbSoundSource::TbSoundSource()
    : m_Pos()
    , m_Velo()
    , m_State( STATE_STOP )
    , m_ReqState(STATE_INVALID)
    , m_Volume(1.0f)
    , m_ResouceInfo( NULL )
    , m_Shape( NULL )
    , m_StreamingParam()
    , m_IsBusy(TB_FALSE)
    , m_Next( NULL )
    , m_Prev( NULL )
    , m_LayerIndex( 0 )
    , m_Depend()
{
}


/*!
 * コンストラクタ
 * @author teco
 */
TbSoundSource::TbSoundSource( TbSoundId soundId )
    : m_Pos()
    , m_Velo()
    , m_State( STATE_STOP )
    , m_ReqState(STATE_INVALID)
    , m_ResouceInfo( NULL )
    , m_Shape( NULL )
    , m_StreamingParam()
    , m_IsBusy(TB_FALSE)
    , m_Next( NULL )
    , m_Prev( NULL )
    , m_LayerIndex( 0 )
    , m_Depend()
{
    TbSoundManager& soundManager = TbSoundManager::GetInstance();
    if( soundManager.IsInitialized() ){
        m_ResouceInfo = soundManager.GetResource(soundId);
        TB_ASSERT(m_ResouceInfo && m_ResouceInfo->resource->IsReady() );
        setup();
    }else{
        m_BitArray.SetBit(FLAG_ERROR);
    }
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbSoundSource::TbSoundSource( const char* name )
    : m_Pos()
    , m_Velo()
    , m_State( STATE_STOP )
    , m_ReqState( STATE_INVALID )
    , m_Volume(1.0f)
    , m_ResouceInfo( NULL )
    , m_Shape( NULL )
    , m_StreamingParam()
    , m_IsBusy(TB_FALSE)
    , m_Next( NULL )
    , m_Prev( NULL )
    , m_LayerIndex( 0 )
    , m_Depend()
{
    TbSoundManager& soundManager = TbSoundManager::GetInstance();
    if( soundManager.IsInitialized() ){
        m_ResouceInfo = soundManager.GetResource(name);
        TB_ASSERT(m_ResouceInfo && m_ResouceInfo->resource->IsReady() );
        setup();
    }else{
        m_BitArray.SetBit(FLAG_ERROR);
    }
}

/*!
 * リセット
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbResult TbSoundSource::Reset( TbSoundId soundId )
{
    TbSoundManager& soundManager = TbSoundManager::GetInstance();
    TbSoundResourceInfo* info = soundManager.GetResource(soundId);
    if( !info ) {
        return TB_E_FAIL;
    }
    return reset(info);
}

/*!
 * リセット
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbResult TbSoundSource::Reset( const char* name )
{
    TbSoundManager& soundManager = TbSoundManager::GetInstance();
    TbSoundResourceInfo* info = soundManager.GetResource(name);
    if( !info ) {
        return TB_E_FAIL;
    }
    return reset(info);
}

/*!
* リセット
* @author teco
*/
TbResult TbSoundSource::reset()
{
    if (m_StreamingParam.stream) {
        // 完全に止まりきっていない場合はコールバック設定でコールバック先で後処理
        m_StreamingParam.stream->SetCompletedReadCallback(TbBind(onReadStreamBufferFinal,
            m_ResouceInfo->resource,
            m_StreamingParam.buffer));
    }
    else {
        TB_SAFE_DELETE(m_StreamingParam.buffer);
    }
    m_ResouceInfo = NULL;
    m_Depend.Reset();
    m_StreamingParam = StreamingParam();
    m_BitArray.ResetBit(FLAG_READY);
    return TB_S_OK;
}

/*!
* リセット
* @author Miyake_Shunsuke
* @since 2014.06.15
*/
TbResult TbSoundSource::reset(TbSoundResourceInfo* resourceInfo)
{
    TB_RESQUE_RET(!IsBusy(),TB_E_FAIL);
    if (!resourceInfo) {
        return TB_E_FAIL;
    }
    if (m_ResouceInfo == resourceInfo) {
        return TB_S_OK;
    }
    reset();
    m_ResouceInfo = resourceInfo;
    setup();
    return TB_S_OK;
}

/*!
 * 再生
 * @author teco
 */
void TbSoundSource::Play()
{
    m_ReqState = STATE_PLAY;
    TbSoundManager::GetInstance().AddSource(this);
}

/*!
* 再生時間取得
* @author teco
*/
TbFloat64 TbSoundSource::GetPlayTimeSec() const
{
    if (!IsReady()) {
        return 0.0f;
    }
    const TbSoundFormat& format = m_ResouceInfo->resource->GetFormat();
    TbUint32 sample = getPlayedSampleCount();
    TbFloat64 time = sample;
    time = sample / static_cast<TbFloat64>(format.samplesPerSec);
    time += m_ResouceInfo->resource->GetLoadSetting().skipSec;
    return time;
}

/*!
* タイプ取得
* @author teco
*/
TbSoundType TbSoundSource::GetSoundType() const
{
    if (m_ResouceInfo) {
        return m_ResouceInfo->type;
    }
    return TB_SOUND_TYPE_INVALID;
}

/*!
 * 再生の準備はできている
 * @author Miyake_Shunsuke
 * @since 2012.04.22
 */
TbBool TbSoundSource::IsReady() const
{
    return m_BitArray.Test(FLAG_READY) && m_ResouceInfo;
}

/*!
 * 再生中かどうか
 * @author Miyake_Shunsuke
 * @since 2012.04.11
 */
TbBool TbSoundSource::IsPlaying() const
{
    return m_State.Load() == STATE_PLAY;
}

/*!
 * 再生停止中
 * @author Miyake_Shunsuke
 * @since 2014.06.14
 */
TbBool TbSoundSource::IsStopping() const
{
    // 状態は停止だが、再生中またはストリーム読み込み中の時
    return (m_State.Load() == STATE_STOP && (m_StreamingParam.stream));
}

/*!
* セットアップ
* @author teco
*/
void TbSoundSource::setup()
{
    if (m_BitArray.Test(FLAG_READY)) {
        return;
    }
    if (!m_ResouceInfo) {
        return;
    }

    const TbSoundResource* resource = m_ResouceInfo->resource;
    if (!resource || !resource->IsReady()) {
        return;
    }

    // 依存部分セットアップ
    m_Depend.Setup(*m_ResouceInfo);

    TbSoundManager& manager = TbSoundManager::GetInstance();
    switch (m_ResouceInfo->type)
    {
    case TB_SOUND_TYPE_STATIC: // 静的再生
        break;
    case TB_SOUND_TYPE_STREAM: // ストリーミング再生
        {
            // ストリームバッファを獲得
            const TbSoundFormat& format = resource->GetFormat();
            if (!m_StreamingParam.buffer) {
                m_StreamingParam.buffer = TB_NEW TbSoundStreamBuffer();
                m_StreamingParam.buffer->Setup(format.samplesPerSec, format.bitsPerSample, format.channel);
            }
        }
        break;
    };
    SetVolume(m_Volume);
    m_BitArray.SetBit(FLAG_READY);
}


/*!
 * 前設定
 * @author Miyake Shunsuke
 * @since 2011.07.27
 */
TbResult TbSoundSource::setLayer( TbUint32 layerIndex , TbSoundSource* next )
{
    if( next ){
        m_Prev = next->m_Prev;
        if( next->m_Prev ){
            next->m_Prev->m_Next = this;
        }
        next->m_Prev = this;
        m_Next = next;
    }else{
        m_Next = this;
        m_Prev = this;
    }
    m_LayerIndex = layerIndex;
    return TB_S_OK;
}

/*!
 * チェーンから除去
 * @author Miyake Shunsuke
 * @since 2011.07.27
 */
TbResult TbSoundSource::removeLayer()
{
    if( m_Prev ){
        m_Prev->m_Next = m_Next;
    }
    if( m_Next ){
        m_Next->m_Prev = m_Prev;
    }
    m_LayerIndex = 0;
    m_Next = NULL;
    m_Prev = NULL;
    return TB_S_OK;
}

/*!
 * 最初にストリーミングキューに乗せるバッファの読み込みサイズ
 * @author Miyake_Shunsuke
 * @since 2014.06.14
 */
TbUint32 TbSoundSource::getFirstStreamBufferReadSize() const 
{
    if( m_ResouceInfo && m_ResouceInfo->resource ){
        return m_ResouceInfo->resource->GetStreamFirstBufferSize();
    }
    return 0U;
}

/*!
* ループ時の先頭バッファオフセット
* @author teco
*/
TbUint32 TbSoundSource::getLoopFirstBufferOffset() const
{
    if (m_ResouceInfo && m_ResouceInfo->resource) {
        return m_ResouceInfo->resource->GetLoopFirstBufferOffset();
    }
    return 0U;
}


/*!
 * ストリームセットアップ
 * 再生開始時に呼ばれる
 * @author teco
 */
TbBool TbSoundSource::activateStream(TbBool isFromLoop)
{
    if( !m_StreamingParam.stream && m_ResouceInfo && m_ResouceInfo->resource ){
        m_StreamingParam.stream = m_ResouceInfo->resource->LockStream();
    }
    if( !m_StreamingParam.buffer || !m_StreamingParam.stream ){
//        TB_ASSERT(!"not enable stream!\n");
        // バッファあるいはストリームがない
        return TB_FALSE;
    }
    // バッファを確保しておく
    m_StreamingParam.buffer->ResetRingBlockIndex();
    m_StreamingParam.readTotalBlockCount = 0;
    m_StreamingParam.stream->Seek(0,TB_IO_SEEK_TYPE_BEGIN);
    m_StreamingParam.stream->SetCompletedReadCallback(TbCreateMemFunc(this,&Self::onReadStreamBuffer));

    TbUint8* buffer = isFromLoop ? m_ResouceInfo->resource->GetStreamLoopFirstBuffer() :
                                   m_ResouceInfo->resource->GetStreamFirstBuffer();
    TbUint32 blockBufferSize = m_StreamingParam.buffer->GetBlockBufferSize();
    for (TbUint32 i = 0; i < TbSoundStreamBuffer::FIRST_BLOCK_COUNT; ++i) {
        TbMemCopy(m_StreamingParam.buffer->GetFirstBlockBuffer(i),
                  &buffer[i*blockBufferSize],
                  blockBufferSize);
    }
    return TB_TRUE;
}


/*!
 * 再生終了時に呼ばれる
 * @author Miyake_Shunsuke
 */
void TbSoundSource::deactivateStream()
{
    if( m_ResouceInfo && m_ResouceInfo->resource ){
        if( m_StreamingParam.stream ){
            m_ResouceInfo->resource->UnlockStream();
            m_StreamingParam.stream = NULL;
        }
    }else{
        TB_ASSERT(!m_StreamingParam.stream);
    }
    m_StreamingParam.readSize = 0;
}

/*!
 * ストリーミング用ブロックバッファ読み込み
 * @author Miyake Shunsuke
 */
TbResult TbSoundSource::readStreamRingBlockBuffer(TbUint32 blockCount)
{
    if( !m_StreamingParam.buffer || !m_StreamingParam.stream ){
        // バッファあるいはストリームがない
        return TB_E_FAIL;
    }

    if (m_StreamingParam.stream->IsReading()) {
        // 読み込み中
        return TB_S_OK;
    }
    
    if( TbSoundStreamBuffer::RING_BLOCK_COUNT < m_StreamingParam.buffer->GetCurrentRingBlockIndex()+blockCount ){
        blockCount = TbSoundStreamBuffer::RING_BLOCK_COUNT - m_StreamingParam.buffer->GetCurrentRingBlockIndex();
        if( 0 == blockCount ){
            // 読み込めない
            TB_ASSERT(!"Invalid blockCount");
            return TB_E_INVALIDARG;
        }
    }

    // 全体での残り読み込みサイズ
    TbUint32 leftSize = m_StreamingParam.stream->GetSize() - m_StreamingParam.stream->GetSeek();
    // 次に読み込むサイズ
    TbUint32 readSize = TbMin( m_StreamingParam.buffer->GetBlockBufferSize()*blockCount , leftSize );
    if( readSize > 0 ){
        m_StreamingParam.readSize = readSize;
        m_StreamingParam.readBlockCount = m_StreamingParam.readSize / m_StreamingParam.buffer->GetBlockBufferSize();
        m_StreamingParam.readTotalBlockCount += blockCount; // 読み込んだ累積
        m_StreamingParam.isEnd = (readSize == leftSize);
        m_StreamingParam.stream->Read( m_StreamingParam.buffer->GetRingBuffer(),
                                       0,
                                       m_StreamingParam.readSize);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
* ストリームバッファ読み込み完了
* ファイルスレッドから呼ばれるので注意
* @author teco
*/
void TbSoundSource::onReadStreamBuffer()
{
    if (STATE_STOP != m_State.Load() ) 
    {
        // 再生中に、読みこみ完了したら読みこみ済みバッファに追加
        TbUint32           readSize = m_StreamingParam.readSize;
        TbSoundStreamBuffer* buffer = m_StreamingParam.buffer;
        TbUint8*         ringBuffer = buffer->GetRingBuffer();
        for (TbUint32 i = 0; i < m_StreamingParam.readBlockCount && readSize > 0; ++i) {
            TbUint32 size = TbMin(readSize, buffer->GetBlockBufferSize());
            TbUint8* dstBuffer = m_StreamingParam.buffer->GetCurrentRingBlockBuffer();
            TbMemCopy(dstBuffer,ringBuffer,size);
            m_Depend.pushReadyStreamBlock(dstBuffer, size , m_StreamingParam.isEnd);
            m_StreamingParam.buffer->IncreaseRingBlockIndex();
            ringBuffer += size;
            readSize -= size;
        }
    }
    else 
    {
        // もう再生していなかったらストリーム無効化
        deactivateStream();
    }
}


}
