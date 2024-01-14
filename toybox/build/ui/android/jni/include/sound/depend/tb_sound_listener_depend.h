/*!
 * サウンド管理クラスのAPI依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#ifndef _INCLUDED_TB_SOUND_LISTENER_DEPEND_H_
#define _INCLUDED_TB_SOUND_LISTENER_DEPEND_H_

#if TB_SOUNDAPI_IS_AL || TB_SOUNDAPI_IS_XAUDIO || TB_SOUNDAPI_IS_NONE || TB_SOUNDAPI_IS_NX
    typedef TbUint32 TbSoundListenerDepend; // 特に特定のデータを持たない
#else
    #error "Unkonwn SoundAPI"
#endif

#endif
