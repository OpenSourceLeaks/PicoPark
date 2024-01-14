/*!
 * OpenAL依存のユーティリティ関数
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#ifndef _INCLUDED_TB_SOUND_UTIL_AL_H_
#define _INCLUDED_TB_SOUND_UTIL_AL_H_

#if TB_SOUNDAPI_IS_AL

namespace toybox
{

/*!
 * フォーマットをOpenAL用に変換
 * @author Miyake Shunsuke
 * @since 2011.07.25
 */
inline TbResult TbConvertTbSoundFormatToALenum( ALenum& dst , const TbSoundFormat& src )
{
    if( TB_SOUND_FORMAT_TYPE_PCM != src.type ) {
        // PCMのみ
        return TB_E_FAIL;
    }

    if( 1 == src.channel ){
        // モノラル
        if(16 == src.bitsPerSample){ // 16bit
            dst = AL_FORMAT_MONO16;
        }else if( 8 == src.bitsPerSample ){ // 8bit
            dst = AL_FORMAT_MONO8;
        }else{
            return TB_E_FAIL;
        }
        return TB_S_OK;
    }else if( 2 == src.channel ){
        // ステレオ
        if(16 == src.bitsPerSample){ // 16bit
            dst = AL_FORMAT_STEREO16;
        }else if( 8 == src.bitsPerSample ){ // 8bit
            dst = AL_FORMAT_STEREO8;
        }else{
            return TB_E_FAIL;
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
};

}

#endif
#endif
