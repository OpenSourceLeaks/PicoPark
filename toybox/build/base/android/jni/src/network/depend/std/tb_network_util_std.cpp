/*!
 * ネットワークユーティリティ系
 * @author Miyake_Shunsuke
 * @since 2012.05.10
 */

#include <tb_fwd.h>
#include <network/depend/std/tb_network_util_std.h>
#include <network/tb_packet.h>

namespace toybox
{
    
/*!
 * パケット送信
 * @author Miyake_Shunsuke
 * @since 2012.05.22
 */
TbResult TbNetworkUtilStd::SendPacket( TbSocket& socket , TbPacketSendBuffer& sendBuffer )
{
    // 送信
    const SOCKET stdSocket = socket.GetDepend().GetSocket();
    TbPacketSendBuffer::PacketInfo* sendPacketInfo = sendBuffer.GetFirst();
    while( sendPacketInfo ) {

        // 送信データを計算
        char* sendBuf = reinterpret_cast<char*>(sendPacketInfo->packet);
        sendBuf += sendPacketInfo->sendSize;
        TbUint32 sendSize = sendPacketInfo->packet->size - sendPacketInfo->sendSize;

        // 送信
        int result = 0;
        if( sendPacketInfo->toInfo ){
            sockaddr_in senderInfo;
            senderInfo.sin_family = AF_INET;
            senderInfo.sin_port = htons(sendPacketInfo->toInfo->port);
            if( sendPacketInfo->toInfo->address.IsEmpty() ){
                senderInfo.sin_addr.s_addr = inet_addr("255.255.255.255"); // ブロードキャスト
            }else{
                senderInfo.sin_addr.s_addr = inet_addr(sendPacketInfo->toInfo->address.GetBuffer());
            }
            result = sendto(stdSocket,sendBuf,sendSize,0,reinterpret_cast<sockaddr*>(&senderInfo),sizeof(senderInfo));
        }else{
            result = send(stdSocket,sendBuf,sendSize,0);
        }

        if( SOCKET_ERROR == result ){
            // エラー
            break;
        }else if( (sendPacketInfo->sendSize+result) < (sendPacketInfo->packet->size) ){
            // 全て送信できていない(現在アサート扱い)
            // TB_ASSERT(!"Can't");
            sendPacketInfo->sendSize += result;
            break;
        }else{
            // 送信完了
            sendBuffer.RemoveFirst();
        }
        sendPacketInfo = sendBuffer.GetFirst();
    }
    return TB_S_OK;
}

}
