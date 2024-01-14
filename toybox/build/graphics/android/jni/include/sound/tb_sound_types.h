/*!
 * サウンドの型定義
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#ifndef _INCLUDED_TB_SOUND_TYPES_H_
#define _INCLUDED_TB_SOUND_TYPES_H_

#include <base/string/tb_static_string.h>

// SoundAPI
#define TB_SOUNDAPI_AL 1    // OpenAL
#define TB_SOUNDAPI_SL 2    // OpenSL
#define TB_SOUNDAPI_XAUDIO 3    // XAudio
#define TB_SOUNDAPI_NX 4     // NX専用
#define TB_SOUNDAPI_NONE 5    // なし

#if TB_PLATFORM_IS_WIN
#if TB_IS_DEBUG
    #pragma comment(lib,"libogg_static_d.lib")
    #pragma comment(lib,"libvorbis_static_d.lib")
    #pragma comment(lib,"libvorbisfile_static_d.lib")
#else
    #pragma comment(lib,"libogg_static.lib")
    #pragma comment(lib,"libvorbis_static.lib")
    #pragma comment(lib,"libvorbisfile_static.lib")
#endif
#if 1
    #define TB_SOUNDAPI_TARGET TB_SOUNDAPI_XAUDIO
#else
    #define TB_SOUNDAPI_TARGET TB_SOUNDAPI_AL
#endif
#elif TB_PLATFORM_IS_NX
    #define TB_SOUNDAPI_TARGET TB_SOUNDAPI_NX
#elif TB_PLATFORM_IS_EMSCRIPTEN
    #define TB_SOUNDAPI_TARGET TB_SOUNDAPI_NONE
#else
    #error "Invalid Platform" 

#endif

#define TB_SOUNDAPI_IS_AL (TB_SOUNDAPI_TARGET==TB_SOUNDAPI_AL)
#define TB_SOUNDAPI_IS_SL (TB_SOUNDAPI_TARGET==TB_SOUNDAPI_SL)
#define TB_SOUNDAPI_IS_XAUDIO (TB_SOUNDAPI_TARGET==TB_SOUNDAPI_XAUDIO)
#define TB_SOUNDAPI_IS_NX (TB_SOUNDAPI_TARGET==TB_SOUNDAPI_NX)
#define TB_SOUNDAPI_IS_NONE (TB_SOUNDAPI_TARGET==TB_SOUNDAPI_NONE)

#define TB_SOUND_IS_ENABLE_STREAM_THREAD ((1 && !TB_SOUNDAPI_IS_NONE) || TB_SOUNDAPI_IS_NX)

#if 0
#define TB_SOUND_PRINTF(fmt, ...) TbPrintf(fmt,__VA_ARGS__)
#else
#define TB_SOUND_PRINTF(fmt, ...)
#endif

#include "./depend/tb_sound_types_depend.h"
#ifndef TB_SOUND_BUFFER_ALIGN_SIZE
#   define TB_SOUND_BUFFER_ALIGN_SIZE (16)
#endif

namespace toybox
{

class TbFile;
class TbSoundResource;

typedef TbUint32 TbSoundId;
#define TB_SOUND_ID_INVALID (static_cast<TbUint32>(0U))

// フォーマットの種類
enum TbSoundFormatType
{
    TB_SOUND_FORMAT_TYPE_INVALID = -1 , 
    TB_SOUND_FORMAT_TYPE_BEGIN , 
    TB_SOUND_FORMAT_TYPE_PCM = TB_SOUND_FORMAT_TYPE_BEGIN , // PCM
    TB_SOUND_FORMAT_TYPE_IEEE_FLOAT ,
    TB_SOUND_FORMAT_TYPE_END , 
    TB_SOUND_FORMAT_TYPE_MAX = TB_SOUND_FORMAT_TYPE_END - TB_SOUND_FORMAT_TYPE_BEGIN , 
};

// サウンドフォーマット
struct TbSoundFormat
{
    TbSoundFormat()
        : type(TB_SOUND_FORMAT_TYPE_INVALID)
        , channel(0)
        , samplesPerSec(0)
        , bitsPerSample(0)
    {}
    ~TbSoundFormat() {}
        
    TbSoundFormatType type;          // 種類
    TbUint32          channel;       // チャンネル数
    TbUint32          samplesPerSec; // 周波数(Hz)
    TbUint32          bitsPerSample;  // bit/sample
};

// サウンドファイルタイプ
enum TbSoundFileType
{
    TB_SOUND_FILE_TYPE_UNKNOWN , // 未知
    TB_SOUND_FILE_TYPE_WAV , // wavファイル
    TB_SOUND_FILE_TYPE_OGG , // oggファイル
    TB_SOUND_FILE_TYPE_MAX
};

// サウンドタイプ
enum TbSoundType
{
    TB_SOUND_TYPE_INVALID = -1 ,
    TB_SOUND_TYPE_STATIC , // 静的
    TB_SOUND_TYPE_STREAM , // ストリーム
    TB_SOUND_TYPE_MAX
};

// 初期化パラメータ
struct TbSoundInitParam
{
    TbSoundInitParam() 
        : soundMax(1)
        , resourceListType(0)
        , layerCount(1)
        , simplePlayCount(1)
    {}
    TbUint32 soundMax;             // 静的サウンド最大数初期値
    TbUint32 resourceListType;     // リソースが所属するリストインデックス
    TbUint32 layerCount;           // レイヤー数
    TbUint32 simplePlayCount;        // 同時再生可能なSE数
};

//サウンドリソース情報
struct TbSoundResourceInfo
{
    TbSoundId               id;         //!< ID
    TbStaticString128       name;       //!< 名前
    TbSoundType             type;       //!< タイプ
    TbSoundResource*        resource;   //!< リソース
};

// サウンド読み込み時の設定
struct TbSoundLoadSetting
{
    TbSoundLoadSetting() 
        : skipSec(0.0f)
        , loopFirstSec(0.0f)
    {}
    TbFloat32               skipSec; // 途中再生時間
    TbFloat32               loopFirstSec;
};

}

#endif

