/*!
 * 巨大用入力
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_GIANT_INPUT_H_
#define _INCLUDED_SYS_GIANT_INPUT_H_

#include "sys_input.h"
#include <input/tb_pad.h>
#include <framework/debug/menu/tb_debug_menu_all.h>

class SysGiantInput : public TbSingletonRef<SysGiantInput>
{
    typedef SysGiantInput Self;
public:

    // コンストラクタ
    SysGiantInput();

    // デストラクタ
    ~SysGiantInput();

public:
    
    // 初期化
    void Initialize();
    
    // 定期処理
    void Update();

    // ボタンを押しているか
    TbBool IsOn( SysInput::Button button , TbUint32 playerIndex );

    // ボタンを押しているか
    TbBool IsOnRepeat( SysInput::Button button , TbUint32 playerIndex );

    // ボタンを押したか
    TbBool IsPress( SysInput::Button button , TbUint32 playerIndex);

    // ボタンを話したか
    TbBool IsRelease( SysInput::Button button , TbUint32 playerIndex);

public:

    // デバッグメニューセットアップ
    void SetupDebugMenu( TbDebugMenu* debugMenu );

private:

    TbSint32 getIndex( SysInput::Button button );

    // 入力セットアップ
    void     onClickedSetupInput();

    // 設定書き出し
    void writeGiantSetting();

private:

    TbPadInfo   m_PadInfo;
    TbBool      m_DebugPhase;

};

#endif
