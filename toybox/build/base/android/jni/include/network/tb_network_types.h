/*!
 * ネットワーク関連の定義
 * @author Miyake_Shunsuke
 * @since 2012.05.01
 */

#ifndef _INCLUDED_TB_NETWORK_TYPES_H_
#define _INCLUDED_TB_NETWORK_TYPES_H_

#define TB_NETWORK_API_STD 1
#define TB_NETWORK_API_IOCP 2

#if (TB_PLATFORM_IS_WIN)
#   define TB_NETWORK_API TB_NETWORK_API_STD
#   pragma comment( lib, "ws2_32.lib" )
#   define TbIoctl ioctlsocket
    
#elif (TB_PLATFORM_IS_ANDROID)
#   define TB_NETWORK_API TB_NETWORK_API_STD
#   include <fcntl.h>
#   include <sys/socket.h> // ifa_addr
#   include <netinet/in.h> // sockaddr_in
#   include <sys/ioctl.h> // ioctl
#   include <net/if.h> // ifreq
#   include <arpa/inet.h> // inet_ntop
#   include <unistd.h>
#   define SOCKET int
#   define INVALID_SOCKET (-1)
#   define SOCKET_ERROR (-1)
#   define TbIoctl ioctl

#elif (TB_PLATFORM_IS_IOS)
#   define TB_NETWORK_API TB_NETWORK_API_STD
#   include <fcntl.h>
#   include <ifaddrs.h> // ifaddrs
#   include <sys/socket.h> // ifa_addr
#   include <netinet/in.h> // sockaddr_in
#   include <sys/ioctl.h> // ioctl
#   include <net/if.h> // ifreq
#   include <arpa/inet.h> // inet_ntop
#   include <unistd.h>
#   define SOCKET int
#   define INVALID_SOCKET (-1)
#   define SOCKET_ERROR (-1)
#   define TbIoctl ioctl
    
#else
#   error "Invalid Platform"
#endif

namespace toybox
{
    
#if (TB_PLATFORM_IS_WIN)
    typedef int TbSockInt;
    
#elif (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_IOS)
    typedef socklen_t TbSockInt;
#else
#   error "Invalid Platform"
#endif

// クライアントID
typedef TbUint32 TbNetworkId;

// パケット受信サイズの最大値
#define PACKET_RECV_SIZE_MAX (1024 * 30)

struct TbNetworkInitParam
{
    TbUint32 socketMax;
};

}

#endif
