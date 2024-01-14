/*!
 * 巨大用入力
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_GIANT_INPUT_H_
#define _INCLUDED_SYS_GIANT_INPUT_H_

#include "crayon/input/cr_input.h"
#include <input/tb_pad.h>
#include <framework/debug/menu/tb_debug_menu_all.h>

namespace crayon
{

class CrGiantInput : public toybox::TbSingletonRef<CrGiantInput>
{
    typedef CrGiantInput Self;
public:

    // コンストラクタ
    CrGiantInput();

    // デストラクタ
    ~CrGiantInput();

public:
    
    // 初期化
    void Initialize();
    
    // 定期処理
    void Update();

    // ボタンを押しているか
    TbBool IsOn( CrInput::Button button , TbUint32 playerIndex );

    // ボタンを押しているか
    TbBool IsOnRepeat( CrInput::Button button , TbUint32 playerIndex );

    // ボタンを押したか
    TbBool IsPress( CrInput::Button button , TbUint32 playerIndex);

    // ボタンを話したか
    TbBool IsRelease( CrInput::Button button , TbUint32 playerIndex);

public:

    // デバッグメニューセットアップ
    void SetupDebugMenu( toybox::TbDebugMenu* debugMenu );

private:

    TbSint32 getIndex( CrInput::Button button );

    // 入力セットアップ
    void     onClickedSetupInput();

    // 設定書き出し
    void writeGiantSetting();

private:

    toybox::TbPadInfo   m_PadInfo;
    TbBool      m_DebugPhase;

};

}

#endif
