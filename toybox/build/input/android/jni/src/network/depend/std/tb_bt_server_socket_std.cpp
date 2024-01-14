/*!
 * BlueTooth用サーバーソケット
 * @author teco
 */

#include "tb_fwd.h"
#include "network/tb_bt_server_socket.h"
#include "network/tb_accept_socket.h"
#include "network/tb_network_manager.h"

#include <ws2bth.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbBtServerSocket::TbBtServerSocket()
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
TbBtServerSocket::~TbBtServerSocket()
{
    deleteAcceptSocket();
}

/*!
 * ソケットを開く
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbResult TbBtServerSocket::Open( TbUint32 backlog )
{
    if(IsOpened()){
        // 既に開かれている
        TB_E_DENY;
    }

    m_AcceptSocketList.SetCapacity( backlog);
    const SOCKET listenSocket = socket(AF_BTH,SOCK_STREAM,BTHPROTO_RFCOMM);

//    struct 
    SOCKADDR_BTH addr;
    memset(&addr,0,sizeof(addr));
    addr.addressFamily = AF_BTH;
    addr.port = BT_PORT_ANY;
    bind(listenSocket,(struct sockaddr*)&addr,sizeof(addr));

    int len = sizeof(addr);
    getsockname(listenSocket, (SOCKADDR *)&addr, &len);

    // ノンブロッキング設定
    u_long val=1;
    ioctlsocket(listenSocket, FIONBIO, &val);

    GetDepend().SetSocket(listenSocket);
    SetEnableRead(TB_TRUE);
    if( !TB_VERIFY(TB_SUCCEEDED(Super::Open())) ){
        return TB_E_FAIL;
    }

    // 設定
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    AddTaskLine(TB_TASK_LINE_SYSTEM_PRE,&Self::update,&manager);

    // サービス登録
    CSADDR_INFO info = {0};
    info.LocalAddr.lpSockaddr = (LPSOCKADDR) &addr;
    info.LocalAddr.iSockaddrLength = sizeof(addr);
    info.iSocketType = SOCK_STREAM;
    info.iProtocol = BTHPROTO_RFCOMM;

    WSAQUERYSET set = {0};
    set.dwSize = sizeof(WSAQUERYSET);                              // Must be set to sizeof(WSAQUERYSET)
    set.dwOutputFlags = 0;                                         // Not used
    set.lpszServiceInstanceName = "Server";                        // Recommended.
    set.lpServiceClassId = (LPGUID)&SerialPortServiceClass_UUID;   // !ここのUUID(00001101-0000-1000-8000-00805F9B34FB)はClient側とマッチしている必要がある!
    set.lpVersion = NULL;                                          // Not used.
    set.lpszComment = NULL;                                        // Optional.
    set.dwNameSpace = NS_BTH;                                      // Must be NS_BTH.
    set.lpNSProviderId = NULL;                                     // Not required.
    set.lpszContext = NULL;                                        // Not used.
    set.dwNumberOfProtocols = 0;                                   // Not used.
    set.lpafpProtocols = NULL;                                     // Not used.
    set.lpszQueryString = NULL;                                    // not used.
    set.dwNumberOfCsAddrs = 1;                                     // Must be 1.
    set.lpcsaBuffer = &info;                                       // Pointer to a CSADDR_INFO.
    set.lpBlob = NULL;                                             // Optional.

    if (WSASetService(&set, RNRSERVICE_REGISTER, 0) != 0) {
        return TB_E_FAIL;
    }

    // 受付準備
    listen(listenSocket,backlog);

    return TB_S_OK;
}

/*!
 * 閉じる
 * @author Miyake_Shunsuke
 * @since 2012.05.21
 */
void TbBtServerSocket::OnClosed()
{
    deleteAcceptSocket();
}

/*!
 * 更新
 * @author Miyake_Shunsuke
 * @since 2012.05.14
 */
void TbBtServerSocket::update()
{
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    const fd_set fdSet = manager.GetDepend().GetReadFileDescSetReslt();
    SOCKET serverSocket = Super::GetDepend().GetSocket();
    if( FD_ISSET(serverSocket,&fdSet) ){

        if( !m_AcceptSocketList.IsFull() )
        {
            // 受け入れ可能なので受け入れ
            TbAcceptSocket* acceptSocket = TB_NEW TbAcceptSocket();

            SOCKADDR_BTH fromInfo;
            int addrSize = sizeof(fromInfo);
            SOCKET socket = accept(serverSocket,reinterpret_cast<struct sockaddr *>(&fromInfo),&addrSize);
            acceptSocket->GetDepend().SetSocket(socket);
            
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
void TbBtServerSocket::deleteAcceptSocket()
{
    TbArray<TbAcceptSocket*>::Iterator ite = m_AcceptSocketList.Begin();
    TbArray<TbAcceptSocket*>::Iterator endIte = m_AcceptSocketList.End();
    for( ; ite != endIte ; ++ite ){
        TB_DELETE *ite;
    }
    m_AcceptSocketList.Clear();
}

}
