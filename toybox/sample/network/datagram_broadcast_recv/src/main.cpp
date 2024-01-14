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
     * ���C���X���b�h�ł̃��C�����[�v
     * @author teco
     */
    void mainLoop()
    {
    }

    /*!
     * �p�P�b�g��M
     * @author teco
     */
    void onReceivedPacket( TbPacket& packet , const char* addr , TbSint32 port ) {
        MyPacket& myPacket = reinterpret_cast<MyPacket&>(packet);
        TbConsole::Printf("Receveid MyPacket = %s",myPacket.string);
    }

}

namespace toybox
{

/*!
 * �G���g���[�|�C���g
 * @author teco
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{

    // ������
    TbFrameworkCui::Initialize();

    // �l�b�g���[�N������
    {
        TbNetworkInitParam initParam;
        initParam.socketMax = 5;
        s_NetworkManager = new TbNetworkManager();
        s_NetworkManager->Initialize(initParam);
    }

    // �T�[�o�[�\�P�b�g
    s_Socket = new TbDatagramSocket();
    s_Socket->SetReceiveCallback(onReceivedPacket);
    s_Socket->Open(12344);

    // ���C�����[�v
    TbFrameworkCui::StartMainLoop( NULL , mainLoop , NULL );
}

/*!
 * �I���|�C���g
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // ��n��
    TbFrameworkCui::Finalize();
}

}
