/*!
 * パケット受信バッファ
 * @author teco
 */

#include <tb_fwd.h>
#include "network/tb_packet_util.h"
#include "network/tb_packet.h"

namespace toybox
{

/*!
 * ID比較(低いとtrue);
 * @author teco
 */
TbBool TbPacketUtil::CompareId( TbSint16 left , TbSint16 right )
{
    TB_ASSERT(left!=right); // 同一IDはありえない
    TbSint32 groupL = left/(TbPacket::ID_MAX/3);
    TbSint32 groupR = left/(TbPacket::ID_MAX/3);
    if( groupL == groupR ){
        return left < right;
    }
    if( 0 == groupL && 2 == groupR ) {
        return TB_FALSE;
    }
    if( 2 == groupL && 0 == groupR ) {
        return TB_TRUE;
    }
    return groupL < groupR;
}

}
