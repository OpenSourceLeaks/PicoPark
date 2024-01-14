/*!
 * 音源音量操作
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_FADE_H_
#define _INCLUDED_TB_SOUND_SOURCE_FADE_H_

#include <sound/tb_sound_source_handle.h>
#include <base/math/tb_math_util.h>
#include <base/container/tb_static_array.h>

namespace toybox
{

/*!
 * 音源音量操作
 * @author teco
 */
class TbSoundSourceFade
{
public:
    // コンストラクタ
    TbSoundSourceFade()
        : m_IsBusy(TB_FALSE)
        , m_CurrentSec(0.0f)
        , m_TargetSec(0.0f)
        , m_TargetVolume(0.0f)
    {
    }
    // デストラクタ
    ~TbSoundSourceFade()
    {
    }
public:
    TbBool IsBusy() const {
        return m_IsBusy;
    }
    // 音源追加
    void AddSource(TbSoundSourceHandle* source)
    {
        if (m_SourceInfo.IsFull()) {
            return;
        }
        SourceInfo info;
        info.source = source;
        m_SourceInfo.PushBack(info);
    }
    // 音源リセット
    void ResetSource()
    {
        m_SourceInfo.Clear();
    }
    // フェード開始
    void Fade(TbFloat32 targetVolume, TbFloat32 sec)
    {
        if (m_SourceInfo.IsEmpty()) {
            return;
        }
        TB_RESQUE(0.0f<=sec);
        if (!m_IsBusy)
        {
            if (TB_FLOAT32_EQUAL(sec, 0.0f))
            {
                TbForIterator(it, m_SourceInfo)
                {
                    it->source->SetVolume(targetVolume);
                }
            }
            else
            {
                m_TargetVolume = targetVolume;
                m_TargetSec = sec;
                m_CurrentSec = 0.0f;
                TbForIterator(it, m_SourceInfo)
                {
                    it->beginVolume = it->source->GetVolume();
                }
                m_IsBusy = TB_TRUE;
            }
        }
    }
public:
    // 定期処理
    void Update(TbFloat32 deltasec)
    {
        if (m_IsBusy)
        {
            m_CurrentSec += deltasec;
            TbFloat32 rate = TbMin(1.0f,m_CurrentSec / m_TargetSec);
            TbForIterator(it, m_SourceInfo)
            {
                TbFloat32 volume = TbLerp(it->beginVolume, m_TargetVolume, rate);
                it->source->SetVolume(volume);
            }
            if (1.0f <= rate) {
                m_IsBusy = TB_FALSE;
            }
        }
    }
private:
    static const TbUint32 MAX = 16;
    struct SourceInfo
    {
        SourceInfo()
            : source(nullptr)
            , beginVolume(0.0f)
        {
        }
        TbSoundSourceHandle* source;
        TbFloat32      beginVolume;
    };
private:
    TbStaticArray<SourceInfo,MAX> m_SourceInfo;
    TbBool                        m_IsBusy;
    TbFloat32                     m_CurrentSec;
    TbFloat32                     m_TargetSec;
    TbFloat32                     m_TargetVolume;
};

}

#endif
