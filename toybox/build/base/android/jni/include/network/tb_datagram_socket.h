/*!
 * Datagramソケット(UDP)
 * @author teco
 */

#ifndef _INCLUDED_TB_DATAGRAM_SOCKET_H_
#define _INCLUDED_TB_DATAGRAM_SOCKET_H_

#include <network/tb_network_types.h>
#include <network/tb_socket.h>
#include <network/tb_packet_recv_buffer.h>
#include <network/tb_packet_send_buffer.h>

#include "./depend/tb_datagram_socket_depend.h"

namespace toybox
{

struct TbPacket;

class TbDatagramSocket : public TbSocket
{
public:

    typedef TbSocket Super;
    typedef TbDatagramSocket Self;
    typedef TbStaticFunction32<void (TbPacket&,const char* fromAddress,TbSint32 port)> ReceiveCallback;

public:

    // コンストラクタ
    TbDatagramSocket();

    // デストラクタ
    virtual ~TbDatagramSocket();

public:

    // 接続開始
    TbResult Open( TbUint32 port );

    // ブロードキャスト有効設定
    void SetEnableBroadcast( TbBool isEnable );

public:

    // パケット送信
    void Send( const char* address , TbUint16 port , TbPacket* packet );

    //! 受信コールバック設定
    void SetReceiveCallback( const ReceiveCallback& callback ){
        m_ReceiveCallback = callback;
    }

protected:

    // 閉じる
    virtual void OnClosed();

private:

    // 更新
    void update();

private:
    ReceiveCallback         m_ReceiveCallback;      // 受信コールバック
    TbUint8                 m_RecvBuf[PACKET_RECV_SIZE_MAX];
    TbPacketRecvBuffer      m_PacketRecvBuffer;     // パケット受信バッファ
    TbPacketSendBuffer      m_PacketSendBuffer;     // パケット送信バッファ
    TbDatagramSocketDepend    m_Depend;               // 依存部分
};

}

#endif
