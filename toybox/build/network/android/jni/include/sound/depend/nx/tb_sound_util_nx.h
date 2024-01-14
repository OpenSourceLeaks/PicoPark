/*!
 * NX依存のユーティリティ関数
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SOUND_UTIL_NX_H_
#define _INCLUDED_TB_SOUND_UTIL_NX_H_

#include <sound/tb_sound_types.h>

namespace toybox
{

/*!
 * フォーマットをNX用に変換
 */
inline TbResult TbSoundFormatToNX( nn::audio::SampleFormat* dst , const TbSoundFormat& src )
{
    TB_RESQUE_RET(TB_SOUND_FORMAT_TYPE_PCM == src.type,TB_E_FAIL);
    switch (src.bitsPerSample)
    {
    case 8:
        *dst = nn::audio::SampleFormat_PcmInt8;
        break;
    case 16:
        *dst = nn::audio::SampleFormat_PcmInt16;
        break;
    case 24:
        *dst = nn::audio::SampleFormat_PcmInt24;
        break;
    case 32:
        *dst = nn::audio::SampleFormat_PcmInt32;
        break;
    }
    return TB_S_OK;
};

}

#endif
