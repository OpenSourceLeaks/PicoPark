/*!
 * サウンド管理クラスのAPI依存分岐ヘッダー
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_DEPEND_H_
#define _INCLUDED_TB_SOUND_SOURCE_DEPEND_H_

#if TB_SOUNDAPI_IS_AL
    #include "./al/tb_sound_source_al.h"
#elif TB_SOUNDAPI_IS_XAUDIO
    #include "./xaudio/tb_sound_source_xaudio.h"
#elif TB_SOUNDAPI_IS_NX
#include "./nx/tb_sound_source_nx.h"
#elif TB_SOUNDAPI_IS_NONE
    typedef TbUint32 TbSoundSourceDepend;
#else
    #error "Unkonwn SoundAPI"
#endif

#endif
