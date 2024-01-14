/*!
 * サーバーソケット
 * @author Miyake_Shunsuke
 * @since 2012.05.01
 */

#ifndef _INCLUDED_TB_SERVER_SOCKET_H_
#define _INCLUDED_TB_SERVER_SOCKET_H_

#include <network/tb_network_types.h>
#include <network/tb_socket.h>
#include <base/container/tb_array.h>

#include "./depend/tb_server_socket_depend.h"

namespace toybox
{

class TbAcceptSocket;

class TbServerSocket : public TbSocket
{
    typedef TbSocket Super;
    typedef TbServerSocket Self;
    typedef TbStaticFunction32<TbAcceptSocket* ()>            CreateAcceptSocketFunc;
    typedef TbStaticFunction32<void (TbAcceptSocket* socket)> AcceptedCallback;
public:

    // コンストラクタ
    TbServerSocket();

    // デストラクタ
    virtual ~TbServerSocket();

public:

    // ソケットを開く
    TbResult Open( TbUint32 port , TbUint32 backlog );

    // クライアントソケットとの接続コールバック
    void SetAcceptedCallback( const AcceptedCallback& callback ) {
        m_AcceptedCallback = callback;
    }

protected:

    // 閉じる
    virtual void OnClosed();

private:

    // 更新
    void update();
    
    // 受け入れソケット削除
    void deleteAcceptSocket();

private:

    TbBitArray32                m_BitArray;
    TbArray<TbAcceptSocket*>    m_AcceptSocketList;         // 接続ソケットリスト
    AcceptedCallback            m_AcceptedCallback;         // 接続時コールバック
    TbServerSocketDepend        m_Depend;                   // 依存

};

}

#endif
