/*!
 * TCP用サーバーソケット
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#include "tb_fwd.h"
#include "network/tb_server_socket.h"
#include "network/tb_accept_socket.h"
#include "network/tb_network_manager.h"
#include "network/depend/std/tb_network_util_std.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbServerSocket::TbServerSocket()
    : TbSocket()
    , m_AcceptSocketList()
    , m_AcceptedCallback()
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbServerSocket::~TbServerSocket()
{
    deleteAcceptSocket();
}

/*!
 * ソケットを開く
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbResult TbServerSocket::Open( TbUint32 port , TbUint32 backlog )
{
    if(IsOpened()){
        // 既に開かれている
        return TB_E_DENY;
    }

    m_AcceptSocketList.SetCapacity( backlog);

    sockaddr_in addr;
    TbMemClear(addr);

    SOCKET listenSocket = socket(AF_INET,SOCK_STREAM,0);
    GetDepend().SetSocket(listenSocket);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(port)); // ポート
    TbNetworkUtilStd::SetAddress(&addr,INADDR_ANY);
    
    // バインド
    int ret = bind(listenSocket,(struct sockaddr*)&addr,sizeof(addr));

    // ノンブロッキング設定
    u_long val=1;
    ret = TbIoctl(listenSocket, FIONBIO, &val);
    SetAddress("",port);

    SetEnableRead(TB_TRUE);
    if( !TB_VERIFY(TB_SUCCEEDED(Super::Open())) ){
        return TB_E_FAIL;
    }

    // 設定
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    AddTaskLine(TB_TASK_LINE_SYSTEM_PRE,&Self::update,&manager);

    // 受付準備
    listen(listenSocket,backlog);

    return TB_S_OK;
}

/*!
 * 閉じる
 * @author Miyake_Shunsuke
 * @since 2012.05.21
 */
void TbServerSocket::OnClosed()
{
    deleteAcceptSocket();
}

/*!
 * 更新
 * @author Miyake_Shunsuke
 * @since 2012.05.14
 */
void TbServerSocket::update()
{
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    const fd_set fdSet = manager.GetDepend().GetReadFileDescSetReslt();
    const SOCKET serverSocket = Super::GetDepend().GetSocket();
    if( FD_ISSET(serverSocket,&fdSet) ){

        if( !m_AcceptSocketList.IsFull() )
        {
            // 受け入れ可能なので受け入れ
            TbAcceptSocket* acceptSocket = TB_NEW TbAcceptSocket();
            sockaddr_in fromInfo;
            TbSockInt addrSize = sizeof(fromInfo);
            SOCKET socket = accept(serverSocket,reinterpret_cast<struct sockaddr *>(&fromInfo),&addrSize);
            TbSint32 port = ntohs(fromInfo.sin_port);
            char* addrStr = inet_ntoa(fromInfo.sin_addr);
            acceptSocket->GetDepend().SetSocket(socket);
            acceptSocket->SetAddress(addrStr,port);
            
            if( TB_VERIFY(TB_SUCCEEDED(acceptSocket->Initialize())) ){
                // リストに追加
                m_AcceptSocketList.PushBack(acceptSocket);
                if( m_AcceptedCallback.IsValid() ){
                    m_AcceptedCallback(acceptSocket);
                }
            }else{
                // 初期化失敗
                TB_SAFE_DELETE(acceptSocket);
            }
        }
    }
}

/*!
 * 受け入れソケット削除
 * @author Miyake_Shunsuke
 * @since 2012.05.21
 */
void TbServerSocket::deleteAcceptSocket()
{
    TbArray<TbAcceptSocket*>::Iterator ite = m_AcceptSocketList.Begin();
    TbArray<TbAcceptSocket*>::Iterator endIte = m_AcceptSocketList.End();
    for( ; ite != endIte ; ++ite ){
        TB_DELETE *ite;
    }
    m_AcceptSocketList.Clear();
}

}
