/*!
 * UDP用ソケット
 * @author teco
 */

#include <tb_fwd.h>
#include "network/tb_network_manager.h"
#include "network/tb_datagram_socket.h"
#include "network/tb_packet.h"
#include "network/depend/std/tb_network_util_std.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbDatagramSocket::TbDatagramSocket()
    : TbSocket()
    , m_ReceiveCallback()
    , m_PacketRecvBuffer()
    , m_PacketSendBuffer()
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbDatagramSocket::~TbDatagramSocket()
{
}

/*!
 * 開始
 * @param address 接続先IPアドレス
 * @param port    接続先ポート番号
 * @author teco
 */
TbResult TbDatagramSocket::Open( TbUint32 port )
{
    if(IsOpened()){
        // 既に開かれている
        return TB_E_DENY;
    }

    // ソケット設定
    const SOCKET sock = socket(AF_INET,SOCK_DGRAM,0);
    TbDebugPrintf("sock=%d\n",sock);
    GetDepend().SetSocket(sock);

    // 接続先
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(static_cast<u_short>(port));
    TbNetworkUtilStd::SetAddress(&sockAddr,INADDR_ANY);
    SetAddress("",port);

    // バインド
    int ret = bind(sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
    if( !TB_VERIFY_MSG_FORMAT(0==ret,"ret=%d",ret) ){
        return TB_E_FAIL;
    }

    // ノンブロッキング設定
    u_long val=1;
    ret = TbIoctl(sock, FIONBIO, &val);
    if( !TB_VERIFY_MSG_FORMAT(0==ret,"ret=%d",ret) ){
        return TB_E_FAIL;
    }

    SetEnableRead(TB_TRUE);
    SetEnableWrite(TB_TRUE);
    if( !TB_VERIFY(TB_SUCCEEDED(Super::Open())) ){
        return TB_E_FAIL;
    }

    // タスク設定
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    AddTaskLine(TB_TASK_LINE_SYSTEM_PRE,&Self::update,&manager);

    return TB_S_OK;
}

// ブロードキャスト有効設定
void TbDatagramSocket::SetEnableBroadcast( TbBool isEnable )
{
    // ブロードキャスト
#if TB_PLATFORM_IS_WIN
    BOOL yes = isEnable;
#else
    int yes = isEnable;
#endif
    const SOCKET sock = Super::GetDepend().GetSocket();
    setsockopt(sock,SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
}

/*!
 * サーバーにパケット送信
 * @author Miyake_Shunsuke
 * @since 2012.05.16
 */
void TbDatagramSocket::Send( const char* address , TbUint16 port , TbPacket* packet )
{
    // バッファに積む
    TbPacketSendBuffer::ToInfo toInfo;
    toInfo.address = address;
    toInfo.port = port;
    m_PacketSendBuffer.Add(packet,&toInfo);
}

/*!
 * 閉じる
 * @author Miyake_Shunsuke
 * @since 2012.05.19
 */
void TbDatagramSocket::OnClosed()
{
    RemoveTaskLine(TB_TASK_LINE_SYSTEM_PRE);
}

/*!
 * 更新
 * @author teco
 */
void TbDatagramSocket::update()
{
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    const fd_set& readFDSet = manager.GetDepend().GetReadFileDescSetReslt();
    const fd_set& writeFDSet = manager.GetDepend().GetWriteFileDescSetReslt();
    SOCKET socket = Super::GetDepend().GetSocket();

    // 読み込み
    if( FD_ISSET(socket,&readFDSet) ){
        while( 1 )
        {
            struct sockaddr_in fromInfo;
            TbSockInt addrLen = sizeof(fromInfo);
            int readSize = recvfrom(socket,
                                    reinterpret_cast<char*>(m_RecvBuf),
                                    sizeof(m_RecvBuf),0,
                                    reinterpret_cast<sockaddr*>(&fromInfo),
                                    &addrLen);
            if( readSize <= 0 ){
                break;
            }
            TbSint32 port = ntohs(fromInfo.sin_port);
            char* addrStr = inet_ntoa(fromInfo.sin_addr);
            m_PacketRecvBuffer.Add(m_RecvBuf,readSize,addrStr,port);
        }
    }
    // 送信可能
    if( FD_ISSET(socket,&writeFDSet) ){
        // 送信
        TbNetworkUtilStd::SendPacket(*this,m_PacketSendBuffer);
    }

    // 受信パケットを通知する
    {
        TbPacketRecvBuffer::PacketInfo* packet = m_PacketRecvBuffer.GetFirst();
        while( packet ){
            if( m_ReceiveCallback.IsValid() ){
                m_ReceiveCallback(TbRef(*packet->packet),packet->addess.GetBuffer(),packet->port);
            }
            m_PacketRecvBuffer.RemoveFirst();
            packet = m_PacketRecvBuffer.GetFirst();
        }
    }
}

}
