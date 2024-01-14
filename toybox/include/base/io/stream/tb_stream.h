/*!
 * ストリームの抽象クラス
 * @author Miyake Shunsuke
 * @since 2012.03.01
 */

#ifndef _INCLUDED_TB_STREAM_H_
#define _INCLUDED_TB_STREAM_H_

#include <base/io/tb_io_types.h>
#include <base/util/tb_function.h>

namespace toybox
{

class TbStream
{
public:
    typedef TbFunction<void ()> CallbackType;
public:
    
    // コンストラクタ
    TbStream() : m_BitArray() , m_Size(0) , m_Seek(0) , m_CompletedReadCallback() , m_CompletedWriteCallback() {};
    
    // デストラクタ
    ~TbStream() {};
    
public:

    // 読み込み
    virtual TbResult Read( TB_OUT void* buf , TbUint32 offset , TbUint32 count ) = 0;

    // 読み込み中 (Read関数が完了復帰できなかった場合のチェック関数)
    TbBool IsReading() const {
        return m_BitArray.Test( FLAG_READING );
    }

    // 書き込み
    virtual TbResult Write( const void* buf , TbUint32 count ) = 0;
    
    // 読み込み中 (Write関数が完了復帰できなかった場合のチェック関数)
    TbBool IsWriting() const {
        return m_BitArray.Test( FLAG_WRITING );
    }

    // シーク
    virtual TbResult Seek( TbSint32 offset , TbIOSeekType type ) = 0;

public: // コールバック

    // 読み込み完了コールバック設定
    void SetCompletedReadCallback( const CallbackType& callback ) {
        m_CompletedReadCallback = callback;
    }

    // 書き込み完了コールバック設定
    void SetCompletedWriteCallback( const CallbackType& callback ) {
        m_CompletedWriteCallback = callback;
    }

public: // 読込み
        
    // サイズ取得
    TB_FORCE_INLINE TbUint32 GetSize() const {
        return m_Size;
    }
    
    // 位置取得
    TB_FORCE_INLINE TbUint32 GetSeek() const {
        return m_Seek;
    }

protected:
    
    // 読み込み中設定
    void SetReading( TbBool isReading ){
        m_BitArray.SetBit(FLAG_READING,isReading);
    }

    // 書き込み中設定
    void SetWriting( TbBool isWriting ){
        m_BitArray.SetBit(FLAG_WRITING,isWriting);
    }

    // サイズ設定
    TB_FORCE_INLINE void SetSize( TbUint32 size ) {
        m_Size = size;
    }

    // 現在位置設定
    TB_FORCE_INLINE void SetSeek( TbUint32 seek ) {
        m_Seek = seek;
    }

    // 読み込み間慮うコールバック呼び出し
    void CallCompletedReadCallback() {
        if( m_CompletedReadCallback.IsValid() ){
            m_CompletedReadCallback();
        }
    }

    // 書き込み完了コールバック呼び出し
    void CallCompletedWriteCallback() {
        if( m_CompletedReadCallback.IsValid() ){
            m_CompletedReadCallback();
        }
    }

private:

    enum Flag {
        FLAG_READING ,
        FLAG_WRITING ,
    };

protected:

     CallbackType m_CompletedReadCallback;  // 読み込み完了コールバック
     CallbackType m_CompletedWriteCallback; // 書き込み完了コールバック

private:
    
    TbBitArray32    m_BitArray;    // ビット配列
    TbUint32        m_Size;        // 長さ
    TbUint32        m_Seek;        // 現在位置

};

}

#endif
