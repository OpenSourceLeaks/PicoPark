/*!
 * XAudio
 * @author teco
 */

#include "tb_fwd.h"
#include "sound/tb_sound_manager.h"
#include "sound/tb_sound_source.h"
#include "sound/resource/tb_sound_resource_wav.h"
#include "sound/depend/nx/tb_sound_util_nx.h"
#include "base/string/tb_string_util.h"

#if TB_SOUNDAPI_IS_NX

namespace toybox
{

static const int8_t MAIN_BUS[] =
{
    0 ,1 
};

static const TbUint32 MEMORY_POOL_BUUFER_SIZE = 1024*1024*128;

// 依存クラス

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundManagerDepend::TbSoundManagerDepend()
    : m_MemoryPoolBuffer(nullptr)
    , m_WorkBuffer(nullptr)
    , m_ConfigBuffer(nullptr)
{
}

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundManagerDepend::TbSoundManagerDepend( const TbSoundInitParam& initParam )
    : m_MemoryPoolBuffer(nullptr)
    , m_WorkBuffer(nullptr)
    , m_ConfigBuffer(nullptr)
{
    initialize(initParam);
}

/*!
 * デストラクタ
 * @author teco
 */
TbSoundManagerDepend::~TbSoundManagerDepend()
{
    nn::audio::StopAudioRenderer(m_RendererHandle);
    nn::audio::CloseAudioRenderer(m_RendererHandle);
    nn::os::DestroySystemEvent(m_SystemEvent.GetBase());
    TbFreeDefaultHeap(m_ConfigBuffer);
    TbFreeDefaultHeap(m_WorkBuffer);
    m_MemoryPoolHeap.Finalize();
    TbFreeDefaultHeap(m_MemoryPoolBuffer);
}

/*!
 * ボイスの遅延破棄リクエスト
 * @author teco
 */
void TbSoundManagerDepend::RequestDeleteVoice(TbSoundSourceDepend::Voice* voice)
{
    m_DeleteVoiceList.PushBack(voice);
}

/*!
 * 初期化
 * @author teco
 */
TbResult TbSoundManagerDepend::initialize( const TbSoundInitParam& initParam )
{
    static const int RANDER_RATE = 48000;
    static const int RENDER_COUNT = (RANDER_RATE / 200);

    // レンダラのパラメータを指定します。
    nn::audio::AudioRendererParameter parameter;
    nn::audio::InitializeAudioRendererParameter(&parameter);
    parameter.sampleRate = RANDER_RATE;
    parameter.sampleCount = RENDER_COUNT;
    parameter.mixBufferCount = 2;
    parameter.voiceCount = 128;
    parameter.subMixCount = 0;
    parameter.sinkCount =   1;
    parameter.effectCount = 0;
    parameter.performanceFrameCount = 0;

    // レンダラのワークバッファを準備します。
    size_t workBufferSize = nn::audio::GetAudioRendererWorkBufferSize(parameter);
    m_WorkBuffer = TbMallocDefaultHeap(workBufferSize,nn::os::MemoryPageSize);

    nn::Result result = nn::audio::OpenAudioRenderer(&m_RendererHandle, 
                                                 &m_SystemEvent, 
                                                 parameter, m_WorkBuffer, workBufferSize);
    TB_ASSERT(result.IsSuccess());

    // AudioRendererConfig を初期化します。
    size_t configBufferSize = nn::audio::GetAudioRendererConfigWorkBufferSize(parameter);
    m_ConfigBuffer = TbMallocDefaultHeap(configBufferSize, nn::os::MemoryPageSize);
    nn::audio::InitializeAudioRendererConfig(&m_RendererConfig, parameter, m_ConfigBuffer, configBufferSize);

    // FinalMix設定
    nn::audio::AcquireFinalMix(&m_RendererConfig, &m_FinalMix, 2);

    // 出力デバイス登録
    result = nn::audio::AddDeviceSink(&m_RendererConfig, 
                                                 &m_Sink, 
                                                 &m_FinalMix, MAIN_BUS, CHANNEL_MAX, "MainAudioOut");
    NN_ABORT_UNLESS(result.IsSuccess());

    // 設定したパラメータをレンダラに反映させます。
    result = nn::audio::RequestUpdateAudioRenderer(m_RendererHandle, &m_RendererConfig);
    NN_ABORT_UNLESS(result.IsSuccess());

    // レンダリングを開始します。
    result = nn::audio::StartAudioRenderer(m_RendererHandle);
    NN_ABORT_UNLESS(result.IsSuccess());

    // WaveBuffer に追加するサンプルデータを保持するためのメモリプールを準備します。
    m_MemoryPoolBuffer = static_cast<TbUint8*>(TbMallocDefaultHeap(MEMORY_POOL_BUUFER_SIZE, nn::os::MemoryPageSize));
    bool rt = AcquireMemoryPool(&m_RendererConfig, &m_MemoryPool, m_MemoryPoolBuffer, MEMORY_POOL_BUUFER_SIZE);
    NN_ABORT_UNLESS(rt);
    rt = RequestAttachMemoryPool(&m_MemoryPool);
    NN_ABORT_UNLESS(rt);

    m_MemoryPoolHeap.Initialize(m_MemoryPoolBuffer, MEMORY_POOL_BUUFER_SIZE,TB_SOUND_BUFFER_ALIGN_SIZE);

    m_DeleteVoiceList.SetEnableSetCapacityAuto(TB_TRUE);
    m_DeleteVoiceList.SetCapacity(64);

    return TB_S_OK;
}

// 共通


/*!
* アロケーション
* @author teco
*/
void* TbSoundManager::AllocBuffer(TbSizeT size, TbSizeT align)
{
    return m_Depend.m_MemoryPoolHeap.AllocateForward(size,align);
}

/*!
* 解放
* @author teco
*/
void TbSoundManager::FreeBuffer(void* ptr)
{
    m_Depend.m_MemoryPoolHeap.Deallocate(ptr);
}

/*!
 * 依存定期処理
 * @author teco
 */
void TbSoundManager::updateDepend()
{
//    if( m_Depend.m_SystemEvent.TryWait() )
    if (m_Depend.m_WorkBuffer)
    {
        nn::Result result = nn::audio::RequestUpdateAudioRenderer(m_Depend.m_RendererHandle,
                                                                  &m_Depend.m_RendererConfig);
        TB_ASSERT(result.IsSuccess());

        for (TbSoundManagerDepend::DeleteVoiceArray::Iterator it = m_Depend.m_DeleteVoiceList.Begin(); it != m_Depend.m_DeleteVoiceList.End(); )
        {
            if ((*it)->IsEnableDelete()) {
                TB_DELETE (*it);
                it = m_Depend.m_DeleteVoiceList.Erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

/*!
 * 音量更新
 * @author teco
 */
void TbSoundManager::updateVolume()
{
    if(m_Depend.m_WorkBuffer)
    {
        nn::audio::SetFinalMixVolume(&m_Depend.m_FinalMix, m_MasterVolume);
    }
}

}

#endif