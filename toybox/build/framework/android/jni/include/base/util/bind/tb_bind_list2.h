/*!
* Bind用引数リスト(引数2)
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#ifndef _INCLUDED_TB_BIND_LIST2_H_
#define _INCLUDED_TB_BIND_LIST2_H_

namespace toybox
{

template<typename T1,typename T2>
class TbBindList2
{
public:
    
    // コンストラクタ
    TbBindList2( T1& arg1 , T2& arg2 ) 
        : m_Arg1(arg1)
        , m_Arg2(arg2)
    {}
    
    // デストラクタ
    ~TbBindList2() {}

public:
    
    // 関数呼び出し
    template<typename R,typename F,typename A>
    R operator()( F& func , A& arg ){
        return func( arg.GetArg( m_Arg1 ) , arg.GetArg( m_Arg2 ) );
    }
    
public:
    
    //! 引数取得(第1引数)
    T1& GetArg( TbBindPlaceholder<1>& ) {
        return m_Arg1;
    }
    
    //! 引数取得(第2引数)
    T2& GetArg( TbBindPlaceholder<2>& ) {
        return m_Arg2;
    }
    
    //! 引数取得(バインド値)
    template<typename T>
    T& GetArg( T& arg ){
        return arg;
    }
    
private:
    T1 m_Arg1; // 第一引数
    T2 m_Arg2; // 第二引数
};

}

#endif