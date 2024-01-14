/*!
 * Bluetooth用クライアントソケット
 * @author teco
 */

#include <tb_fwd.h>
#include "network/tb_network_manager.h"
#include "network/tb_bt_client_socket.h"
#include "network/tb_packet.h"
#include "network/depend/std/tb_network_util_std.h"
#include "base/time/tb_tick.h"

#include <ws2bth.h>

namespace toybox
{

namespace
{
    TbBool isConnected( const SOCKET& socket ){
        // 接続チェック
        int optVal=-1;
        int optValSize =sizeof(optVal);
        int result = getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)&optVal,(int*)&optValSize);
        if( 0 == result && 0 == optVal ){
            return TB_TRUE;
        }
        return TB_FALSE;
    }
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.05.16
 */
TbBtClientSocket::TbBtClientSocket()
    : TbSocket()
    , m_ConnectWaitSec(0.0f)
    , m_TimeOut(0.0f)
    , m_ReceiveCallback()
    , m_PacketRecvBuffer()
    , m_PacketSendBuffer()
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.05.16
 */
TbBtClientSocket::~TbBtClientSocket()
{
}

/*!
 * 接続開始
 * @param address 接続先IPアドレス
 * @param port    接続先ポート番号
 * @author teco
 */
TbResult TbBtClientSocket::Open( const TbBluetoothDeviceInfo& device )
{
    if(IsOpened()){
        // 既に開かれている
        return TB_E_DENY;
    }

    // ソケット設定
    const SOCKET sock = ::socket(AF_BTH,SOCK_STREAM,BTHPROTO_RFCOMM);
    GetDepend().SetSocket(sock);

    // 接続先
    SOCKADDR_BTH server;
    TbMemClear(server);
    server.addressFamily = AF_BTH;
    server.serviceClassId = SerialPortServiceClass_UUID;
    server.btAddr = device.id;
    server.port = 0;

    // ノンブロッキング設定
    u_long val=1;
    ::ioctlsocket(sock, FIONBIO, &val);

    SetEnableRead(TB_TRUE);
    SetEnableWrite(TB_TRUE);
    if( !TB_VERIFY(TB_SUCCEEDED(Super::Open())) ){
        return TB_E_FAIL;
    }
    
    SetAddress( "" , 0 );

    // タスク設定
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    AddTaskLine(TB_TASK_LINE_SYSTEM_PRE,&Self::update,&manager);

    // 接続
    int error = ::WSAGetLastError();
    int result = ::connect(sock,(struct sockaddr*)&server,sizeof(server));
    error = ::WSAGetLastError();
    // ノンブロッキングモードなのでエラーが返る
    if( result < 0 ){
#if (TB_PLATFORM_IS_WIN)
        int error = ::WSAGetLastError();
        if (WSAEWOULDBLOCK != error) {    // win32固有処理
            return TB_E_FAIL;
        }
#endif
    }else{
        if( isConnected(sock) ){
            // 接続済み
            m_BitArray.SetBit(FLAG_CONNECT);
        }
    }
    m_ConnectWaitSec = 0.0f;
    return TB_S_OK;
}

/*!
 * サーバーにパケット送信
 * @author teco
 */
void TbBtClientSocket::Send( TbPacket* packet )
{
    // バッファに積む
    m_PacketSendBuffer.Add(packet);
}

/*!
 * 閉じる
 * @author teco
 */
void TbBtClientSocket::OnClosed()
{
    RemoveTaskLine(TB_TASK_LINE_SYSTEM_PRE);
    m_BitArray.SetBit(FLAG_CONNECT,TB_FALSE);
}

/*!
 * 更新
 * @author teco
 */
void TbBtClientSocket::update()
{
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    const fd_set& readFDSet = manager.GetDepend().GetReadFileDescSetReslt();
    const fd_set& writeFDSet = manager.GetDepend().GetWriteFileDescSetReslt();
    SOCKET socket = Super::GetDepend().GetSocket();

    if( !IsConnected() ){
        if( manager.GetDepend().GetWriteFileDescSetResltCount() > 0 &&  FD_ISSET(socket,&writeFDSet) ){
            if( isConnected(socket) ){
                // 接続済み
                m_BitArray.SetBit(FLAG_CONNECT);
            }else{
                // 閉じる
                Close();
                return;
            }
        }
        if( 0.0f < m_TimeOut ) {
            m_ConnectWaitSec += TbTick::GetFrameSpeedSeconds();
            if( m_TimeOut <= m_ConnectWaitSec ){
                // タイムアウト閉じる
                Close();
                return;
            }
        }
    }else{
        // 読み込み
        if( FD_ISSET(socket,&readFDSet) ){
            int readSize = recv(socket,reinterpret_cast<char*>(m_RecvBuf),sizeof(m_RecvBuf),0);
            if( 0 == readSize || 0xFFFFFFFF == readSize ){
                // 閉じる
                Close();
                return;
            } else {
                // データ受信
                m_PacketRecvBuffer.Add(m_RecvBuf,readSize);
            }
        }
        // 送信可能
        if( FD_ISSET(socket,&writeFDSet) ){
            // 送信
            TbNetworkUtilStd::SendPacket(*this,m_PacketSendBuffer);
        }
    }

    // 受信パケットを通知する
    {
        TbPacketRecvBuffer::PacketInfo* packet = m_PacketRecvBuffer.GetFirst();
        while( packet ){
            if( m_ReceiveCallback.IsValid() ){
                m_ReceiveCallback(packet->packet);
            }
            m_PacketRecvBuffer.RemoveFirst();
            packet = m_PacketRecvBuffer.GetFirst();
        }
    }
}

}
