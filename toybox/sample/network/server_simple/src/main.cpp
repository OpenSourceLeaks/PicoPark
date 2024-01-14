#include "fwd.h"
#include <base/system/tb_console.h>
#include <framework/tb_framework_cui.h>
#include <network/tb_network_all.h>

namespace
{
    using namespace toybox;
    TbNetworkManager* s_NetworkManager = NULL;
    TbServerSocket*   s_ServerSocket = NULL;

    struct MyPacket : TbPacket
    {
        char string[32];
    };

    /*!
     * メインスレッドでのメインループ
     * @author Miyake Shunsuke
     * @since 2011.05.11
     */
    void mainLoop()
    {
    }

    /*!
     * パケット受信
     * @author Miyake Shunsuke
     * @since 2012.12.11
     */
    void onReceivedPacket( TbPacket& packet ) {
        MyPacket& myPacket = reinterpret_cast<MyPacket&>(packet);
        TbConsole::Printf("Receveid MyPacket = %s",myPacket.string);
    }

    /*!
     * クライアントとの接続完了
     * @author Miyake Shunsuke
     * @since 2012.12.11
     */
    void onAcceptedClient( TbAcceptSocket* socket ){
        socket->SetReceiveCallback( onReceivedPacket );
        TbConsole::Printf("Accepted!!\n");
    }

}

namespace toybox
{

/*!
 * エントリーポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{

    // 初期化
    TbFrameworkCui::Initialize();

    // ネットワーク初期化
    {
        TbNetworkInitParam initParam;
        initParam.socketMax = 5;
        s_NetworkManager = new TbNetworkManager();
        s_NetworkManager->Initialize(initParam);
    }

    // サーバーソケット
    s_ServerSocket = new TbServerSocket();
    s_ServerSocket->SetAcceptedCallback( onAcceptedClient );
    s_ServerSocket->Open(12345,5);

    // メインループ
    TbFrameworkCui::StartMainLoop( NULL , mainLoop , NULL );
}

/*!
 * 終了ポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // 後始末
    TbFrameworkCui::Finalize();
}

}
