/*!
 * パケット受信バッファ
 * @author teco
 */

#include <tb_fwd.h>
#include "base/math/tb_math_util.h"
#include "network/tb_packet_recv_buffer.h"
#include "network/tb_packet.h"

namespace toybox
{

namespace
{
    const TbUint32 QUEUE_SIZE = 100;
};

/*!
 * コンストラクタ
 * @author teco
 */
TbPacketRecvBuffer::TbPacketRecvBuffer()
    : m_PacketQueue(QUEUE_SIZE)
    , m_ReceivingPacket(NULL)
    , m_ReceivingPacketSize(0)
    , m_ReceivingSize(0)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbPacketRecvBuffer::~TbPacketRecvBuffer()
{
    while(!m_PacketQueue.IsEmpty()){
        RemoveFirst();
    }
}

/*!
 * パケット追加
 * @author teco
 */
void TbPacketRecvBuffer::Add( void* buffer , TbUint32 bufSize , const char* address , TbSint32 port )
{
    if( !buffer || 0 == bufSize || 0xFFFFFFFF == bufSize ){
        return;
    }
    
    TbUint32 copySize = 0;
    TbUint8* currentBuf = reinterpret_cast<TbUint8*>(buffer);
    TbUint32 currentBufSize = bufSize;

    while( currentBufSize > 0 )
    {
        if( !m_ReceivingPacket )
        {
            // 受信中のパケットがなければサイズの受診
            TbUint8* packetSizeBuf = reinterpret_cast<TbUint8*>(&m_ReceivingPacketSize);
            packetSizeBuf += m_ReceivingSize;

            // パケットサイズバッファ用の残りサイズ
            TbUint32 leftPacketSizeBufSize = TB_MEMBER_SIZE_OF(TbPacket,size)- m_ReceivingSize;

            // パケットのサイズバッファにコピー
            copySize = TbMin( leftPacketSizeBufSize , currentBufSize );
            TbMemCopy(packetSizeBuf,currentBuf,copySize);

            // ずらす
            m_ReceivingSize += copySize;
            currentBuf += copySize;
            currentBufSize -= copySize;

            if( m_ReceivingSize == TB_MEMBER_SIZE_OF(TbPacket,size) ){
                // パケットサイズを受信完了したら、受信パケットのバッファ確保
                TbUint8* packetBuf = TB_NEW TbUint8[ m_ReceivingPacketSize ];
                m_ReceivingPacket = reinterpret_cast<TbPacket*>(packetBuf);
                m_ReceivingPacket->size = m_ReceivingPacketSize;
            }
        }
        if( m_ReceivingPacket ){

            TbUint32 leftSize = m_ReceivingPacket->size - m_ReceivingSize;
            copySize = TbMin( leftSize , currentBufSize );
            TbUint8* buf = reinterpret_cast<TbUint8*>(m_ReceivingPacket);
            TbMemCopy(&buf[m_ReceivingSize],currentBuf,copySize);

            m_ReceivingSize += copySize;
            currentBuf += copySize;
            currentBufSize -= copySize;

            if( m_ReceivingSize == m_ReceivingPacket->size ){
                // パケット全部受信完了
                PacketInfo packetInfo;
                packetInfo.packet = m_ReceivingPacket;
                if( address ) {
                    packetInfo.addess = address;
                    packetInfo.port = port;
                }
                if( !m_PacketQueue.IsFull() ) {
                    m_PacketQueue.PushBack(packetInfo);
                }else{
                    TbUint8* buf = reinterpret_cast<TbUint8*>(m_ReceivingPacket);
                    TB_SAFE_DELETE_ARRAY(buf);
                }
                m_ReceivingPacket = NULL;
                m_ReceivingSize = 0U;
                m_ReceivingPacketSize = 0U;
            }
        }
    }
}

/*!
 * 先頭パケット取得
 * @author teco
 */
TbPacketRecvBuffer::PacketInfo* TbPacketRecvBuffer::GetFirst()
{
    return !m_PacketQueue.IsEmpty() ? &m_PacketQueue[0] : NULL;
}

/*!
 * 先頭パケット削除
 * @author teco
 */
void TbPacketRecvBuffer::RemoveFirst()
{
    if( !m_PacketQueue.IsEmpty() ){
        TbUint8* buf = reinterpret_cast<TbUint8*>(m_PacketQueue[0].packet);
        TB_SAFE_DELETE_ARRAY(buf);
        m_PacketQueue.PopFront();
    }
}

}
