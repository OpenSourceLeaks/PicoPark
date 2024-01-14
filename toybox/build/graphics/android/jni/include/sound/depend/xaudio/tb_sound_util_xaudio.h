/*!
 * OpenAL依存のユーティリティ関数
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#ifndef _INCLUDED_TB_SOUND_UTIL_XAUDIO_H_
#define _INCLUDED_TB_SOUND_UTIL_XAUDIO_H_

#include <sound/tb_sound_types.h>

namespace toybox
{

/*!
 * フォーマットをOpenAL用に変換
 */
inline TbResult TbConvertTbSoundFormatTo( WAVEFORMATEX& dst , const TbSoundFormat& src )
{
    if( TB_SOUND_FORMAT_TYPE_PCM != src.type ) {
        // PCMのみ
        return TB_E_FAIL;
    }

    TbMemClear(dst);
    dst.wFormatTag = WAVE_FORMAT_PCM;
    dst.nChannels = static_cast<WORD>(src.channel);
    dst.nSamplesPerSec = src.samplesPerSec;
    dst.wBitsPerSample = static_cast<WORD>(src.bitsPerSample);
    dst.nBlockAlign = static_cast<WORD>(src.channel*2);
    dst.nAvgBytesPerSec = dst.nSamplesPerSec * dst.nBlockAlign;
    return TB_S_OK;
};

}

#endif
