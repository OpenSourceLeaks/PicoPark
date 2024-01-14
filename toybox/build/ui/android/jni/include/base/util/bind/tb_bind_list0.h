/*!
* BInd用引数リスト(引数0)
 * @author Miyake_Shunsuke
 * @since 2012.04.24
 */

#ifndef _INCLUDED_TB_BIND_LIST0_H_
#define _INCLUDED_TB_BIND_LIST0_H_

namespace toybox
{

class TbBindList0
{
public:
    
    // コンストラクタ
    TbBindList0() 
    {}
    
    // デストラクタ
    ~TbBindList0() {}
    
public:
    
    // 関数呼び出し
    template<typename R,typename F,typename A>
    R operator()( F& func , A& arg ){
        return func();
    }
    
    //! 引数取得(バインド値)
    template<typename T>
    T& GetArg( T& arg ){
        return arg;
    }

private:
};

}

#endif