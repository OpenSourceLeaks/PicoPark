/*!
 * 引数無しのFunctionテンプレート
 * BoostのFunctionテンプレートと同じ機能を持つ
 * @author Miyake Shunsuke
 */

namespace toybox
{

// 先行宣言
template<typename F>
class TbFunction;

/*!
 * 引数無しのFunctionテンプレートクラス
 */
template <typename R TB_FUNCTION_TEMPLATE_ARGS >
class TbFunction< R (TB_FUNCTION_DECL_ARGS) >
{
public:
    typedef TbFunction< R (TB_FUNCTION_DECL_ARGS) > Self;
private:

    //関数ポインタと関数オブジェクトの共有
    union FuncPtrObj {
        void (*funcStatic)(); // 関数ポインタ
        void *funcObject;     // 関数オブジェクト
    };

public:

    /*!
     * コンストラクタ
     */
    TbFunction()
        : m_Invoke(NULL)
        , m_Destructor(NULL)
        , m_CopyFunc(NULL)
    {
        m_Functor.funcStatic = NULL;
        m_Functor.funcObject = NULL;
    }

    /*!
     * コンストラクタ(関数ポインタ)
     */
    TbFunction( R (*func)(TB_FUNCTION_DECL_ARGS) )
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc(NULL)
    {
        m_Functor.funcStatic = NULL;
        m_Functor.funcObject = NULL;
        (*this) = func;
    }

    /*!
     * コンストラクタ(関数オブジェクト)
     */
    template<typename F>
    TbFunction( const F& func )
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc(NULL)
    {
        m_Functor.funcStatic = NULL;
        m_Functor.funcObject = NULL;
        (*this) = func;
    }

    /*!
     * コピーコンストラクタ
     */
    TbFunction( const Self& src )
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc(NULL)
    {
        m_Functor.funcStatic = NULL;
        (*this) = src;
    }

    //デストラクタ
    ~TbFunction(){
        Clear();
    }

    /*!
     * 代入演算子(関数ポインタ)
     */
    TbFunction& operator=(R (*func)(TB_FUNCTION_DECL_ARGS))
    {
        //行使者
        struct Invoker{
            typedef R (*FuncType)(TB_FUNCTION_DECL_ARGS);
            static R Invoke( FuncPtrObj &funcPtr TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS )
            {
                FuncType f = reinterpret_cast<FuncType>(funcPtr.funcStatic); // void(*)()になってる関数ポインタを本来の型にキャスト
                return f(TB_FUNCTION_ARGS);
            }
        };

        //関数オブジェクトの破棄
        if(m_Destructor){
            m_Destructor(m_Functor);
            m_Functor.funcObject = NULL;
            m_Destructor = NULL;
        }

        // 関数ポインタを実行する関数を登録
        m_Invoke = &Invoker::Invoke;

        // (型削除した)関数ポインタを保持
        m_Functor.funcStatic = reinterpret_cast<void(*)()>(func);
        return *this;
    }

    /*!
     * 代入演算子(関数オブジェクト)
     */
    template<typename F>
    TbFunction& operator=(const F &func)
    {
        //行使者
        struct Invoker{
            //関数実行
            static R Invoke(FuncPtrObj &funcObj TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS )
            {
                F* func = reinterpret_cast<F*>(funcObj.funcObject); // void*になってる関数オブジェクトを本来の型にキャスト
                 return (*func)(TB_FUNCTION_ARGS);
            }
            //関数オブジェクト破棄
            static void Delete(FuncPtrObj &funcObj)
            {
                if(funcObj.funcObject){
                    F* func = reinterpret_cast<F*>(funcObj.funcObject);
                    TB_DELETE(func);
                }
            }
            //関数オブジェクト破棄
            static void Copy(FuncPtrObj &dst , const FuncPtrObj &src)
            {
                if(src.funcObject){
                    const F* srcFunc = reinterpret_cast<const F*>(src.funcObject);
                    dst.funcObject = reinterpret_cast<void*>(TB_NEW F(*srcFunc));
                }
            }
        };
        // 関数オブジェクトを実行する関数を登録
        m_Invoke = &Invoker::Invoke;
        m_Destructor = &Invoker::Delete;
        m_CopyFunc = &Invoker::Copy;
        // (型削除した)関数オブジェクトを保持
        m_Functor.funcObject = reinterpret_cast<void*>(TB_NEW F(func));
        
        return *this;
    }

    /*!
     * 代入演算子
     */
    TbFunction& operator=(const Self& func)
    {
        // 破棄
        Clear();
        m_Invoke = func.m_Invoke;
        m_Destructor = func.m_Destructor;
        m_CopyFunc = func.m_CopyFunc;
        if( m_CopyFunc ){
            m_CopyFunc( m_Functor , func.m_Functor );
        }else{
            m_Functor.funcStatic = func.m_Functor.funcStatic;
        }
        return *this;
    }

    /*!
     * 設定解除
     */
    void Clear()
    {
        //関数オブジェクトの破棄
        if(m_Destructor){
            m_Destructor(m_Functor);
            m_Functor.funcStatic = NULL;
        }
        m_Invoke = NULL;
        m_Destructor = NULL;
        m_CopyFunc = NULL;
    }

    /*!
     * 関数ポインタ・関数オブジェクトの実行
     */
    R operator()(TB_FUNCTION_DECL_ARGS)
    {
        return m_Invoke(m_Functor TB_FUNCTION_COMMA TB_FUNCTION_ARGS );
    }
    
    /*!
     * コールバック関数が設定されてるかどうか
     */
    bool IsValid(){return (m_Invoke != NULL);}

private:
    R (*m_Invoke)(FuncPtrObj& TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS );         // 関数ポインタ・関数オブジェクトの実行関数
    void (*m_Destructor)(FuncPtrObj&);  // 関数オブジェクトの破棄用関数
    void (*m_CopyFunc)(FuncPtrObj&,const FuncPtrObj&);  // コピー用関数
    FuncPtrObj m_Functor;               // 関数ポインタ・関数オブジェクト	
};

}
