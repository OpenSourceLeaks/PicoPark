/*!
 * ネットワーク管理クラス
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#include "tb_fwd.h"
#include "network/tb_network_manager.h"
#include "network/tb_socket.h"

namespace toybox
{

    /*!
     * コンストラクタ
     * @author Miyake_Shunsuke
     * @since 2012.04.24
     */
    TbNetworkManager::TbNetworkManager()
        : Parent(1, TB_TASK_NAME("TbNetworkManager"))
        , m_Depend()
    {
    }

    /*!
     * デストラクタ
     * @author Miyake_Shunsuke
     * @since 2012.04.24
     */
    TbNetworkManager::~TbNetworkManager()
    {
        if (m_BitArray.Test(FLAG_INITIALIZE)){
#if (TB_PLATFORM_IS_WIN)
            WSACleanup();
#endif
        }
    }

    /*!
     * 初期化
     * @author Miyake_Shunsuke
     * @since 2012.04.24
     */
    TbResult TbNetworkManager::Initialize(const TbNetworkInitParam& initParam)
    {
        if (m_BitArray.Test(FLAG_INITIALIZE)){
            return TB_E_DENY;
        }
#if (TB_PLATFORM_IS_WIN)
        // win32固有処理
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
        AddTaskLine(TB_TASK_LINE_SYSTEM_PRE, &Self::update);
        m_SocketList.SetCapacity(initParam.socketMax);
        m_BitArray.SetBit(FLAG_INITIALIZE);
        return TB_S_OK;
    }

    /*!
     * ソケット追加
     * 多重IOに追加する
     * @author Miyake_Shunsuke
     * @since 2012.05.14
     */
    TbResult TbNetworkManager::AddSocket(TbSocket* socket)
    {
        if (!TB_VERIFY(!m_SocketList.IsFull())){
            // 初期想定以上のソケット追加が発生した
            return TB_E_FAIL;
        }
        if (!m_BitArray.Test(FLAG_INITIALIZE)){
            return TB_E_FAIL;
        }
        const SOCKET soc = socket->GetDepend().GetSocket();
        // 読み込み用
        if (socket->IsEnableRead()){
            FD_SET(soc, &m_Depend.m_ReadFileDescSet);

            if (soc >= m_Depend.m_ReadFileDescMax){
                m_Depend.m_ReadFileDescMax = soc;
            }
        }
        // 書き込み用
        if (socket->IsEnableWrite()){
            FD_SET(soc, &m_Depend.m_WriteFileDescSet);

            if (soc >= m_Depend.m_WriteFileDescMax){
                m_Depend.m_WriteFileDescMax = soc;
            }
        }
        m_SocketList.PushBack(socket);
        return TB_S_OK;
    }

    /*!
     * ソケット除去
     * 多重IOに追加する
     * @author Miyake_Shunsuke
     * @since 2012.05.14
     */
    TbResult TbNetworkManager::RemoveSocket(TbSocket* socket)
    {
        if (!socket || !m_BitArray.Test(FLAG_INITIALIZE)){
            return TB_E_FAIL;
        }
        // リストから除去
    {
        TbArray<TbSocket*>::Iterator ite = m_SocketList.Begin();
        TbArray<TbSocket*>::Iterator endIte = m_SocketList.End();
        for (; ite != endIte; ++ite){
            if (socket == *ite){
                m_SocketList.Erase(ite);
                break;
            }
        }
    }

    const SOCKET soc = socket->GetDepend().GetSocket();
    // 読み込み用
    if (socket->IsEnableRead()){
        FD_CLR(soc, &m_Depend.m_ReadFileDescSet);
        if (m_Depend.m_ReadFileDescMax == soc)
        {
            m_Depend.m_ReadFileDescMax = 0;
            TbForIterator(ite, m_SocketList){
                if ((*ite)->IsEnableRead() && m_Depend.m_ReadFileDescMax < (*ite)->GetDepend().GetSocket()){
                    m_Depend.m_ReadFileDescMax = (*ite)->GetDepend().GetSocket();
                }
            }
        }
    }
    // 書き込み用
    if (socket->IsEnableWrite()){
        FD_CLR(soc, &m_Depend.m_WriteFileDescSet);
        if (m_Depend.m_WriteFileDescMax == soc)
        {
            m_Depend.m_WriteFileDescMax = 0;
            TbForIterator(ite, m_SocketList){
                if ((*ite)->IsEnableWrite() && m_Depend.m_WriteFileDescMax < (*ite)->GetDepend().GetSocket()){
                    m_Depend.m_WriteFileDescMax = (*ite)->GetDepend().GetSocket();
                }
            }
        }
    }

    return TB_S_OK;
    }

    /*!
     * 更新
     * @author Miyake_Shunsuke
     * @since 2012.05.14
     */
    void TbNetworkManager::update()
    {
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        // 読み込み
        if (m_Depend.m_ReadFileDescMax > 0){
            m_Depend.m_ReadFileDescSetResult = m_Depend.m_ReadFileDescSet;
            m_Depend.m_ReadFileDescSetResultCount =
                select(m_Depend.m_ReadFileDescMax + 1, &m_Depend.m_ReadFileDescSetResult, NULL, NULL, &timeout);
            if (m_Depend.m_ReadFileDescSetResultCount > 0){
                int a = 0;
            }
        }
        // 書き込み
        if (m_Depend.m_WriteFileDescMax > 0){
            m_Depend.m_WriteFileDescSetResult = m_Depend.m_WriteFileDescSet;
            m_Depend.m_WriteFileDescSetResultCount =
                select(m_Depend.m_WriteFileDescMax + 1, NULL, &m_Depend.m_WriteFileDescSetResult, NULL, &timeout);
            if (m_Depend.m_WriteFileDescSetResultCount > 0){
                int a = 0;
            }
        }
    }

}
