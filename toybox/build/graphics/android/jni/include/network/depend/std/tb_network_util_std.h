/*!
 * ネットワークユーティリティ系
 * @author Miyake_Shunsuke
 * @since 2012.05.10
 */

#ifndef _INCLUDED_TB_NETWORK_UTIL_STD_H_
#define _INCLUDED_TB_NETWORK_UTIL_STD_H_

#include <network/tb_socket.h>
#include <network/tb_packet_send_buffer.h>

namespace toybox
{

class TbNetworkUtilStd
{
public:
    
    // パケット送信
    static TbResult SendPacket( TbSocket& socket , TbPacketSendBuffer& sendBuffer );
    
    // アドレスをsockaddr_inに設定
    static void SetAddress( sockaddr_in* sockaddr , TbUint32 addr ) {
#if TB_PLATFORM_IS_WIN
        sockaddr->sin_addr.S_un.S_addr = addr;
#else
        sockaddr->sin_addr.s_addr = addr;
#endif
    }

};

}

#endif
