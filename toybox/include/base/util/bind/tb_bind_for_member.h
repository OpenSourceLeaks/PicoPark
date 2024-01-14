/*!
 * メンバー関数用Bind
 * @author Miyake_Shunsuke
 * @since 2012.04.28
 */

#ifndef _INCLUDED_TB_BIND_FOR_MEMBER_H_
#define _INCLUDED_TB_BIND_FOR_MEMBER_H_

#include "./tb_bind_mem_func.h"

namespace toybox
{

//! 引数1のメンバー関数
template<typename R,typename Class,typename T1,typename A1>
    TbBindImpl< R , TbBindMemFunc1<R,Class,T1> , TbBindList1<A1> > 
    TbBind( R (Class::*func)(T1) , Class* instance , A1 a1 )
{
    typedef TbBindMemFunc1<R,Class,T1> MemFunc;
    typedef TbBindList1<A1> List;
    MemFunc memFunc( instance , func );
    List argList(a1);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数1のconstメンバー関数
template<typename R,typename Class,typename T1,typename A1>
    TbBindImpl< R , TbBindMemFuncConst1<R,Class,T1> , TbBindList1<A1> >
    TbBind( R (Class::*func)(T1) const , const Class* instance , A1 a1 )
{
    typedef TbBindMemFuncConst1<R,Class,T1> MemFunc;
    typedef TbBindList1<A1> List;
    MemFunc memFunc( instance , func );
    List argList(a1);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数1のconstメンバー関数
template<typename R,typename Class,typename T1,typename A1>
    TbBindImpl< R , TbBindMemFuncConst1<R,Class,T1> , TbBindList1<A1> >
    TbBind( R (Class::*func)(T1) const , Class* instance , A1 a1 )
{
    typedef TbBindMemFuncConst1<R,Class,T1> MemFunc;
    typedef TbBindList1<A1> List;
    MemFunc memFunc( static_cast<const Class*>(instance) , func );
    List argList(a1);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数2のメンバー関数
template<typename R,typename Class,typename T1,typename T2,typename A1,typename A2>
    TbBindImpl< R , TbBindMemFunc2<R,Class,T1,T2> , TbBindList2<A1,A2> > 
    TbBind( R (Class::*func)(T1,T2) , Class* instance , A1 a1 , A2 a2 )
{
    typedef TbBindMemFunc2<R,Class,T1,T2> MemFunc;
    typedef TbBindList2<A1,A2> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数2のconstメンバー関数
template<typename R,typename Class,typename T1,typename T2,typename A1,typename A2>
    TbBindImpl< R , TbBindMemFuncConst2<R,Class,T1,T2> , TbBindList2<A1,A2> > 
    TbBind( R (Class::*func)(T1,T2) const , const Class* instance , A1 a1 , A2 a2 )
{
    typedef TbBindMemFuncConst2<R,Class,T1,T2> MemFunc;
    typedef TbBindList2<A1,A2> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数2のconstメンバー関数
template<typename R,typename Class,typename T1,typename T2,typename A1,typename A2>
    TbBindImpl< R , TbBindMemFuncConst2<R,Class,T1,T2> , TbBindList2<A1,A2> > 
    TbBind( R (Class::*func)(T1,T2) const , Class* instance , A1 a1 , A2 a2 )
{
    typedef TbBindMemFuncConst2<R,Class,T1,T2> MemFunc;
    typedef TbBindList2<A1,A2> List;
    MemFunc memFunc( static_cast<const Class*>(instance) , func );
    List argList(a1,a2);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数3のメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,
         typename A1,typename A2,typename A3>
    TbBindImpl< R , TbBindMemFunc3<R,Class,T1,T2,T3> , TbBindList3<A1,A2,A3> > 
    TbBind( R (Class::*func)(T1,T2,T3) , Class* instance , A1 a1 , A2 a2 , A3 a3 )
{
    typedef TbBindMemFunc3<R,Class,T1,T2,T3> MemFunc;
    typedef TbBindList3<A1,A2,A3> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2,a3);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数3のconstメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,
         typename A1,typename A2,typename A3>
    TbBindImpl< R , TbBindMemFuncConst3<R,Class,T1,T2,T3> , TbBindList3<A1,A2,A3> > 
    TbBind( R (Class::*func)(T1,T2,T3) const , const Class* instance , A1 a1 , A2 a2 , A3 a3 )
{
    typedef TbBindMemFuncConst3<R,Class,T1,T2,T3> MemFunc;
    typedef TbBindList3<A1,A2,A3> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2,a3);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数3のconstメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,
         typename A1,typename A2,typename A3>
    TbBindImpl< R , TbBindMemFuncConst3<R,Class,T1,T2,T3> , TbBindList3<A1,A2,A3> > 
    TbBind( R (Class::*func)(T1,T2,T3) const , Class* instance , A1 a1 , A2 a2 , A3 a3 )
{
    typedef TbBindMemFuncConst3<R,Class,T1,T2,T3> MemFunc;
    typedef TbBindList3<A1,A2,A3> List;
    MemFunc memFunc( static_cast<const Class*>(instance) , func );
    List argList(a1,a2,a3);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数4のメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,typename T4,
         typename A1,typename A2,typename A3,typename A4>
    TbBindImpl< R , TbBindMemFunc4<R,Class,T1,T2,T3,T4> , TbBindList4<A1,A2,A3,A4> > 
    TbBind( R (Class::*func)(T1,T2,T3,T4) , Class* instance , A1 a1 , A2 a2 , A3 a3 , A4 a4 )
{
    typedef TbBindMemFunc4<R,Class,T1,T2,T3,T4> MemFunc;
    typedef TbBindList4<A1,A2,A3,A4> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2,a3,a4);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数4のconstメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,typename T4,
         typename A1,typename A2,typename A3,typename A4>
    TbBindImpl< R , TbBindMemFuncConst4<R,Class,T1,T2,T3,T4> , TbBindList4<A1,A2,A3,A4> > 
    TbBind( R (Class::*func)(T1,T2,T3,T4) const , const Class* instance , A1 a1 , A2 a2 , A3 a3 , A4 a4 )
{
    typedef TbBindMemFuncConst4<R,Class,T1,T2,T3,T4> MemFunc;
    typedef TbBindList4<A1,A2,A3,A4> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2,a3,a4);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数4のconstメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,typename T4,
         typename A1,typename A2,typename A3,typename A4>
    TbBindImpl< R , TbBindMemFuncConst4<R,Class,T1,T2,T3,T4> , TbBindList4<A1,A2,A3,A4> > 
    TbBind( R (Class::*func)(T1,T2,T3,T4) const , Class* instance , A1 a1 , A2 a2 , A3 a3 , A4 a4 )
{
    typedef TbBindMemFuncConst4<R,Class,T1,T2,T3,T4> MemFunc;
    typedef TbBindList4<A1,A2,A3,A4> List;
    MemFunc memFunc( static_cast<const Class*>(instance) , func );
    List argList(a1,a2,a3,a4);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数5のメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,typename T4,typename T5,
         typename A1,typename A2,typename A3,typename A4,typename A5>
    TbBindImpl< R , TbBindMemFunc5<R,Class,T1,T2,T3,T4,T5> , TbBindList5<A1,A2,A3,A4,A5> > 
    TbBind( R (Class::*func)(T1,T2,T3,T4,T5) , Class* instance , A1 a1 , A2 a2 , A3 a3 , A4 a4 , A5 a5 )
{
    typedef TbBindMemFunc5<R,Class,T1,T2,T3,T4,T5> MemFunc;
    typedef TbBindList5<A1,A2,A3,A4,A5> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2,a3,a4,a5);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数5のconstメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,typename T4,typename T5,
         typename A1,typename A2,typename A3,typename A4,typename A5>
    TbBindImpl< R , TbBindMemFuncConst5<R,Class,T1,T2,T3,T4,T5> , TbBindList5<A1,A2,A3,A4,A5> > 
    TbBind( R (Class::*func)(T1,T2,T3,T4,T5) const , const Class* instance , A1 a1 , A2 a2 , A3 a3 , A4 a4 , A5 a5 )
{
    typedef TbBindMemFuncConst5<R,Class,T1,T2,T3,T4,T5> MemFunc;
    typedef TbBindList5<A1,A2,A3,A4,A5> List;
    MemFunc memFunc( instance , func );
    List argList(a1,a2,a3,a4,a5);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

//! 引数5のconstメンバー関数
template<typename R,typename Class,
         typename T1,typename T2,typename T3,typename T4,typename T5,
         typename A1,typename A2,typename A3,typename A4,typename A5>
    TbBindImpl< R , TbBindMemFuncConst5<R,Class,T1,T2,T3,T4,T5> , TbBindList5<A1,A2,A3,A4,A5> > 
    TbBind( R (Class::*func)(T1,T2,T3,T4,T5) const , Class* instance , A1 a1 , A2 a2 , A3 a3 , A4 a4 , A5 a5 )
{
    typedef TbBindMemFuncConst5<R,Class,T1,T2,T3,T4,T5> MemFunc;
    typedef TbBindList5<A1,A2,A3,A4,A5> List;
    MemFunc memFunc( static_cast<const Class*>(instance) , func );
    List argList(a1,a2,a3,a4,a5);
    return TbBindImpl<R,MemFunc,List> ( memFunc ,argList);
}

}

#endif
