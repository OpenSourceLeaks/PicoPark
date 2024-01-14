/*!
 * 関数オブジェクト用Bind
 * @author Miyake_Shunsuke
 * @since 2012.04.28
 */

#ifndef _INCLUDED_TB_BIND_FOR_OBJECT_H_
#define _INCLUDED_TB_BIND_FOR_OBJECT_H_

namespace toybox
{

//! 引数1の関数オブジェクト
template<typename F,typename A1>
    TbBindImpl< typename F::ResultType , F , TbBindList1<A1> > 
    TbBind( F f , A1 a1 )
{
    typedef TbBindList1<A1> List;
    List argList(a1);
    return TbBindImpl< typename F::ResultType , F ,TbBindList1<A1> > (f,argList);
}

//! 引数2の関数オブジェクト
template<typename F,typename A1,typename A2>
    TbBindImpl< typename F::ResultType , F , TbBindList2<A1,A2> > 
    TbBind( F f , A1 a1 , A2 a2 )
{
    typedef TbBindList2<A1,A2> List;
    List argList(a1,a2);
    return TbBindImpl< typename F::ResultType,F,List> (f,argList);
}

}

#endif
