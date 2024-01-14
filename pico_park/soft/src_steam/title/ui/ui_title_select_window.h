/*!
 * 選択ウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_TITLE_SELECT_WINDOW_H_
#define _INCLUDED_UI_TITLE_SELECT_WINDOW_H_

#include "ui/ui_select_window.h"

#define UI_TITLE_SELECT_ENABLE_OPTION (0 || (1 && !CMN_ENABLE_BUILD_8BITCAFE && !CMN_ENABLE_BUILD_ARCADE))

class UiTitleSelectWindow : public UiSelectWindow
{
public:

    typedef UiSelectWindow Super;
    typedef UiTitleSelectWindow Self;

public:

    enum Item
    {
        ITEM_GAME_BEGIN , 
        ITEM_2P = ITEM_GAME_BEGIN , 
        ITEM_3P , 
        ITEM_4P , 
        ITEM_5P , 
        ITEM_6P , 
#if 7 <= CMN_PLAYER_COUNT
        ITEM_7P , 
#endif
#if 8 <= CMN_PLAYER_COUNT
        ITEM_8P , 
#endif
#if 9 <= CMN_PLAYER_COUNT
        ITEM_9P , 
#endif
#if 10 <= CMN_PLAYER_COUNT
        ITEM_10P , 
#endif
        ITEM_GAME_END , 
#if UI_TITLE_SELECT_ENABLE_OPTION
        ITEM_GAMEPAD_CONFIG = ITEM_GAME_END, 
#if TB_PLATFORM_IS_WIN
        ITEM_KEYBOARD_CONFIG ,
#endif
        ITEM_OPTION ,
#if TB_PLATFORM_IS_WIN
        ITEM_EXIT_GAME ,
#endif
        ITEM_MAX
#else
        ITEM_MAX = ITEM_GAME_END
#endif
    };

public:
    
    // コンストラクタ
    UiTitleSelectWindow();
    
    // デストラクタ
    ~UiTitleSelectWindow();
    
};

#endif
