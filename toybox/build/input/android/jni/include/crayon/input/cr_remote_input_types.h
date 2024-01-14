/*!
 * ネットワークコントローラ用定義
 */

#ifndef _INCLUDED_CR_REMOTE_INPUT_TYPES_H_
#define _INCLUDED_CR_REMOTE_INPUT_TYPES_H_

#include <network/tb_packet.h>
#include <base/algorithm/tb_algorithm_misc.h>

#pragma pack(push,4)

namespace crayon
{

#define CR_INPUT_IS_ENABLE_BUILD_BLUETOOTH (0)
    
const TbSint32 CR_REMOTE_INPUT_DATAGRAM_HOST_PORT = 61515;
const TbSint32 CR_REMOTE_INPUT_HOST_PORT = 61516;

enum
{
    CR_REMOTE_INPUT_PACKET_MAIN = 1
};

enum
{
    CR_REMOTE_INPUT_PACKET_SUB_INPUT1 = 0,
    CR_REMOTE_INPUT_PACKET_SUB_INPUT5,
    CR_REMOTE_INPUT_PACKET_SUB_INPUT10,
    CR_REMOTE_INPUT_PACKET_SUB_INPUT20,
    CR_REMOTE_INPUT_PACKET_SUB_INPUT50,
    CR_REMOTE_INPUT_PACKET_SUB_INPUT100
};

/*!
* パッド入力パケット(1人用)
* @author teco
*/
struct CrRemotePacketInput1 : toybox::TbPacket
{
    CrRemotePacketInput1()
        : key(0)
    {
        Init(sizeof(*this), CR_REMOTE_INPUT_PACKET_MAIN, CR_REMOTE_INPUT_PACKET_SUB_INPUT1);
    }

    TbUint32 key;
};
TB_STATIC_ASSERT(sizeof(CrRemotePacketInput1) == 20);

/*!
* パッド入力パケット(10人用)
* @author teco
*/
struct CrRemotePacketInput10 : toybox::TbPacket
{
    static const TbUint32 PAD_MAX = 10;
    static const TbUint8 FLAG_BIT_BROADCAST = TB_BIT(USER_BIT);

    CrRemotePacketInput10()
        : padCount(0)
    {
        toybox::TbFill<TbUint32>(key, 0);
        Init(sizeof(*this), CR_REMOTE_INPUT_PACKET_MAIN, CR_REMOTE_INPUT_PACKET_SUB_INPUT10);
    }

    TbUint32 padCount;
    TbUint32 key[PAD_MAX];
};
TB_STATIC_ASSERT(sizeof(CrRemotePacketInput10) == 60);

#pragma pack(pop)

}

#endif