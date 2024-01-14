/*!
 * サーバーでのクライアントとの接続ソケット
 * @author Miyake_Shunsuke
 * @since 2012.05.01
 */

#ifndef _INCLUDED_TB_ACCEPT_SOCKET_H_
#define _INCLUDED_TB_ACCEPT_SOCKET_H_

#include <network/tb_network_types.h>
#include <network/tb_socket.h>
#include <network/tb_packet_recv_buffer.h>
#include <network/tb_packet_send_buffer.h>

#include "./depend/tb_accept_socket_depend.h"

namespace toybox
{

struct TbPacket;
class TbAcceptSocket : public TbSocket
{
public:

    typedef TbSocket Super;
    typedef TbAcceptSocket Self;
    typedef TbStaticFunction64<void (TbPacket&)> ReceiveCallback;

public:

    // コンストラクタ
    TbAcceptSocket();

    // デストラクタ
    virtual ~TbAcceptSocket();

public:

    // 初期化
    TbResult Initialize();

    // パケット送信
    void Send( TbPacket* packet );

    //! 受信コールバック設定
    void SetReceiveCallback( const ReceiveCallback& callback ){
        m_ReceiveCallback = callback;
    }

protected:

    // 毎フレーム呼ばれる派生クラス用
    virtual void Update() {}

    // 閉じる
    virtual void OnClosed();

private:

    // 更新
    void update();

private:

    TbBitArray32        m_BitArray;         // ビット配列
    ReceiveCallback     m_ReceiveCallback;  // 受信コールバック
    TbUint8             m_RecvBuf[PACKET_RECV_SIZE_MAX];
    TbPacketRecvBuffer  m_PacketRecvBuffer; // パケット受信バッファ
    TbPacketSendBuffer  m_PacketSendBuffer; // パケット送信バッファ

};

}

#endif
