/*!
 * 汎用パケット
 * @author teco
 */

#ifndef _INCLUDED_TB_PACKET_COMMON_H_
#define _INCLUDED_TB_PACKET_COMMON_H_

#pragma pack(push,4)

namespace toybox
{

enum
{
    TB_PACKET_MAIN_COMMON = 0
};

enum
{
    TB_PACKET_SUB_PING= 0
};

/*!
 * 応答パケット
 * @author teco
 */
struct TbPacketPing : TbPacket
{
public:
    
    TbPacketPing()
    {
        Init( sizeof(*this) , TB_PACKET_MAIN_COMMON , TB_PACKET_SUB_PING );
    }
    
};

}

#pragma pack(pop)

#endif
