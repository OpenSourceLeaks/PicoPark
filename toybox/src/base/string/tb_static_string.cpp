/*!
 * 固定サイズ文字列定義
 * @author Miyake Shunsuke
 * @since 2010.12.31
 */

#include "tb_fwd.h"
#include "base/string/tb_static_string.h"

namespace toybox
{

#define TB_STATIC_STRING_EMPTY_IMPL(charSize,size) \
    template<> \
    const TbStaticStringBase<TbChar##charSize,size> TbStaticStringBase<TbChar##charSize,size>::EMPTY

    /*
TB_STATIC_STRING_EMPTY_IMPL(8,8);
TB_STATIC_STRING_EMPTY_IMPL(8,16);
TB_STATIC_STRING_EMPTY_IMPL(8,32);
TB_STATIC_STRING_EMPTY_IMPL(8,64);
TB_STATIC_STRING_EMPTY_IMPL(8,128);
TB_STATIC_STRING_EMPTY_IMPL(8,256);
TB_STATIC_STRING_EMPTY_IMPL(8,512);*/

}
