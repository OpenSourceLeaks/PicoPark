/*!
 * サウンド管理クラスのAPI依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#ifndef _INCLUDED_TB_SOUND_MANAGER_DEPEND_H_
#define _INCLUDED_TB_SOUND_MANAGER_DEPEND_H_

#if TB_SOUNDAPI_IS_AL
    #include "./al/tb_sound_manager_al.h"
#elif TB_SOUNDAPI_IS_XAUDIO
    #include "./xaudio/tb_sound_manager_xaudio.h"
#elif TB_SOUNDAPI_IS_NX
    #include "./nx/tb_sound_manager_nx.h"
#elif TB_SOUNDAPI_IS_NONE
    typedef TbUint32 TbSoundManagerDepend;
#else
    #error "Unkonwn SoundAPI"
#endif

#endif
