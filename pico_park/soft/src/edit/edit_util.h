/*!
* 編集ユーティリティ
* @author teco
*/

#ifndef _INCLUDED_EDIT_UTIL_H_
#define _INCLUDED_EDIT_UTIL_H_

#include <base/math/tb_geometry.h>
#include "root.h"

class EditUtil
{
public:

    static void PrintfLine( TbPointS32* pos , const char* fmt, ... )
    {
        static const TbUint32 BUF_SIZE = 512;
        va_list ap;
        TbChar8    buf[BUF_SIZE];
        va_start(ap, fmt);
        TbVStrPrintf(buf, BUF_SIZE, fmt, ap);
        va_end(ap);
        TbFontBuffer2d* font = Root::GetInstance().GetDebugFont();
        TbUint32 fontSize = font->GetFontSize();
        TbUint32 x = pos->x * (fontSize+2);
        TbUint32 y = pos->y * (fontSize+2);
        font->Printf(x,y,buf);
        ++pos->y;
    }

    static const char* GetMapChipName(TbUint32 mapChip)
    {
        static const char* TABLE[] =
        {
            "MC_INV",
            "MC_NON",
            "MC_BLK",
            "MC_FLC",
            "MC_FLL",
            "MC_FLR",
            "MC_CEC",
            "MC_CEL",
            "MC_CER",
            "MC_WAL",
            "MC_WAR",
            "MC_INC",
            "MC_ILU",
            "MC_IRU",
            "MC_ILD",
            "MC_IRD",
            "MC_IUP",
            "MC_IDW",
            "MC_ILE",
            "MC_IRG",
            "MC_BHU",
            "MC_BHC",
            "MC_BHD",
            "MC_BWL",
            "MC_BWC",
            "MC_BWR",

            "MC_DLU",
            "MC_DLD",
            "MC_DRU",
            "MC_DRD",
            "MC_BR1",
            "MC_BR2",
            "MC_BR3",
            "MC_BR4",
            "MC_BR5",
        };
        if (mapChip < TB_ARRAY_COUNT_OF(TABLE)) {
            return TABLE[mapChip];
        }
        TB_ASSERT(0);
        return nullptr;
    }

};

#endif