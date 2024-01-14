/*!
 * パケット
 * @author Miyake_Shunsuke
 * @since 2012.04.29
 */

#ifndef _INCLUDED_TB_PACKET_H_
#define _INCLUDED_TB_PACKET_H_

#pragma pack(push,4)

namespace toybox
{

enum
{
    TB_PACKET_MAIN_SYSTEM_MAX = 10 // システム予約のメインカテゴリ
};

/*!
 * パケット規定構造体 (12バイト)
 * @author Miyake_Shunsuke
 * @since 2012.04.30
 */
struct TbPacket
{
public:
    
    // 初期化
    void Init( TbUint32 inSize , TbUint16 inMain , TbUint16 inSub ) {
        size = inSize;
        bitArray = 0;
        tag = TAG;
        main = inMain;
        sub = inSub;
        id = 0;
    }

public:
    static const TbUint16 TAG = 0xABCD;
    static const TbSint32 ID_MAX = 30000;
    static const TbUint16 USER_BIT = 4;

    enum Flag
    {
        FLAG_BIT_BIG_ENDIAN = TB_BIT(0) , 
    };
    TbUint32  size;      // サイズ
    TbUint8   bitArray;  // ビット配列
    TbUint8   reserve;   // 予約
    TbUint16  tag;       // タグ
    TbUint16  main;      // メインカテゴリ
    TbUint16  sub;       // サブカテゴリ
    TbSint16  id;        // ID
};
TB_STATIC_ASSERT(sizeof(TbPacket)==16);

}

#pragma pack(pop)

#endif
