/*!
 * 非同期関数
 * 複数フレームをまたがって処理をする関数をサポートします。
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */

#include "tb_fwd.h"
#include "base/util/tb_async_func.h"
#include "base/system/tb_assert.h"

#include "base/util/tb_enum.h"

namespace toybox
{

/*
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.08.02
 */
TbAsyncFunc::TbAsyncFunc()
    : m_BitArray(0)
    , m_FuncInfo( NULL )
    , m_FuncCount( 0 )
    , m_CallStack( 0 )
    , m_CurrentFuncIndex( 0 )
    , m_CurrentLocalPhase( -1 )
{
    TbMemClear( m_EventCallBack , sizeof(m_EventCallBack) );
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbAsyncFunc::TbAsyncFunc( const FuncType* func , const TbUint32 funcCount , const TbUint32 stackSize )
    : m_BitArray(0)
    , m_FuncInfo( NULL )
    , m_FuncCount( 0 )
    , m_CallStack( 0 )
    , m_CurrentFuncIndex( 0 )
    , m_CurrentLocalPhase( -1 )
{
    TbMemClear( m_EventCallBack , sizeof(m_EventCallBack) );
    Initilaize( func , funcCount , stackSize );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbAsyncFunc::~TbAsyncFunc()
{
    TB_SAFE_DELETE_ARRAY( m_FuncInfo );
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2011.08.02
 */
TbResult TbAsyncFunc::Initilaize( const FuncType* func , const TbUint32 funcCount , const TbUint32 stackSize )
{
    TB_SAFE_DELETE_ARRAY( m_FuncInfo );
    m_FuncCount = funcCount;
    m_CallStack.SetCapacity( stackSize / sizeof(CallInfo) );
    m_FuncInfo = TB_NEW Self::FuncInfo[ m_FuncCount ];
    for( TbUint32 i = 0; i < m_FuncCount; ++i ){
        m_FuncInfo[i].func = func[i];
    }
    return TB_S_OK;
}

/*!
 * 更新する
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbResult TbAsyncFunc::Update()
{
    TB_ASSERT( m_CurrentFuncIndex < m_FuncCount );
    if( IsBusy() ){
        Self::FuncType& func = m_FuncInfo[ m_CurrentFuncIndex ].func;
        m_BitArray.SetBit( FLAG_DISABLE_CALL , TB_FALSE );

        if( func.IsValid() && Self::RETURN_YIELD != func( *this ) ){
            TbSint32 callStackCount = m_CallStack.GetCount();
            if( callStackCount > 0 ){
                // コールスタックが残っていたら復帰する。
                CallInfo& callInfo = m_CallStack[ callStackCount-1 ];
                m_CurrentFuncIndex = callInfo.funcIndex;
                m_CurrentLocalPhase = callInfo.localPhase;
                m_CallStack.PopBack();
                m_BitArray.SetBit( FLAG_DISABLE_CALL , TB_TRUE );
                // イベントコールバック
                if( m_EventCallBack[ EVENT_TYPE_FUNC_END ].IsValid() ){
                    m_EventCallBack[ EVENT_TYPE_FUNC_END ]( *this , m_CurrentFuncIndex );
                }
            }else{
                // 終了
                m_BitArray.SetBit( FLAG_BUSY , TB_FALSE );
                // イベントコールバック
                if( m_EventCallBack[ EVENT_TYPE_FUNC_END ].IsValid() ){
                    m_EventCallBack[ EVENT_TYPE_FUNC_END ]( *this , m_CurrentFuncIndex );
                }
            }
        }else{
            // 中断
            m_BitArray.SetBit( FLAG_DISABLE_CALL , TB_TRUE );
        }

    }
    return TB_S_OK;
}

/*!
 * 現在呼び出し中
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbBool TbAsyncFunc::IsBusy() const
{
    return m_BitArray.Test( FLAG_BUSY );
}

/*!
 * 関数呼び出し
 * @param funcIndex 登録している関数のインデックス番号
 * @retval TB_S_OK 成功
 * @retval TB_E_FAIL 失敗
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbResult TbAsyncFunc::Call( const TbUint32 funcIndex )
{
    // 呼び出し不可能
    if( IsBusy() && m_BitArray.Test( FLAG_DISABLE_CALL ) ){
        return TB_E_FAIL;
    }
    // 不正な引数
    if( funcIndex >= m_FuncCount ){
        return TB_E_FAIL;
    }
    // 呼び出し中関数があればコールスタックに追加
    if( IsBusy() ){
        Self::CallInfo callInfo = { m_CurrentFuncIndex , m_CurrentLocalPhase };
        if( !m_CallStack.PushBack( callInfo ) ){
            // コールスタックの容量が一杯
            return TB_E_FAIL;
        }
    }
    m_CurrentFuncIndex = funcIndex;
    m_CurrentLocalPhase = 0;
    m_BitArray.SetBit( FLAG_BUSY );
    m_BitArray.SetBit( FLAG_DISABLE_CALL );
    return TB_S_OK;
}

/*!
 * 現在呼ばれている関数のみで共有されるフェーズ
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbResult TbAsyncFunc::SetPhase( const TbSint32 phase )
{
    if( IsBusy() ){
        m_CurrentLocalPhase = phase;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 現在呼ばれている関数のみで共有されるフェーズ
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
TbSint32 TbAsyncFunc::GetPhase() const
{
    if( IsBusy() ){
        return m_CurrentLocalPhase;
    }
    return -1;
}

/*!
 * イベントコールバック設定
 * @author Miyake Shunsuke
 * @since 2011.08.02
 */
TbResult TbAsyncFunc::SetEventCallBack( const EventType type , const EventCallbackType& callback )
{
    if( TB_VERIFY( 0 <= type && type < EVENT_TYPE_MAX ) ){
        m_EventCallBack[ type ] = callback;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

}
