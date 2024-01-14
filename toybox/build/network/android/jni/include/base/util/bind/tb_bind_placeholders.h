/*!
 * 
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */
 
#ifndef _INCLUDED_TB_BIND_PLACEHOLDER_H_
#define _INCLUDED_TB_BIND_PLACEHOLDER_H_

namespace toybox
{

template<TbUint32 index>
struct TbBindPlaceholder{
    TbBindPlaceholder(){}
};

namespace
{
const TbBindPlaceholder<1> _1;
const TbBindPlaceholder<2> _2;
const TbBindPlaceholder<3> _3;
const TbBindPlaceholder<4> _4;
const TbBindPlaceholder<5> _5;
}

}

#endif