/*!
 * 型ID用関数
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */

#ifndef _INCLUDED_TB_TYPE_ID_H_
#define _INCLUDED_TB_TYPE_ID_H_

namespace toybox
{

typedef TbAddr TbTypeId;

/*!
 * 型IDを取得
 * @author Miyake Shunsuke
 * @since 2012.02.03
 */
template<typename T>
TbTypeId TbGetTypeId()
{
    static TbUint8 value;
    return reinterpret_cast<TbTypeId>(&value);
}

}

#endif
