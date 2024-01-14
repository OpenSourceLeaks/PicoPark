/*!
 * パケット送信バッファ
 * @author teco
 */

#include <tb_fwd.h>
#include "network/tb_packet_send_buffer.h"
#include "network/tb_packet.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbPacketSendBuffer::TbPacketSendBuffer()
    : m_RootNode()
    , m_PacketId(0)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbPacketSendBuffer::~TbPacketSendBuffer()
{
    while( m_RootNode ){
        RemoveFirst();
    }
}

/*!
 * パケット追加
 * @author teco
 */
void TbPacketSendBuffer::Add( TbPacket* packet , ToInfo* to )
{
    if( !packet ){
        return;
    }
    TbUint8* packetBuf = TB_NEW TbUint8[ packet->size ];
    TbMemCopy( packetBuf , packet , packet->size );
    Node* node = TB_NEW Node();
    node->packetInfo.packet = reinterpret_cast<TbPacket*>(packetBuf);
    node->packetInfo.packet->id = m_PacketId;
    ++m_PacketId;
    if( TbPacket::ID_MAX <= m_PacketId ) {
        m_PacketId = 0;
    }
    node->packetInfo.sendSize = 0;
    if( to ){
        node->packetInfo.toInfo = TB_NEW ToInfo();
        *node->packetInfo.toInfo = *to;
    }else{
        node->packetInfo.toInfo = nullptr;
    }
    node->next = node;
    node->prev = node;
    if( m_RootNode ){
        node->prev = m_RootNode->prev;
        m_RootNode->prev = node;
        node->next = m_RootNode;
        node->prev->next = node;
    }else{
        m_RootNode = node;
    }
}

/*!
 * 先頭パケット取得
 * @author teco
 */
TbPacketSendBuffer::PacketInfo* TbPacketSendBuffer::GetFirst()
{
    return m_RootNode ? &m_RootNode->packetInfo : NULL;
}

/*!
 * 先頭パケット削除
 * @author teco
 */
void TbPacketSendBuffer::RemoveFirst()
{
    if( m_RootNode ){
        TbUint8* buf = reinterpret_cast<TbUint8*>(m_RootNode->packetInfo.packet);
        TB_SAFE_DELETE_ARRAY(buf);
        if( m_RootNode->packetInfo.toInfo ) {
            TB_SAFE_DELETE(m_RootNode->packetInfo.toInfo);
        }
        Node* next = m_RootNode->next;
        if( next == m_RootNode ){
            next = NULL;
        }else{
            next->prev = m_RootNode->prev;
            m_RootNode->prev->next = next;
        }
        TB_SAFE_DELETE(m_RootNode);
        m_RootNode = next;
    }
}

};