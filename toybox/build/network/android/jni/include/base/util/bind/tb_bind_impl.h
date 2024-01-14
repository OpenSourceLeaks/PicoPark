/*!
 * Bindの実装部分
 * @author Miyake Shunsuke
 * @since 2012.03.11
 */
 
 #ifndef _INCLUDED_TB_BIND_IMPL_H_
 #define _INCLUDED_TB_BIND_IMPL_H_

namespace toybox
{

// 関数用
template<typename R,typename F,typename A>
class TbBindImpl
{
public:
    typedef A ArgType;
    typedef F FuncType;
    typedef R ReturnType;
public:

    // コンストラクタ
    TbBindImpl( F func , A arg )
        : m_Func( func )
        , m_BindArg(arg)
    {}

    // コピーコンストラクタ
    TbBindImpl( const TbBindImpl& src )
        : m_Func( src.m_Func )
        , m_BindArg( src.m_BindArg )
    {}

    // デストラクタ
    ~TbBindImpl() {}

public:

    // 呼び出し(引数0)
    R operator()() {
        typedef TbBindList0 List;
        List argList;
#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
        return m_BindArg.operator()<R,F,List>( m_Func , argList );
#else
        return m_BindArg.template operator()<R,F,List>( m_Func , argList );
#endif
    }

    // 呼び出し(引数1)
    template <typename T1>
    R operator()(T1 t1) {
        typedef TbBindList1<T1> List;
        List argList(t1);
#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
        return m_BindArg.operator()<R,F,List>( m_Func , argList );
#else
        return m_BindArg.template operator()<R,F,List>( m_Func , argList );
#endif
    }

    // 呼び出し(引数2)
    template <typename T1,typename T2>
    R operator()(T1 t1,T2 t2) {
        typedef TbBindList2<T1,T2> List;
        List argList(t1,t2);
#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
        return m_BindArg.operator()<R,F,List>( m_Func , argList );
#else
        return m_BindArg.template operator()<R,F,List>( m_Func , argList );
#endif
    }

    // 呼び出し(引数3)
    template <typename T1,typename T2,typename T3>
    R operator()(T1 t1,T2 t2,T3 t3) {
        typedef TbBindList3<T1,T2,T3> List;
        List argList(t1,t2,t3);
#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
        return m_BindArg.operator()<R,F,List>( m_Func , argList );
#else
        return m_BindArg.template operator()<R,F,List>( m_Func , argList );
#endif
    }

    // 呼び出し(引数4)
    template <typename T1,typename T2,typename T3,typename T4>
    R operator()(T1 t1,T2 t2,T3 t3,T4 t4) {
        typedef TbBindList4<T1,T2,T3,T4> List;
        List argList(t1,t2,t3,t4);
#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
        return m_BindArg.operator()<R,F,List>( m_Func , argList );
#else
        return m_BindArg.template operator()<R,F,List>( m_Func , argList );
#endif
    }


    // 呼び出し(引数5)
    template <typename T1,typename T2,typename T3,typename T4,typename T5>
    R operator()(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5) {
        typedef TbBindList5<T1,T2,T3,T4,T5> List;
        List argList(t1,t2,t3,t4,t5);
#if TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN
        return m_BindArg.operator()<R,F,List>( m_Func , argList );
#else
        return m_BindArg.template operator()<R,F,List>( m_Func , argList );
#endif
    }

private:
    
    F   m_Func;     // 関数
    A   m_BindArg;  // バインドした引数
    
};

}

#endif