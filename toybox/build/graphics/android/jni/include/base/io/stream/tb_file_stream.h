/*!
 * @file
 * @brief ファイルクストリームラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_STREAM_H_
#define _INCLUDED_TB_FILE_STREAM_H_

#include <base/io/stream/tb_stream.h>

namespace toybox
{

class TbFile;

class TbFileStream : public TbStream
{
    typedef TbStream Super;
    typedef TbFileStream  Self;

public:

    // コンストラクタ
    TbFileStream();

    // コンストラクタ
    explicit TbFileStream( TbFile* file );

    // コンストラクタ
    TbFileStream( TbFile* file , TbUint32 begin , TbUint32 size );

    // デストラクタ
    virtual ~TbFileStream();

private: // コピー禁止

    TB_DISALLOW_COPY_AND_ASSIGN( TbFileStream );

public:

    // 参照しているファイル取得
    const TbFile* GetFile() const {
        return m_File;
    }

    // 初期化
    void Initialize( TbFile* file );

    // 初期化
    void Initialize( TbFile* file , TbUint32 begin , TbUint32 size );

public:
    
    // 読み込み
    virtual TbResult Read(TB_OUT void* buf,TbUint32 offset,TbUint32 count);
    
    // 書き込み
    virtual TbResult Write(const void* buf,TbUint32 count);
    
    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type );

private:

    // 初期化
    void setup();

    // 読み込み完了
    void onCompletedRead();

    // 書き込み完了
    void onCompletedWrite();

private:

    TbUint32    m_BeginSeekPos;    // 開始位置
    TbFile*     m_File;            // ファイル

};

}

#endif
