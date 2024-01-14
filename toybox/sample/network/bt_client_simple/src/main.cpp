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

    TbNetworkManager*               s_NetworkManager = NULL;
    TbBtClientSocket*               s_ClientSocket = NULL;
    TbSint32                        s_Phase = PHASE_INIT;
    TbSint32                        s_DeviceIndex = 0;
    TbArray<TbBluetoothDeviceInfo>* s_DeviceInfo;

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
            s_ClientSocket = new TbBtClientSocket();
            s_ClientSocket->SetTimeOut( 1.0f );
            s_ClientSocket->Open(s_DeviceInfo->GetAt(s_DeviceIndex)); //  接続
            TbConsole::Printf("Connecting...(%s)\n",s_DeviceInfo->GetAt(s_DeviceIndex).deviceName.GetBuffer());
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
            s_DeviceIndex = (s_DeviceIndex+1)%s_DeviceInfo->GetCount();
            s_Phase = PHASE_INIT;
        }
        break;
    case PHASE_READY:
        if( !s_ClientSocket->IsConnected() ) {
            TbConsole::Printf("Closed\n");
            s_Phase = PHASE_INIT;
        }
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

    // 端末検索
    TbSint32 deviceIndex = 0xFFFF;
    s_DeviceInfo = new TbArray<TbBluetoothDeviceInfo>();
    s_DeviceInfo->SetCapacity(10);
    {
        TbBluetoothUtil::SearchDevice(s_DeviceInfo);
        if( s_DeviceInfo->IsEmpty() ){
            TbPrintf("[Error] Device is empty\n");
            return;
        }
        TbSint32 index = 0;
        TbForIterator( it , *s_DeviceInfo ) {
            TbConsole::Printf("%d : %s\n" , index , it->deviceName.GetBuffer() );
            ++index;
        }
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
    TbFrameworkCui::Finalize();
}

}

