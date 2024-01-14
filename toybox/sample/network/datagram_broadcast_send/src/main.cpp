#include "fwd.h"
#include <base/system/tb_console.h>
#include <framework/tb_framework_cui.h>
#include <network/tb_network_all.h>

namespace
{
    using namespace toybox;
    TbNetworkManager* s_NetworkManager = nullptr;
    TbDatagramSocket* s_Socket = nullptr;

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
     * @author teco
     */
    void onReceivedPacket( TbPacket& packet , const char* address , TbSint32 port ) {
        MyPacket& myPacket = reinterpret_cast<MyPacket&>(packet);
        TbConsole::Printf("Receveid MyPacket = %s",myPacket.string);
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
    s_Socket = new TbDatagramSocket();
    s_Socket->SetReceiveCallback(onReceivedPacket);
    s_Socket->Open(12343);

    MyPacket packet;
    packet.Init(sizeof(MyPacket),1,1);
    TbStrPrintf( packet.string , TB_ARRAY_COUNT_OF(packet.string) , "test" );
    s_Socket->Send("127.0.0.1",12344,&packet);

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
