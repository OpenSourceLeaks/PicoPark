/*!
 * 選択ウィンドウ
 * @author teco
 */

#include "fwd.h"
#include "title/ui/ui_title_select_window.h"
#include "system/sys_input.h"
#include "root.h"

#include <base/util/tb_bind.h>

static const char* ITEM_TABLE[] =
{
    "2 PLAYERS GAME",
    "3 PLAYERS GAME",
    "4 PLAYERS GAME",
    "5 PLAYERS GAME",
    "6 PLAYERS GAME",
#if 7 <= CMN_PLAYER_COUNT
    "7 PLAYERS GAME",
#endif
#if 8 <= CMN_PLAYER_COUNT
    "8 PLAYERS GAME",
#endif
#if 9 <= CMN_PLAYER_COUNT
    "9 PLAYERS GAME",
#endif
#if 10 <= CMN_PLAYER_COUNT
    "10 PLAYERS GAME",
#endif
#if UI_TITLE_SELECT_ENABLE_OPTION
    "GAME PAD CONFIG",
#if TB_PLATFORM_IS_WIN
    "KEYBOARD CONFIG",
#endif
    "OPTION",
#if TB_PLATFORM_IS_WIN
    "EXIT GAME"
#endif
#endif
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ITEM_TABLE)==UiTitleSelectWindow::ITEM_MAX);

/*!
 * コンストラクタ
 * @author teco
 */
UiTitleSelectWindow::UiTitleSelectWindow()
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(ITEM_TABLE); ++i ) {
        AddSelectItem( ITEM_TABLE[i] );
    }
    SetSelectedIndex( ITEM_2P );
}

/*!
 * デストラクタ
 * @author teco
 */
UiTitleSelectWindow::~UiTitleSelectWindow()
{
}
