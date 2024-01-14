/*!
 * ソケット
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#include "tb_fwd.h"
#include "network/tb_socket.h"
#include "network/tb_network_manager.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbSocket::TbSocket()
    : TbTask(1,TB_TASK_NAME("TbSocket"))
    , m_Port(0)
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
TbSocket::~TbSocket()
{
    close2();
}

/*!
 * 開く
 * @author Miyake_Shunsuke
 * @since 2012.05.19
 */
TbResult TbSocket::Open()
{
    // 管理から外す
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    TbResult result = manager.AddSocket(this);
    m_BitArray.SetBit(FLAG_OPEN,TB_SUCCEEDED(result));
    return result;
}

/*!
 * 閉じる
 * @author Miyake_Shunsuke
 * @since 2012.05.19
 */
TbResult TbSocket::Close()
{
    OnClosed();
    return close2();
}

/*!
 * 閉じる
 * @author Miyake_Shunsuke
 * @since 2012.05.19
 */
TbResult TbSocket::close2()
{
    SOCKET socket = m_Depend.GetSocket();
    if( !socket || !IsOpened() ){
        // 開かれていない場合は何もしない
        return TB_E_FAIL;
    }
    shutdown(socket,2); // 送信、受信両方切断
#if (TB_PLATFORM_IS_WIN)
    closesocket(socket);
#else
    close(socket);
#endif

    // 開く
    m_BitArray.SetBit(FLAG_OPEN,TB_FALSE);

    // 管理から外す
    TbNetworkManager& manager = TbNetworkManager::GetInstance();
    manager.RemoveSocket(this);

    if( m_ClosedCallback.IsValid() ){
        m_ClosedCallback(this);
    }
	m_Depend.SetSocket(INVALID_SOCKET);
	
	return TB_S_OK;
}

}












