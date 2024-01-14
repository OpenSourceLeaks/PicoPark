/*!
 * TCP用サーバー接続ソケット
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#include "tb_fwd.h"
#include "network/tb_accept_socket.h"
#include "network/tb_network_manager.h"
#include "network/tb_packet.h"
#include "network/depend/std/tb_network_util_std.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbAcceptSocket::TbAcceptSocket()
    : TbSocket()
    , m_ReceiveCallback()
    , m_PacketRecvBuffer()
    , m_PacketSendBuffer()
{
}
    
/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbAcceptSocket::~TbAcceptSocket()
{
}

/*!
 * 初期化
 * これを呼ぶ前にsocketが設定されている必要がある
 * @author Miyake_Shunsuke
 * @since 2012.05.17
 */
TbResult TbAcceptSocket::Initialize()
{
    SOCKET socket = Super::GetDepend().GetSocket();
    if( !socket ){
        return TB_E_FAIL;
    }

    // ノンブロッキング設定
    u_long val=1;
    TbIoctl(socket, FIONBIO, &val);

    SetEnableRead(TB_TRUE);
    SetEnableWrite(TB_TRUE);

    // タスク設定
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    AddTaskLine(TB_TASK_LINE_SYSTEM_PRE,&Self::update,&manager);
    Super::Open();

    return TB_S_OK;
}

/*!
 * パケット送信
 * @author Miyake_Shunsuke
 * @since 2012.05.16
 */
void TbAcceptSocket::Send( TbPacket* packet )
{
    m_PacketSendBuffer.Add(packet);
}

/*!
 * 更新
 * @author Miyake_Shunsuke
 * @since 2012.05.16
 */
void TbAcceptSocket::update()
{
    TbNetworkManager& manager = TbNetworkManager::GetInstance();

    const fd_set& readFDSet = manager.GetDepend().GetReadFileDescSetReslt();
    const fd_set& writeFDSet = manager.GetDepend().GetWriteFileDescSetReslt();
    const SOCKET socket = Super::GetDepend().GetSocket();

    // 読み込み
    if( FD_ISSET(socket,&readFDSet) ){
        int readSize = recv(socket,reinterpret_cast<char*>(m_RecvBuf),sizeof(m_RecvBuf),0);
        if( SOCKET_ERROR == readSize || 0 == readSize ){ // SOCKET_ERRORは異常切断、0は正常切断
            // 切断
            Close();
            return;
        }else{
            m_PacketRecvBuffer.Add(m_RecvBuf,readSize);
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
                m_ReceiveCallback(TbRef(*packet->packet));
            }
            m_PacketRecvBuffer.RemoveFirst();
            packet = m_PacketRecvBuffer.GetFirst();
        }
    }
    Update();
}

/*!
 * 閉じる
 * @author Miyake_Shunsuke
 * @since 2012.05.19
 */
void TbAcceptSocket::OnClosed()
{
    RemoveTaskLine(TB_TASK_LINE_SYSTEM_PRE);
}

}
