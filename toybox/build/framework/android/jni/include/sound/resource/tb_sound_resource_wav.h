/*!
 * リソース(WAV用)
 * @author Miyake Shunsuke
 * @since 2012.03.30
 */

#ifndef _INCLUDED_TB_SOUND_RESOURCE_WAV_H_
#define _INCLUDED_TB_SOUND_RESOURCE_WAV_H_

#include <sound/resource/tb_sound_resource.h>
#include <base/io/stream/tb_file_stream.h>
#include <base/io/tb_file.h>

namespace toybox
{

class TbFile;

class TbSoundResourceWav : public TbSoundResource
{
public:

    typedef TbSoundResource Super;
    typedef TbSoundResourceWav Self;

public:

    // コンストラクタ
    explicit TbSoundResourceWav( const char* filename , TbSoundType type , const TbSoundLoadSetting* setting );

    // デストラクタ
    virtual ~TbSoundResourceWav();

public:

    // 初期化中
    virtual TbBool Initialize();

private:

    enum Flag
    {
        FLAG_WAIT_READ , 
    };

    struct GUID {
        TbUint32 data1;
        TbUint16 data2;
        TbUint16 Data3;
        TbUint8  Data4[ 8 ];
    };

    struct FileHeader
    {
        TbChar8 riff[4];
        TbUint32 fileSize;
        TbChar8 wave[4];
    };

    struct RiffChunck
    {
        TbChar8 tag[4];
        TbUint32 size;
    };

    struct FormatInfo
    {
        TbUint16 formatId;           // フォーマットID
        TbUint16 channelCount;       // チャンネル数
        TbUint32 samplesPerSec;      // サンプリングレート 
        TbUint32 averageBytesPerSec; // データ速度
        TbUint16 blockAlign;         // ブロックサイズ 
        TbUint16 bitsPerSample;      // サンプルあたりのビット数
        TbUint16 size;               // 拡張サイズ
        union {
            TbUint16 validBitsPerSample;       /* bits of precision  */
            TbUint16 samplesPerBlock;          /* valid if wBitsPerSample==0 */
            TbUint16 reserved;                 /* If neither applies, set to zero. */
        } sample;
        TbUint32 channelMask;        // チャンネルマスク
        GUID subFormat;          // サブフォーマット
    };

private:

    // 読み込み完了
    void onRead();

    // フォーマット取得
    TbBool getSoundFormat( TbSoundFormat& dst , const FormatInfo& src );

    // 読みこみ
    template <typename T>
    TbResult read( T* dst ){
        return read( sizeof(T) , dst , sizeof(T) );
    }

    // 読みこみ
    TbResult read( TbSizeT size , void* dst , TbSizeT dstSize ) 
    {
        m_FileOffset += size;
        TbResult result = m_File.Read(size,dst,dstSize,nullptr);
        if( TB_S_OK == result || TB_E_PENDING == result ) {
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }

private:

    TbSint32        m_Phase;        // フェーズ
    TbBitArray32    m_BitArray;     // ビット配列
    FileHeader      m_Header;       // ヘッダー
    RiffChunck      m_Chunk;        // チャンク
    FormatInfo      m_FormatInfo;   // フォーマット情報
    TbFileStream    m_FileStream;   // ストリーミング用ストリーム
    TbFile          m_File;         // ファイル
    TbSizeT         m_FileOffset;   // オフセット

};

}

#endif
