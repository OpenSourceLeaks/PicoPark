/*!
 * XAudio
 * @author teco
 */

#include "tb_fwd.h"
#include "sound/tb_sound_manager.h"
#include "sound/tb_sound_source.h"
#include "sound/resource/tb_sound_resource_wav.h"
#include "sound/depend/xaudio/tb_sound_util_xaudio.h"
#include "base/string/tb_string_util.h"

#if TB_SOUNDAPI_IS_XAUDIO

namespace toybox
{

// 依存クラス

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundManagerDepend::TbSoundManagerDepend()
    : m_XAudio2(nullptr)
    , m_MasteringVoice(nullptr)
{
}

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundManagerDepend::TbSoundManagerDepend( const TbSoundInitParam& initParam )
    : m_XAudio2(nullptr)
    , m_MasteringVoice(nullptr)
{
    initialize(initParam);
}

/*!
 * デストラクタ
 * @author teco
 */
TbSoundManagerDepend::~TbSoundManagerDepend()
{
    if( m_MasteringVoice ){
        m_MasteringVoice->DestroyVoice();
        TB_SAFE_RELEASE(m_XAudio2);
        CoUninitialize();
    }
}

/*!
 * 初期化
 * @author teco
 */
TbResult TbSoundManagerDepend::initialize( const TbSoundInitParam& initParam )
{
    CoInitialize(nullptr);

    HRESULT hr = XAudio2Create(&m_XAudio2,0);
    TB_SOUND_PRINTF("XAudio2Create=%d",hr);
    if( FAILED(hr) ){
        TB_ASSERT(SUCCEEDED(hr));
        CoUninitialize();
        return TB_E_FAIL;
    }

    hr = m_XAudio2->CreateMasteringVoice(&m_MasteringVoice);
    TB_SOUND_PRINTF("[TbSoundManagerDepend]CreateMasteringVoice=%d",hr);
    if( FAILED(hr) ){
        TB_ASSERT(SUCCEEDED(hr));
        TB_SAFE_RELEASE(m_XAudio2);
        CoUninitialize();
        return TB_E_FAIL;
    }

    return TB_S_OK;
}

// 共通

/*!
* アロケーション
* @author teco
*/
void* TbSoundManager::AllocBuffer(TbSizeT size, TbSizeT align)
{
    return new TbUint8[size];
}

/*!
* 解放
* @author teco
*/
void TbSoundManager::FreeBuffer(void* ptr)
{
    TbUint8* val = reinterpret_cast<TbUint8*>(ptr);
    TB_SAFE_DELETE_ARRAY(val);
}

/*!
 * 音量更新
 * @author teco
 */
void TbSoundManager::updateVolume()
{
    IXAudio2MasteringVoice* voice = m_Depend.GetMasteringVoice();
    if( voice ){
        voice->SetVolume(m_MasterVolume);
    }
}

/*!
 * 定期処理
 * @author teco
 */
void TbSoundManager::updateDepend()
{
}

}

#endif