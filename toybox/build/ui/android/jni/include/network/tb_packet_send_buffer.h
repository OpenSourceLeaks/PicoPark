/*!
 * パケット送信バッファ
 * @author teco
 */

#ifndef _INCLDUED_TB_PACKET_SEND_BUFFER_H_
#define _INCLDUED_TB_PACKET_SEND_BUFFER_H_

namespace toybox
{

struct TbPacket;

class TbPacketSendBuffer
{
public:

    // 送信先情報
    struct ToInfo
    {
        TbStaticString32 address;
        TbSint16         port;
    };

    struct PacketInfo
    {
        TbPacket*    packet;    // パケット
        TbUint32     sendSize;  // 送信済みサイズ
        ToInfo*      toInfo;  // ユーザーデータ
    };

public:

    // コンストラクタ
    TbPacketSendBuffer();

    // デストラクタ
    ~TbPacketSendBuffer();

public:
    
    // パケット追加
    void Add( TbPacket* packet , ToInfo* to = nullptr );
    
    // 先頭パケット取得
    PacketInfo* GetFirst();
    
    // 先頭パケット削除
    void RemoveFirst();
    
private:
    
    struct Node
    {
        PacketInfo   packetInfo;
        Node*        prev;
        Node*        next;
    };
    
private:
    
    TbSint32    m_PacketId;
    Node*       m_RootNode;
    
};

}

#endif
