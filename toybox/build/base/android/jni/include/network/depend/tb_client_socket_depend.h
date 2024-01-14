/*!
 * TCP用サーバーソケット
 * @author Miyake_Shunsuke
 * @since 2012.05.09
 */

#ifndef _INCLUDED_TB_CLIENT_SOCKET_DEPEND_H_
#define _INCLUDED_TB_CLIENT_SOCKET_DEPEND_H_

#if TB_NETWORK_API == TB_NETWORK_API_STD
    namespace toybox {
        struct TbClientSocketDepend 
        {
            sockaddr_in sockAddr;
        };
    }
#endif

#endif
