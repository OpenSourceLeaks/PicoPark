/*!
 * Bindの実装部分
 * @author Miyake Shunsuke
 * @since 2012.03.11
 */

#define TB_BIND_MEM_FUNC TB_PP_CAT(TbBindMemFunc,TB_BIND_MEM_FUNC_ARGS_MAX)
#define TB_BIND_MEM_FUNC_CONST TB_PP_CAT(TbBindMemFuncConst,TB_BIND_MEM_FUNC_ARGS_MAX)

// 引数一つのメンバー関数
template<typename R,typename Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_DECL_TEMPLATE_ARGS>
class TB_BIND_MEM_FUNC
{
public:
    typedef R (Class::*FuncType)(TB_BIND_MEM_FUNC_DECL_ARGS);
public:

    // コンストラクタ
    TB_BIND_MEM_FUNC( Class* instance , FuncType func )
        : m_Instance( instance )
        , m_MemberFunc( func )
    {}

    // デストラクタ
    ~TB_BIND_MEM_FUNC() {}

    // コピーコンストラクタ
    TB_BIND_MEM_FUNC( const TB_BIND_MEM_FUNC& src )
        : m_Instance( src.m_Instance )
        , m_MemberFunc( src.m_MemberFunc )
    {}

public:

    // 代入演算子
    TB_BIND_MEM_FUNC& operator=( TB_BIND_MEM_FUNC& src ) {
        m_Instance = src.m_Instance;
        m_MemberFunc = src.m_MemberFunc;
    }

    // 関数演算子
    R operator() ( TB_BIND_MEM_FUNC_DECL_ARGS ) {
        return (m_Instance->*m_MemberFunc)( TB_BIND_MEM_FUNC_ARGS );
    }

private:

    Class*      m_Instance;     // インスタンス
    FuncType    m_MemberFunc;   // 関数

};

// 引数一つのメンバー関数
template<typename R,typename Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_DECL_TEMPLATE_ARGS>
class TB_BIND_MEM_FUNC_CONST
{
public:
    typedef R (Class::*FuncType)(TB_BIND_MEM_FUNC_DECL_ARGS) const;
public:

    // コンストラクタ
    TB_BIND_MEM_FUNC_CONST( const Class* instance , FuncType func )
        : m_Instance( instance )
        , m_MemberFunc( func )
    {}

    // デストラクタ
    ~TB_BIND_MEM_FUNC_CONST() {}

    // コピーコンストラクタ
    TB_BIND_MEM_FUNC_CONST( const TB_BIND_MEM_FUNC_CONST& src )
        : m_Instance( src.m_Instance )
        , m_MemberFunc( src.m_MemberFunc )
    {}

public:

    // 代入演算子
    TB_BIND_MEM_FUNC_CONST& operator=( TB_BIND_MEM_FUNC_CONST& src ) {
        m_Instance = src.m_Instance;
        m_MemberFunc = src.m_MemberFunc;
    }

    // 関数演算子
    R operator() ( TB_BIND_MEM_FUNC_DECL_ARGS ) {
        return (m_Instance->*m_MemberFunc)( TB_BIND_MEM_FUNC_ARGS );
    }

private:

    const Class*    m_Instance;     // インスタンス
    FuncType        m_MemberFunc;   // 関数

};

template<typename R,typename Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_DECL_TEMPLATE_ARGS>
TB_BIND_MEM_FUNC<R,Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_TEMPLATE_ARGS>
    TbCreateMemFunc(Class* instance,R (Class::*memberFunc)(TB_BIND_MEM_FUNC_DECL_ARGS))
{
    return TB_BIND_MEM_FUNC<R,Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_TEMPLATE_ARGS>(instance,memberFunc);
}

template<typename R,typename Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_DECL_TEMPLATE_ARGS>
TB_BIND_MEM_FUNC_CONST<R,Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_TEMPLATE_ARGS>
    TbCreateMemFunc(const Class* instance,R (Class::*memberFunc)(TB_BIND_MEM_FUNC_DECL_ARGS) const)
{
    return TB_BIND_MEM_FUNC_CONST<R,Class TB_BIND_MEM_FUNC_COMMA TB_BIND_MEM_FUNC_TEMPLATE_ARGS>(instance,memberFunc);
}

#undef TB_BIND_MEM_FUNC
#undef TB_BIND_MEM_FUNC_CONST