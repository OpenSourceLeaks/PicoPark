/*!
 * 引数無しのメンバ関数用関数オブジェクト
 * ヒープ確保しないバージョン
 * @author Miyake Shunsuke
 */

namespace toybox
{

template<typename F>
class TbMemberFunction;

template<typename R TB_FUNCTION_TEMPLATE_ARGS>
class TbMemberFunction<R (TB_FUNCTION_DECL_ARGS)>
{
public:
    typedef R ResultType;
    static const TbUint32 MEMBER_FUNCTION_PTR_SIZE_MAX = 12;
public:
    
    //! コンストラクタ
    TbMemberFunction()
        : m_Invoke(NULL)
    {
        TbMemClear( &m_CallInfo , sizeof(m_CallInfo) );
    }
    
    //! コンストラクタ
    template<typename C>
    TbMemberFunction( C* instance , R (C::*memberFunc)(TB_FUNCTION_DECL_ARGS) )
        : m_Invoke(NULL)
    {
        TbMemClear( &m_CallInfo , sizeof(m_CallInfo) );
        Set( instance , memberFunc );
    }

    //! コンストラクタ
    template<typename C>
    TbMemberFunction( const C* instance , R (C::*memberFunc)(TB_FUNCTION_DECL_ARGS) const )
        : m_Invoke(NULL)
    {
        TbMemClear( &m_CallInfo , sizeof(m_CallInfo) );
        Set( instance , memberFunc );
    }

    //! コンストラクタ
    TbMemberFunction( const TbMemberFunction& func )
        : m_Invoke(func.m_Invoke)
    {
        TbMemCopy( &m_CallInfo , &func.m_CallInfo , sizeof(m_CallInfo) );
    }

    //! デストラクタ
    ~TbMemberFunction()
    {
    }

public:
    
    // 関数呼び出し
    R operator()( TB_FUNCTION_DECL_ARGS ) const
    {
        return m_Invoke(&m_CallInfo TB_FUNCTION_COMMA TB_FUNCTION_ARGS );
    }

    // インスタンスとメンバ関数登録
    template<typename C>
    void Set( C* instance , R (C::*memberFunc)( TB_FUNCTION_DECL_ARGS ) )
    {
        //行使者
        struct Invoker{
            typedef R (C::*MemberFuncType)( TB_FUNCTION_DECL_ARGS );
            //関数実行
            static R Invoke( const CallInfo* callInfo TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS )
            {
                C* caller = reinterpret_cast<C*>(callInfo->instance);
                const MemberFuncType* func = reinterpret_cast<const MemberFuncType*>(callInfo->func);
                return (caller->**func)( TB_FUNCTION_ARGS );
            }
        };
        // 関数オブジェクトを実行する関数を登録
        m_Invoke = &Invoker::Invoke;
        // 型を消失させる
        m_CallInfo.instance = reinterpret_cast<void*>(instance);
        // メンバ関数ポインタを保存(4バイト以上にはならないと思うが)
        TB_STATIC_ASSERT( sizeof(m_CallInfo.func) >= sizeof(memberFunc) );
        TbMemCopy( m_CallInfo.func , &memberFunc , sizeof(memberFunc) );
    }
    
    // インスタンスとメンバ関数登録
    template<typename C>
    void Set( const C* instance , R (C::*memberFunc)( TB_FUNCTION_DECL_ARGS ) const )
    {
        (instance->*memberFunc)();
        //行使者
        struct Invoker{
            typedef R (C::*MemberFuncType)( TB_FUNCTION_DECL_ARGS );
            //関数実行
            static R Invoke( const CallInfo* callInfo TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS )
            {
                C* caller = reinterpret_cast<C*>(callInfo->instance);
                const MemberFuncType* func = reinterpret_cast<const MemberFuncType*>(callInfo->func);
                return (caller->**func)( TB_FUNCTION_ARGS );
            }
        };
        // 関数オブジェクトを実行する関数を登録
        m_Invoke = &Invoker::Invoke;
        // 型を消失させる
        m_CallInfo.instance = reinterpret_cast<void*>(const_cast<C*>(instance));
        // メンバ関数ポインタを保存(4バイト以上にはならないと思うが)
        TB_STATIC_ASSERT( sizeof(m_CallInfo.func) >= sizeof(memberFunc) );
        TbMemCopy( m_CallInfo.func , &memberFunc , sizeof(memberFunc) );
    }

private:

    struct CallInfo{
        void* instance;
        TbUint8 func[MEMBER_FUNCTION_PTR_SIZE_MAX]; // メンバ関数ポインタ用バッファ
    };

private:

    R (*m_Invoke)( const CallInfo* callInfo TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS );
    CallInfo m_CallInfo;

};

template<typename R,typename C TB_FUNCTION_TEMPLATE_ARGS>
inline TbMemberFunction<R (TB_FUNCTION_DECL_ARGS)> TbCreateMemFunc(C* instance,R (C::*memberFunc)(TB_FUNCTION_DECL_ARGS))
{
    return TbMemberFunction<R (TB_FUNCTION_DECL_ARGS)>(instance,memberFunc);
}

template<typename R,typename C TB_FUNCTION_TEMPLATE_ARGS>
inline TbMemberFunction<R (TB_FUNCTION_DECL_ARGS)> TbCreateMemFunc( const C* instance,R (C::*memberFunc)(TB_FUNCTION_DECL_ARGS) const)
{
    return TbMemberFunction<R (TB_FUNCTION_DECL_ARGS)>(instance,memberFunc);
}

}

