/*!
 * NX依存のサウンド管理クラス
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_MANAGER_NX_H_
#define _INCLUDED_TB_SOUND_MANAGER_NX_H_

#include <sound/tb_sound_types.h>
#include <sound/tb_sound_source.h>
#include <base/container/tb_static_array.h>

namespace toybox
{

class TbSoundManagerDepend
{
    friend class TbSoundManager;
public:
    
    // コンストラクタ
    TbSoundManagerDepend();

    // コンストラクタ
    explicit TbSoundManagerDepend( const TbSoundInitParam& initParam );

    // デストラクタ
    ~TbSoundManagerDepend(); 

public:

    // ボイスの遅延破棄リクエスト
    void RequestDeleteVoice(TbSoundSourceDepend::Voice* voice);

    // config取得
    nn::audio::AudioRendererConfig* GetConfig() {
        return &m_RendererConfig;
    }

    // FinalMix取得
    nn::audio::FinalMixType* GetFinalMix() {
        return &m_FinalMix;
    }

private:

    // 初期化
    TbResult initialize( const TbSoundInitParam& initParam );

private:

    static const TbUint32 CHANNEL_MAX = 2;
    typedef TbArray<TbSoundSourceDepend::Voice*> DeleteVoiceArray;

private:

    nn::audio::AudioRendererHandle  m_RendererHandle;
    nn::audio::AudioRendererConfig  m_RendererConfig;
    nn::audio::FinalMixType         m_FinalMix;
    nn::audio::DeviceSinkType       m_Sink;
    nn::audio::MemoryPoolType       m_MemoryPool;
    TbUint8*                        m_MemoryPoolBuffer;
    TbHeap                          m_MemoryPoolHeap;
    void*                           m_WorkBuffer;
    void*                           m_ConfigBuffer;
    nn::os::SystemEvent             m_SystemEvent;  // オーディオフレーム間隔で定期的にシグナルされるイベント
    DeleteVoiceArray                m_DeleteVoiceList;

};

}

#endif
