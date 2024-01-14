/*!
 * ソケット依存部分
 * @author Miyake_Shunsuke
 */

#ifndef _INCLUDED_TB_SOCKET_DEPEND_H_
#define _INCLUDED_TB_SOCKET_DEPEND_H_

#if TB_NETWORK_API == TB_NETWORK_API_STD
    #include <network/depend/std/tb_socket_std.h>
#endif

#endif