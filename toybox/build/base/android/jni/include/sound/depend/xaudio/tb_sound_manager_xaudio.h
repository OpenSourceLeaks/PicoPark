/*!
 * XAudio依存のサウンド管理クラス
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_MANAGER_XAUDIO_H_
#define _INCLUDED_TB_SOUND_MANAGER_XAUDIO_H_

#include <sound/tb_sound_types.h>
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

    IXAudio2* GetXAudio() {
        return m_XAudio2;
    }

    const IXAudio2* GetXAudio() const {
        return m_XAudio2;
    }

    IXAudio2MasteringVoice* GetMasteringVoice() {
        return m_MasteringVoice;
    }

    const IXAudio2MasteringVoice* GetMasteringVoice() const {
        return m_MasteringVoice;
    }

private:

    // 初期化
    TbResult initialize( const TbSoundInitParam& initParam );

private:
    IXAudio2*               m_XAudio2;
    IXAudio2MasteringVoice* m_MasteringVoice;
};

}

#endif
