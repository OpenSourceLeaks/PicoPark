/*!
 * パケット系ユーティリティ関数
 * @author teco
 */

#ifndef _INCLUDED_TB_PACKET_UTIL_H_
#define _INCLUDED_TB_PACKET_UTIL_H_

namespace toybox
{

class TbPacketUtil
{
public:

    // ID比較(低いとtrue);
    static TbBool CompareId( TbSint16 left , TbSint16 right );

};

}

#endif
