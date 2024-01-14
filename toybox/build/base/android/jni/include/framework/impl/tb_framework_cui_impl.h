/*!
 * フレームワーク実装部分の共通
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_FRAMEWORK_CUI_IMPL_H_
#define _INCLUDED_TB_FRAMEWORK_CUI_IMPL_H_

#include <framework/impl/tb_framework_impl_common.h>
#include <base/io/resource/tb_resource_manager.h>
#include <base/time/tb_tick.h>

namespace toybox
{

class TbFrameworkCuiImpl : private TbFrameworkImplCommon
{
public:
    typedef TbFrameworkImplCommon Super;
public:
    // コンストラクタ
    TbFrameworkCuiImpl( TbUint32 taskLineMax ,
                        const TbResourceManager::InitParam& resourceParam )
        : Super( taskLineMax )
        , m_ResourceManager(resourceParam)
    {
    }

    // デストラクタ
    ~TbFrameworkCuiImpl()
    {
    }

public:

    // メインループスタート
    void StartMainLoop( MainLoopFunc begin , MainLoopFunc main , MainLoopFunc end )
    {
        Super::StartMainLoop(begin,main,end);
    }

    // メインループ終了
    void FinishMainLoop()
    {
        Super::FinishMainLoop();
    }

    // メインループは有効か
    TbBool IsEnableMainLoop() const {
        return Super::IsEnableMainLoop();
    }

    // 定期処理
    void Update();

private:
    TbStackTrace         m_StackTrace;
    TbResourceManager    m_ResourceManager;
};

}

#endif
