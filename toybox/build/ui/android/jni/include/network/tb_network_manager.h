/*!
 * ネットワーク管理クラス
 * @author Miyake_Shunsuke
 * @since 2012.04.18
 */

#ifndef _INCLUDED_TB_NETWORK_MANAGER_H_
#define _INCLUDED_TB_NETWORK_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <base/util/tb_function.h>
#include <base/container/tb_array.h>
#include <base/task/tb_task.h>
#include <network/tb_network_types.h>

#include <network/depend/tb_network_manager_depend.h>

namespace toybox
{

class TbPacketBase;
class TbSocket;

class TbNetworkManager : public TbTask , public TbSingletonRef<TbNetworkManager>
{
public:
    typedef TbTask Parent;
    typedef TbNetworkManager Self;
public:

    // コンストラクタ
    TbNetworkManager();

    // デストラクタ
    ~TbNetworkManager();

public:

    // 初期化
    TbResult Initialize( const TbNetworkInitParam& initParam );

    // ソケット追加
    TbResult AddSocket( TbSocket* socket );

    // ソケット除去
    TbResult RemoveSocket( TbSocket* socket );

public:

    // 依存取得
    TbNetworkManagerDepend& GetDepend() {
        return m_Depend;
    }

    // 依存取得
    const TbNetworkManagerDepend& GetDepend() const { 
        return m_Depend;
    }

private:

    enum Flag {
        FLAG_INITIALIZE , 
        FLAG_MAX , 
    };

private:

    // 更新
    void update();

private:

    TbBitArray32            m_BitArray;
    TbArray<TbSocket*>      m_SocketList;   // ソケットリスト
    TbNetworkManagerDepend  m_Depend;       // 依存

};

}

#endif
