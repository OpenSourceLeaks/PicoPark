/*!
 * �T�[�o�[�ƃN���C�A���g�ŋ��L����T���v���p�P�b�g
 * @author Miyake Shunsuke
 * @since 2012.12.11
 */

#ifndef _INCLUDED_PACKET_H_
#define _INCLUDED_PACKET_H_

#include <network/tb_packet.h>

struct SimplePacket : TbPacket
{
    char string[32];
};

#endif
