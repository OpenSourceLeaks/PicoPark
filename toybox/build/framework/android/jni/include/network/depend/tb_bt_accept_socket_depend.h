﻿/*!
 * Bluetooth用サーバーソケット
 * @author teco
 */

#ifndef _INCLUDED_TB_BT_SERVER_SOCKET_DEPEND_H_
#define _INCLUDED_TB_BT_SERVER_SOCKET_DEPEND_H_

#if TB_NETWORK_API == TB_NETWORK_API_STD
    namespace toybox
    {
        struct TbBtAcceptSocketDepend {};
    }
#endif

#endif