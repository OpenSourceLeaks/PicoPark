/*!
 * ソケット
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */

#ifndef _INCLUDED_TB_SOCKET_H_
#define _INCLUDED_TB_SOCKET_H_

#include <network/tb_network_types.h>
#include <base/task/tb_task.h>

#include "./depend/tb_socket_depend.h"

namespace toybox
{

class TbSocket : public TbTask
{
public:
    typedef TbTask Super;
    typedef TbSocket Self;
    typedef TbStaticFunction32<void (TbSocket*)> ClosedCallback;
public:

    // コンストラクタ
    TbSocket();

    // デストラクタ
    virtual ~TbSocket();

public:

    // 開く
    TbResult Open();

    //! 閉じる
    TbResult Close();

    //! 読み込み可能か
    TbBool IsEnableRead() const {
        return m_BitArray.Test(FLAG_READ);
    }

    //! 書き込み可能か
    TbBool IsEnableWrite() const {
        return m_BitArray.Test(FLAG_WRITE);
    }

    //! 開いているか
    TbBool IsOpened() const {
        return m_BitArray.Test(FLAG_OPEN);
    }

    //! 切断コールバック設定
    void SetClosedCallback( const ClosedCallback& callback ){
        m_ClosedCallback = callback;
    }

    // アドレス取得
    const TbChar8* GetAddress() const {
        return m_Address.GetBuffer();
    }

    TbSint32 GetPort() const {
        return m_Port;
    }

    // アドレス取得
    void SetAddress( const TbChar8* address , TbSint32 port ) {
        m_Address = address;
        m_Port = port;
    }

protected:

    //! 閉じる (デストラクタで閉じる際は呼ばれないので注意)
    virtual void OnClosed() {}

    //! 読み込み可能設定
    void SetEnableRead( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_READ,isEnable);
    }

    //! 書き込み可能設定
    void SetEnableWrite( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_WRITE,isEnable);
    }

public:

    // 依存部分取得
    TbSocketDepend& GetDepend() {
        return m_Depend;
    }

    // 依存部分取得
    const TbSocketDepend& GetDepend() const {
        return m_Depend;
    }

private:

    enum Flag
    {
        FLAG_READ , 
        FLAG_WRITE , 
        FLAG_OPEN , 
        FLAG_MAX 
    };

private:

    // 閉じる(OnCllosedを呼ばないデストラクタ用)
    TbResult close2();

private:

    TbBitArray32        m_BitArray;
    ClosedCallback      m_ClosedCallback;
    TbStaticString32    m_Address;
    TbSint32            m_Port;
    TbSocketDepend      m_Depend;

};

}

#endif
