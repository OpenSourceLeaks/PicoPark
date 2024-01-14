/*!
* Bind用引数リスト(引数6)
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#ifndef _INCLUDED_TB_BIND_LIST6_H_
#define _INCLUDED_TB_BIND_LIST6_H_

namespace toybox
{

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
class TbBindList6
{
public:
    
    // コンストラクタ
    TbBindList6( T1& arg1 , T2& arg2 , T3& arg3 , T4& arg4 , T5& arg5 , T6& arg6 ) 
        : m_Arg1(arg1)
        , m_Arg2(arg2)
        , m_Arg3(arg3)
        , m_Arg4(arg4)
        , m_Arg5(arg5)
        , m_Arg6(arg6)
    {}
    
    // デストラクタ
    ~TbBindList6() {}
    
public:
    
    // 関数呼び出し
    template<typename R,typename F,typename A>
    R operator()( F& func , A& arg ){
        return func( arg.GetArg( m_Arg1 ) , 
                     arg.GetArg( m_Arg2 ) , 
                     arg.GetArg( m_Arg3 ) , 
                     arg.GetArg( m_Arg4 ) ,
                     arg.GetArg( m_Arg5 ) ,
                     arg.GetArg( m_Arg6 ) );
    }
    
    
    //! 引数取得(第1引数)
    T1& GetArg( TbBindPlaceholder<1>& ) {
        return m_Arg1;
    }

    //! 引数取得(第2引数)
    T2& GetArg( TbBindPlaceholder<2>& ) {
        return m_Arg2;
    }

    //! 引数取得(第3引数)
    T3& GetArg( TbBindPlaceholder<3>& ) {
        return m_Arg3;
    }

    //! 引数取得(第4引数)
    T4& GetArg( TbBindPlaceholder<4>& ) {
        return m_Arg4;
    }

    //! 引数取得(第5引数)
    T5& GetArg( TbBindPlaceholder<5>& ) {
        return m_Arg5;
    }

    //! 引数取得(第1引数)
    T6& GetArg( TbBindPlaceholder<6>& ) {
        return m_Arg6;
    }

    //! 引数取得(バインド値)
    template<typename T>
    T& GetArg( T& arg ){
        return arg;
    }

private:
    T1 m_Arg1; // 第2引数
    T2 m_Arg2; // 第3引数
    T3 m_Arg3; // 第4引数
    T4 m_Arg4; // 第5引数
    T5 m_Arg5; // 第6引数
    T6 m_Arg6; // 第1引数
};

}

#endif