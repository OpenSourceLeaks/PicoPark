#include "fwd.h"
#include <base/system/tb_console.h>
#include <framework/tb_framework_cui.h>
#include <network/tb_network_all.h>

using namespace toybox;

namespace
{
    enum Phase
    {
        PHASE_INIT , 
        PHASE_READY = 10 ,
    };

    TbNetworkManager* s_NetworkManager = NULL;
    TbClientSocket*   s_ClientSocket = NULL;
    TbSint32          s_Phase = PHASE_INIT;

    struct MyPacket : TbPacket
    {
        char string[32];
    };
}

/*!
 * メインスレッドでのメインループ
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void mainLoop()
{
    switch( s_Phase )
    {
    case PHASE_INIT:
        {
            // ソケット
            TB_SAFE_DELETE(s_ClientSocket);
            s_ClientSocket = new TbClientSocket();
            s_ClientSocket->SetTimeOut( 3.0f );
            s_ClientSocket->Open("127.0.0.1",12345); //  接続
            TbConsole::Printf("Connecting...\n");
            ++s_Phase;
        }
        break;
    case PHASE_INIT+1:
        if( s_ClientSocket->IsOpened() ){
            if( !s_ClientSocket->IsConnected() ) {
                break;
            }
            TbConsole::Printf("Connected\n");

            // 送信
            MyPacket packet;
            packet.Init(sizeof(MyPacket),1,1);
            TbMemCopy( packet.string , "aaa" , 4 );
            s_ClientSocket->Send( &packet );
            s_Phase = PHASE_READY;
        }else{
            TbConsole::Printf("Closed\n");
            s_Phase = PHASE_INIT;
        }
        break;
    case PHASE_READY:
        if( !s_ClientSocket->IsConnected() ) {
            TbConsole::Printf("Closed\n");
            s_Phase = PHASE_INIT;
        }
        break;
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
    TB_SAFE_DELETE(s_ClientSocket);
    TbFrameworkCui::Finalize();
}

}

