/*!
 * 非同期関数
 * 複数フレームをまたがって処理をする関数をサポートします。
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */

#ifndef _INCLUDED_TB_ASYNC_FUNC_H_
#define _INCLUDED_TB_ASYNC_FUNC_H_

#include <base/container/tb_array.h>

namespace toybox
{

/*!
 * 非同期関数
 * @author Miyake Shunsuke
 * @since 2011.06.26
 */
class TbAsyncFunc
{
public:

    // 自分自身を指す
    typedef TbAsyncFunc Self;

    // YIELD
    static const TbSint32 RETURN_YIELD = 0;
    static const TbSint32 RETURN_FINISH = 1;

    // デフォルトスタックサイズ
    static const TbUint32 STACK_SIZE_DEFAULT = 10;

    // 呼び出す関数型
    typedef TbStaticFunction32<TbSint32 (TbAsyncFunc&)> FuncType;
    typedef TbStaticFunction32<TbSint32 (TbAsyncFunc&,const TbUint32 funcIndex)> EventCallbackType;

    // イベントタイプ
    enum EventType
    {
        EVENT_TYPE_FUNC_END , 
        EVENT_TYPE_MAX , 
    };

public:

    // コンストラクタ
    TbAsyncFunc();

    // コンストラクタ
    TbAsyncFunc( const FuncType* func , const TbUint32 funcCount , const TbUint32 stackSize );
    
    // デストラクタ
    ~TbAsyncFunc();

public:

    // 初期化
    TbResult Initilaize( const FuncType* func , const TbUint32 funcCount , const TbUint32 stackSize );

    // 更新する
    TbResult Update();
        
public:
    
    // 関数呼び出し
    TbResult Call( TbUint32 funcIndex );

    // 現在呼び出し中
    TbBool IsBusy() const;

public:
    
    // 現在呼ばれている関数のみで使用されるフェーズ
    TbResult SetPhase( TbSint32 phase );
    
    // 現在呼ばれている関数のみで使用されるフェーズ
    TbResult AddPhase( TbSint32 phase ) {
        return SetPhase( GetPhase()+phase );
    }

    // 現在呼ばれている関数のみで使用されるフェーズ
    TbSint32 GetPhase() const;
    
    //! 登録されている関数の数取得
    TbUint32 GetFuncCount() const { 
        return m_FuncCount; 
    }
    
    //! 利用可能なコールスタック数
    TbUint32 GetCallStackMax() const { 
        return m_CallStack.GetCapacity(); 
    }
    
    //! 現在利用中のコールスタック数
    TbUint32 GetCallStackCount() const { 
        return m_CallStack.GetCount(); 
    }

    // イベントコールバック設定
    TbResult SetEventCallBack( const EventType type , const EventCallbackType& callback );

private:

    enum {
        FLAG_BUSY , // 呼び出し中
        FLAG_DISABLE_CALL , // コール呼び出し可能
    };

    // 関数情報
    struct FuncInfo
    {
        FuncInfo() : func() , funcPhase( 0 ) {}
        ~FuncInfo() {}
        FuncType func;
        TbSint32 funcPhase;
    };

    // 呼び出し情報
    struct CallInfo
    {
        TbUint32 funcIndex;  // 復帰する関数インデックス
        TbSint32 localPhase; // 復帰する時のローカルフェーズ
    };

public:

    TbBitArray32       m_BitArray;                              // ビット配列
    FuncInfo*          m_FuncInfo;                              // 関数情報
    TbUint32           m_FuncCount;                             // 関数の数
    TbArray<CallInfo>  m_CallStack;                             // コールスタック
    TbUint32           m_CurrentFuncIndex;                      // 現在呼び出し中関数インデックス
    TbSint32           m_CurrentLocalPhase;                     // 現在呼び出し中関数のローカルフェイズ
    EventCallbackType  m_EventCallBack[Self::EVENT_TYPE_MAX];   // イベントコールバック
};

}

#endif // _INCLUDED_TB_ASYNC_FUNC_H_
