/*!
 * OpenAL用定義
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */

#ifndef _INCLUDED_TB_SOUND_TYPES_DEPEND_H_
#define _INCLUDED_TB_SOUND_TYPES_DEPEND_H_

#if TB_SOUNDAPI_IS_AL
    #include "./al/tb_sound_types_al.h"
#elif TB_SOUNDAPI_IS_XAUDIO
    #include "./xaudio/tb_sound_types_xaudio.h"
#elif TB_SOUNDAPI_IS_NX
    #include "./nx/tb_sound_types_nx.h"
#elif TB_SOUNDAPI_IS_NONE
#else
    #error "Unkonwn SoundAPI"
#endif

#endif
