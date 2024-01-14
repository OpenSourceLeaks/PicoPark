/*!
 * @file
 * @brief スコープから抜け出す時に自動で関数を呼ぶクラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SCOPE_FINALIZER_H_
#define _INCLUDED_TB_SCOPE_FINALIZER_H_

namespace toybox
{

class TbScopeFinalizer
{
public:
    
    // コンストラクタ
    template< typename R >
    TbScopeFinalizer( R (*func)() )
        : m_CallFunc( NULL )
        , m_FuncPtr( NULL )
    {
        struct Invoke{
            typedef R (*FuncType)();
            static void Call( void* funcPtr ){
                reinterpret_cast<FuncType>(funcPtr)();
            };
        };
        // 型消滅させる
        m_FuncPtr = reinterpret_cast<void*>(func);
        m_CallFunc = Invoke::Call;
    }
    
    // デストラクタ
    ~TbScopeFinalizer()
    {
        if( m_FuncPtr ){
            m_CallFunc( m_FuncPtr );
        }
    }
private:
    void (*m_CallFunc)( void* funcPtr );
    void* m_FuncPtr;
};

}

#endif