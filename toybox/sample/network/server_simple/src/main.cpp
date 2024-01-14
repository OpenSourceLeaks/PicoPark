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
     * ���C���X���b�h�ł̃��C�����[�v
     * @author Miyake Shunsuke
     * @since 2011.05.11
     */
    void mainLoop()
    {
    }

    /*!
     * �p�P�b�g��M
     * @author Miyake Shunsuke
     * @since 2012.12.11
     */
    void onReceivedPacket( TbPacket& packet ) {
        MyPacket& myPacket = reinterpret_cast<MyPacket&>(packet);
        TbConsole::Printf("Receveid MyPacket = %s",myPacket.string);
    }

    /*!
     * �N���C�A���g�Ƃ̐ڑ�����
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
 * �G���g���[�|�C���g
 * @author Miyake Shunsuke
 * @since 2011.05.11
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
    s_ServerSocket = new TbServerSocket();
    s_ServerSocket->SetAcceptedCallback( onAcceptedClient );
    s_ServerSocket->Open(12345,5);

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
