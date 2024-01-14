/*!
* BInd用引数リスト(引数1)
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#ifndef _INCLUDED_TB_BIND_LIST1_H_
#define _INCLUDED_TB_BIND_LIST1_H_

namespace toybox
{

template<typename T1>
class TbBindList1
{
public:
    
    // コンストラクタ
    TbBindList1( T1& arg1 ) 
        : m_Arg1(arg1)
    {}
    
    // デストラクタ
    ~TbBindList1() {}
    
public:
    
    // 関数呼び出し
    template<typename R,typename F,typename A>
    R operator()( F& func , A& arg ){
        return func( arg.GetArg( m_Arg1 ) );
    }
    
public:
    
    //! 引数取得(第1引数)
    T1 GetArg( TbBindPlaceholder<1>& ) {
        return m_Arg1;
    }
        
    //! 引数取得(バインド値)
    template<typename T>
    T& GetArg( T& arg ){
        return arg;
    }
    
private:
    T1 m_Arg1; // 第一引数
};

}

#endif