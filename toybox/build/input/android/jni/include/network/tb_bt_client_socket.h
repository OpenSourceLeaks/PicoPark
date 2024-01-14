/*!
 * クライアントソケット
 * @author Miyake_Shunsuke
 * @since 2012.05.01
 */

#ifndef _INCLUDED_TB_BT_CLIENT_SOCKET_H_
#define _INCLUDED_TB_BT_CLIENT_SOCKET_H_

#include <network/tb_network_types.h>
#include <network/tb_bluetooth_types.h>
#include <network/tb_socket.h>
#include <network/tb_packet_recv_buffer.h>
#include <network/tb_packet_send_buffer.h>

#include "./depend/tb_bt_client_socket_depend.h"

namespace toybox
{

struct TbPacket;

class TbBtClientSocket : public TbSocket
{
public:

    typedef TbSocket Super;
    typedef TbBtClientSocket Self;
    typedef TbStaticFunction32<void (TbPacket*)> ReceiveCallback;

public:

    // コンストラクタ
    TbBtClientSocket();

    // デストラクタ
    virtual ~TbBtClientSocket();

public:

    // 接続開始
    TbResult Open( const TbBluetoothDeviceInfo& device );

    // 接続済み
    TbBool IsConnected() const {
        return m_BitArray.Test(FLAG_CONNECT);
    }

public:

    // サーバーにパケット送信
    void Send( TbPacket* packet );

    //! 受信コールバック設定
    void SetReceiveCallback( const ReceiveCallback& callback ){
        m_ReceiveCallback = callback;
    }

    // タイムアウト
    void SetTimeOut( TbFloat32 timeout ) {
        m_TimeOut = timeout;
    }

protected:

    // 閉じる
    virtual void OnClosed();

private:

    enum Flag
    {
        FLAG_CONNECT    , // 接続した
    };

private:

    // 更新
    void update();

private:
    TbBitArray32                m_BitArray;             // ビット配列
    TbFloat32                   m_ConnectWaitSec;
    TbFloat32                   m_TimeOut;
    ReceiveCallback             m_ReceiveCallback;      // 受信コールバック
    TbUint8                     m_RecvBuf[PACKET_RECV_SIZE_MAX];
    TbPacketRecvBuffer          m_PacketRecvBuffer;     // パケット受信バッファ
    TbPacketSendBuffer          m_PacketSendBuffer;     // パケット送信バッファ
    TbBtClientSocketDepend      m_Depend;               // 依存部分
};

}

#endif
