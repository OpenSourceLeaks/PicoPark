/*!
 * グローバル関数用Bind
 * @author Miyake_Shunsuke
 * @since 2012.04.28
 */

#ifndef _INCLUDED_TB_BIND_FOR_GLOBAL_H_
#define _INCLUDED_TB_BIND_FOR_GLOBAL_H_

namespace toybox
{

//! 引数1のグローバル関数
template<typename R,typename T1,typename A1>
    TbBindImpl< R , R (*)(T1 ) , TbBindList1<A1> > 
    TbBind( R (*func)(T1) , A1 a1 )
{
    TbBindList1<A1> argList(a1);
    return TbBindImpl<R,R (*)(T1 ),TbBindList1<A1> > (func,argList);
}

//! 引数2のグローバル関数
template<typename R,typename T1,typename T2,typename A1,typename A2>
TbBindImpl< R , R (*)(T1,T2) , TbBindList2<A1,A2> > TbBind( R (*func)(T1,T2) , A1 a1 , A2 a2 )
{
    TbBindList2<A1,A2> argList(a1,a2);
    return TbBindImpl<R,R (*)(T1,T2),TbBindList2<A1,A2> > (func,argList);
}

//! 引数3のグローバル関数
template<typename R,
         typename T1,typename T2,typename T3,
         typename A1,typename A2,typename A3>
    TbBindImpl< R , R (*)(T1,T2,T3) , 
                TbBindList3<A1,A2,A3> > 
    TbBind( R (*func)(T1,T2,T3) , 
            A1 a1 , A2 a2 , A3 a3 )
{
    typedef TbBindList3<A1,A2,A3> List;
    List argList(a1,a2,a3);
    return TbBindImpl<R,R (*)(T1,T2,T3),List> (func,argList);
}

//! 引数4のグローバル関数
template<typename R,
         typename T1,typename T2,typename T3,typename T4,
         typename A1,typename A2,typename A3,typename A4>
    TbBindImpl< R , R (*)(T1,T2,T3,T4) , 
                TbBindList4<A1,A2,A3,A4> > 
    TbBind( R (*func)(T1,T2,T3,T4) , 
            A1 a1 , A2 a2 , A3 a3 , A4 a4 )
{
    typedef TbBindList4<A1,A2,A3,A4> List;
    List argList(a1,a2,a3,a4);
    return TbBindImpl<R,R (*)(T1,T2,T3,T4),List> (func,argList);
}

//! 引数5のグローバル関数
template<typename R,
         typename T1,typename T2,typename T3,typename T4,typename T5,
         typename A1,typename A2,typename A3,typename A4,typename A5>
    TbBindImpl< R , R (*)(T1,T2,T3,T4,T5) , 
                TbBindList5<A1,A2,A3,A4,A5> > 
    TbBind( R (*func)(T1,T2,T3,T4,T5) , 
            A1 a1 , A2 a2 , A3 a3 , A4 a4 , A5 a5 )
{
    typedef TbBindList5<A1,A2,A3,A4,A5> List;
    List argList(a1,a2,a3,a4,a5);
    return TbBindImpl<R,R (*)(T1,T2,T3,T4,T5),List> (func,argList);
}

//! 引数0のグローバル関数(返り値の型を変更)
template<typename CastR , typename R>
    TbBindImpl< CastR , R (*)() , TbBindList0 > 
    TbRetBind( R (*func)() )
{
    return TbBindImpl<CastR,R (*)(),TbBindList0>(func,TbBindList0());
}

}

#endif

