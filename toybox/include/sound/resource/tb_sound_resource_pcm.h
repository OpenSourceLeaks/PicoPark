/*!
 * リソース(PCM用)
 * プログラムで自由に作成可能
 * @author Miyake Shunsuke
 * @since 2019.07.13
 */

#ifndef _INCLUDED_TB_SOUND_RESOURCE_PCM_H_
#define _INCLUDED_TB_SOUND_RESOURCE_PCM_H_

#include <sound/tb_sound_types.h>
#include <sound/resource/tb_sound_resource.h>
#include <base/io/stream/tb_memory_stream.h>

namespace toybox
{

class TbSoundResourcePcm : public TbSoundResource
{
public:

    typedef TbSoundResource Super;
    typedef TbSoundResourcePcm Self;

public:

    // コンストラクタ
    explicit TbSoundResourcePcm( TbSoundType type , const TbSoundLoadSetting* setting );

    // デストラクタ
    virtual ~TbSoundResourcePcm();

public:
    
    // セットアップ
    void Setup( const TbSoundPcmData& data);

    // 初期化中
    virtual TbBool Initialize();
    
private:

    TbSint32        m_Phase;        // フェーズ
    TbBitArray32    m_BitArray;     // ビット配列
    TbMemoryStream  m_Stream;       // メモリーストリーム

};

}

#endif
