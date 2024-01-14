/*!
 * 引数無しのFunctionテンプレート
 * BoostのFunctionテンプレートと同じ機能を持つ
 * @author Miyake Shunsuke
 */

namespace toybox
{

#define TB_STATIC_FUNCTION TB_PP_CAT(TbStaticFunction,TB_STATIC_FUNCTION_BUF_SIZE)

template <typename F>
class TB_STATIC_FUNCTION;

/*!
 * 引数無しのFunctionテンプレートクラス
 */
template <typename R TB_FUNCTION_TEMPLATE_ARGS>
class TB_STATIC_FUNCTION<R (TB_FUNCTION_DECL_ARGS)>
{
public:

    static const TbUint32 BUF_SIZE = TB_STATIC_FUNCTION_BUF_SIZE;
     typedef void (*CopyFuncType)( TbUint8 (&dst)[BUF_SIZE] , const TbUint8 (&src)[BUF_SIZE] );

public:

    /*!
     * コンストラクタ
     */
    TB_STATIC_FUNCTION()
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc( NULL )
    {
        TbMemClear(m_FunctorBuf,sizeof(m_FunctorBuf));
    }

    /*!
     * コンストラクタ
     */
    template<typename F>
    TB_STATIC_FUNCTION( const F& func )
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc( NULL )
    {
        TbMemClear(m_FunctorBuf,sizeof(m_FunctorBuf));
        (*this) = func;
    }

    /*!
     * コンストラクタ
     */
    TB_STATIC_FUNCTION( R (*func)(TB_FUNCTION_DECL_ARGS) )
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc( NULL )
    {
        TbMemClear(m_FunctorBuf,sizeof(m_FunctorBuf));
        (*this) = func;
    }

    /*!
     * コピーコンストラクタ
     */
    TB_STATIC_FUNCTION( const TB_STATIC_FUNCTION& src )
        : m_Invoke( NULL )
        , m_Destructor( NULL )
        , m_CopyFunc( NULL )
    {
        TbMemClear(m_FunctorBuf,sizeof(m_FunctorBuf));
        if( src.IsValid() ){
            (*this) = src;
        }
    }

    //デストラクタ
    ~TB_STATIC_FUNCTION(){
        Clear();
    }

    /*!
     * 代入演算子(関数ポインタ)
     */
    TB_STATIC_FUNCTION& operator=(R (*func)(TB_FUNCTION_DECL_ARGS))
    {
        typedef R (*FuncType)(TB_FUNCTION_DECL_ARGS);
        //行使者
        struct Invoker{
            static R Invoke( TbUint8 *buf TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS )
            {
                FuncType* func = reinterpret_cast<FuncType*>(buf); // void(*)()になってる関数ポインタを本来の型にキャスト
                return (*func)(TB_FUNCTION_INVOKE_ARGS);
            }
        };
        TB_STATIC_ASSERT( sizeof(m_FunctorBuf) >= sizeof(func) );
        // 関数ポインタを実行する関数を登録
        m_Invoke = &Invoker::Invoke;
        TbMemCopy( m_FunctorBuf , &func , sizeof(func) );
        return *this;
    }

    /*!
     * 代入演算子(関数オブジェクト)
     */ 
    template<typename C>
    TB_STATIC_FUNCTION& operator=(const C& func)
    {
        //行使者
        struct Invoker{
            //関数実行
            static R Invoke( TbUint8 *buf TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS )
            {
                C* func = reinterpret_cast<C*>(buf); // void*になってる関数オブジェクトを本来の型にキャスト
                return (*func)( TB_FUNCTION_INVOKE_ARGS );
            }
            //関数オブジェクト破棄
            static void Delete( TbUint8 (&buf)[BUF_SIZE] )
            {
                C* func = reinterpret_cast<C*>(buf);
                func->~C();
            }
            //関数オブジェクト破棄
            static void Copy( TbUint8 (&dst)[BUF_SIZE] , const TbUint8 (&src)[BUF_SIZE] )
            {
                const C* func = reinterpret_cast<const C*>(src);
                new(dst) C(*func);
            }
        };
        TB_STATIC_ASSERT( sizeof(m_FunctorBuf) >= sizeof(func) );

        // 関数オブジェクトを実行する関数を登録
        m_Invoke = &Invoker::Invoke;
        m_Destructor = &Invoker::Delete;
        m_CopyFunc = &Invoker::Copy;

        // (型削除した)関数オブジェクトを保持
        // placement new で m_FunctorBuf にコピー
        new(m_FunctorBuf) C(func);
        
        return *this;
    }
    
    /*!
     * 代入演算子
     */
    TB_STATIC_FUNCTION& operator=(const TB_STATIC_FUNCTION& func)
    {
        m_Invoke = func.m_Invoke;
        m_Destructor = func.m_Destructor;
        m_CopyFunc = func.m_CopyFunc;
        if( m_CopyFunc ){
            m_CopyFunc( m_FunctorBuf , func.m_FunctorBuf );
        }else{
            TbMemCopy( m_FunctorBuf , func.m_FunctorBuf , sizeof(m_FunctorBuf) );
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
            m_Destructor(m_FunctorBuf);
        }
        m_Invoke = NULL;
        m_Destructor = NULL;
        m_CopyFunc = NULL;
    }


    /*!
     * 関数ポインタ・関数オブジェクトの実行
     */
    TB_FORCE_INLINE R operator()(TB_FUNCTION_DECL_ARGS) const
    {
        return m_Invoke(const_cast<TbUint8*>(m_FunctorBuf) TB_FUNCTION_COMMA TB_FUNCTION_ARGS );
    }

    /*!
     * コールバック関数が設定されてるかどうか
     */
    bool IsValid() const {
        return (m_Invoke != NULL);
    }

private:

    R (*m_Invoke)( TbUint8 *buf TB_FUNCTION_COMMA TB_FUNCTION_DECL_ARGS );  // 関数ポインタ・関数オブジェクトの実行関数
    void (*m_Destructor)( TbUint8 (&buf)[BUF_SIZE] );  // 関数オブジェクトの破棄用関数
    CopyFuncType m_CopyFunc;
    TbUint8      m_FunctorBuf[BUF_SIZE];        // 関数ポインタ・関数オブジェクトをコピーするためのバッファ

};

}

#undef TB_STATIC_FUNCTION