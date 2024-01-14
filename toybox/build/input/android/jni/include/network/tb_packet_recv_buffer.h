/*!
 * パケット受信バッファ
 * @author Miyake_Shunsuke
 * @since 2012.05.01
 */

#ifndef _INCLDUED_TB_PACKET_RECV_BUFFER_H_
#define _INCLDUED_TB_PACKET_RECV_BUFFER_H_

#include <base/container/tb_queue.h>

namespace toybox
{

struct TbPacket;

class TbPacketRecvBuffer
{
public:

    struct PacketInfo
    {
        TbPacket*        packet;
        TbStaticString16 addess;
        TbSint32         port;
    };

public:

    // コンストラクタ
    TbPacketRecvBuffer();
    // デストラクタ
    ~TbPacketRecvBuffer();

public:
    
    // 受信バッファ取得
    void Add( void* buffer , TbUint32 bufSize , const char* address = nullptr , TbSint32 port = 0 );
    
    // 先頭パケット取得
    PacketInfo* GetFirst();
    
    // 先頭パケット削除
    void RemoveFirst();
    
private:
    
    TbPacket*           m_ReceivingPacket;      // 受信中パケット
    TbUint32            m_ReceivingPacketSize;  // 
    TbUint32            m_ReceivingSize;        // パケットの受信中サイズ
    TbQueue<PacketInfo> m_PacketQueue;   // パケットキュー
    
};

}

#endif
