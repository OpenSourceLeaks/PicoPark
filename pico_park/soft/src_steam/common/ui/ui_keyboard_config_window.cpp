/*!
 * ポーズメニュー
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "common/ui/ui_keyboard_config_window.h"
#include "system/sys_input.h"
#include "root.h"
#include <base/util/tb_bind.h>
#include <input/tb_pad.h>

static const TbFloat32 BUTTON_Y = 116.0f;
static const TbFloat32 BUTTON_INTERVAL_Y = 64.0f;
static const TbFloat32 BUTTON_OFFSET_X[] =
{
    -140.0f , 
    0.0f  ,
    140.0f  ,
};

static const SysInput::Button ITEM_BUTTON_TABLE[UiKeyboardConfigWindow::ITEM_BUTTON_MAX][3]
{
    { SysInput::BUTTON_UP , SysInput::BUTTON_MAX , SysInput::BUTTON_MAX } , 
    { SysInput::BUTTON_DOWN , SysInput::BUTTON_MAX , SysInput::BUTTON_MAX } ,
    { SysInput::BUTTON_LEFT , SysInput::BUTTON_MAX , SysInput::BUTTON_MAX } , 
    { SysInput::BUTTON_RIGHT , SysInput::BUTTON_MAX , SysInput::BUTTON_MAX } , 
    { SysInput::BUTTON_DECIDE  , SysInput::BUTTON_JUMP , SysInput::BUTTON_ROTATE_R } , 
    { SysInput::BUTTON_CANCEL , SysInput::BUTTON_MAX , SysInput::BUTTON_MAX } , 
    { SysInput::BUTTON_START , SysInput::BUTTON_MAX , SysInput::BUTTON_MAX } , 
};

#if 1

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiKeyboardConfigWindow::UiKeyboardConfigWindow()
    : Super(TB_FALSE)
    , m_Phase(PHASE_IDLE)
    , m_MainFocus(MAIN_FOCUS_ITEM)
    , m_FocusItemIndex(0)
    , m_FocusButtonIndex(0)
    , m_Result(RESULT_NONE)
{
    struct ItemProperty
    {
        Item::Type type;
        const char*       name;
        TbRectF32Pod      uv;
        TbFloat32         offsetX;
        TbFloat32         offsetY;
    };

    static const ItemProperty ITEM_TABLE[] = 
    {
        { Item::TYPE_SELECT , "PLAYER" , TbRectF32Pod() , 0.0f , 72.0f } ,
        { Item::TYPE_BUTTON , "UP" , TbRectF32Pod() , 0.0f , 60.0f } ,
        { Item::TYPE_BUTTON , "DOWN" , TbRectF32Pod() , 0.0f , 60.0f } ,
        { Item::TYPE_BUTTON , "LEFT" , TbRectF32Pod() , 0.0f , 60.0f } ,
        { Item::TYPE_BUTTON , "RIGHT" , TbRectF32Pod() , 0.0f , 60.0f } ,
        { Item::TYPE_BUTTON , "ACTION/SELECT" , TbRectF32Pod() , 0.0f , 60.0f } ,
        { Item::TYPE_BUTTON , "CANCEL" , TbRectF32Pod() , 0.0f , 60.0f  } ,
        { Item::TYPE_BUTTON , "START" , TbRectF32Pod() , 0.0f , 60.0f  } ,
        { Item::TYPE_BUTTON , "CHARA CHANGE" , TbRectF32Pod() , 0.0f , 60.0f } ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ITEM_TABLE)==ITEM_MAX);

    m_Title.Enter(this);
    m_Title.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_Title.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_Title.SetText("[KEYBOARD CONFIG]");

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Line); ++i ) {
        m_Line[i].Enter(this);
        m_Line[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_Line[i].SetAlignX( TB_UI_ALIGN_X_CENTER );
        m_Line[i].SetSize(866.0f,2.0f);
    }
    m_Line[0].SetPosY( 57.0f );
    m_Line[1].SetPosY( 131.0f );
    m_Line[2].SetPosY( 620.0f );

    TbFloat32 itemX = 438.0f;
    TbFloat32 itemY = 95.0f;
    TbStaticString32 str;
    for( TbSizeT item = 0; item < TB_ARRAY_COUNT_OF(ITEM_TABLE); ++item )
    {
        m_Item[item].type = ITEM_TABLE[item].type;
        m_Item[item].caption.SetText( ITEM_TABLE[item].name );
        m_Item[item].caption.Enter(this);
        m_Item[item].caption.SetPos( 0.0f , itemY );
        m_Item[item].caption.SetAlignY( TB_UI_ALIGN_Y_CENTER );
        m_Item[item].caption.SetTextAlignY( TB_UI_ALIGN_Y_CENTER);
        
        if( item == ITEM_PLAYER_INDEX ) {
            m_Item[item].selectBox.SetEnableFocusFrame();
            for( TbSint32 p = 0; p < CMN_PLAYER_MAX; ++p ) {
                str.SetFormatedString("%dP",p+1);
                m_Item[item].selectBox.AddItem(str.GetBuffer());
            }
            m_Item[item].selectBox.SetChangedCallback(TbCreateMemFunc(this,&Self::onChangedPlayer));
        }else if( ITEM_CHARA_CHANGE == item ) {
            m_Item[item].button.SetText("NUM KEYS");
//            m_Item[item].button.SetPressedCallback(TbCreateMemFunc(this,&Self::onPressedResetButton));
        }else if( ITEM_KEY_BEGIN <= item && item < ITEM_KEY_END ) {
            m_Item[item].button.SetPressedCallback(TbCreateMemFunc(this,&Self::onPressedKeyButton));
        }

        switch( m_Item[item].type )
        {
        case Item::TYPE_SELECT:
            m_Item[item].selectBox.Enter(this);
            m_Item[item].selectBox.SetPos( itemX , itemY );
            m_Item[item].selectBox.SetAlignY( TB_UI_ALIGN_Y_CENTER );
            m_Item[item].selectBox.SetItemMaxSizeName("                     ");
            break;

        case Item::TYPE_BUTTON:
            m_Item[item].button.Enter(this);
            m_Item[item].button.SetSizeWithText("                          ");
            m_Item[item].button.SetEnableResize(TB_FALSE);
            m_Item[item].button.SetAlign( TB_UI_ALIGN_X_LEFT, TB_UI_ALIGN_Y_CENTER);
            m_Item[item].button.SetTextAlign( TB_UI_ALIGN_X_CENTER );
            m_Item[item].button.SetPos( itemX , itemY );
            m_Item[item].button.SetEnable(TB_TRUE);
            break;
        }
       
        itemX += ITEM_TABLE[item].offsetX;
        itemY += ITEM_TABLE[item].offsetY;
    }

    m_Item[ITEM_CHARA_CHANGE].SetEnable(TB_FALSE);
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].Enter(this);
        m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
        m_Button[i].SetPos(TbVector2(BUTTON_OFFSET_X[i],0.0f));
        m_Button[i].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
        m_Button[i].SetTextAlign( TB_UI_ALIGN_X_CENTER );
        m_Button[i].SetEnable(TB_TRUE);
        m_Button[i].SetText("DEFAULT");
        m_Button[i].SetEnableResize(TB_FALSE);
    }
    m_Button[0].SetAlignX(TB_UI_ALIGN_X_RIGHT);
    m_Button[0].SetText("OK");
    m_Button[0].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_OK));

    m_Button[1].SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_Button[1].SetText("CANCEL");
    m_Button[1].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_CANCEL));

    m_Button[2].SetAlignX(TB_UI_ALIGN_X_LEFT);
    m_Button[2].SetText("DEFAULT");
    m_Button[2].SetPressedCallback(TbCreateMemFunc(this,&Self::onPressedResetButton));

    SetClientSize(900.0f,686.0f);
    fit();
    SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);

    // 入力情報取得
    for( TbSint32 p = 0; p < TB_ARRAY_COUNT_OF(m_ItemButtonInfo); ++p ){
        for( TbSint32 i = 0; i < ITEM_BUTTON_MAX; ++i ){
            m_ItemButtonInfo[p].buttonKey[i] = SysInput::GetButtonKeyboardKey(p,ITEM_BUTTON_TABLE[i][0]);
        }
    }
}


/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiKeyboardConfigWindow::~UiKeyboardConfigWindow()
{
    ClearWidget();
}

/*!
 * 派生クラス用
 * @author teco
 * @since 2014.12.05
 */
void UiKeyboardConfigWindow::OnUpdate( TbFloat32 deltasec  )
{
    Super::OnUpdate(deltasec);
    if( !IsOpened() ) {
        return;
    }

    TbBool isEnableNext = TB_FALSE;
    updateFocusInfo();
    TbUiManager& manager = TbUiManager::GetInstance();

    if( PHASE_TRIGGER_INPUT  == m_Phase )
    {
        m_Phase = PHASE_WAIT_INPUT;
    }
    else if( PHASE_WAIT_INPUT == m_Phase )
    {
        TbSint32 playerIndex = m_Item[ITEM_PLAYER_INDEX].selectBox.GetSelectedIndex();
        for( TbSint32 i = TB_KEY_BEGIN; i < TB_KEY_END; ++i ) {
            TbKeyType type = static_cast<TbKeyType>(i);
            if( TbKeyboard::IsPress(type) ) {
                m_Item[m_FocusItemIndex].button.SetText(TbKeyboard::GetString(type));
                m_ItemButtonInfo[playerIndex].buttonKey[m_FocusItemIndex-ITEM_KEY_BEGIN] = type;
                m_Phase = PHASE_IDLE;
                SetEnableInput(TB_TRUE);
                break;
            }
        }
    }
    else if( MAIN_FOCUS_ITEM == m_MainFocus )
    {
        if( manager.IsPress(TbUiInput::BUTTON_DOWN,GetPlayerBit()) ){
            TbSint32 nextFocusIndex = -1;
            for( TbUint32 i = m_FocusItemIndex+1; i < TB_ARRAY_COUNT_OF(m_Item); ++i ) {
                if( m_Item[i].IsEnable() ){
                    nextFocusIndex = i;
                    break;
                }
            }
            if( 0 <= nextFocusIndex ){
                m_FocusItemIndex = nextFocusIndex;
                m_Item[m_FocusItemIndex].SetFocus();
            }else{
                m_MainFocus = MAIN_FOCUS_BUTTON;
                m_Button[m_FocusButtonIndex].SetFocus();
            }
        }else if( manager.IsPress(TbUiInput::BUTTON_UP,GetPlayerBit()) ){
            TbSint32 nextFocusIndex = -1;
            for( TbSint32 i = m_FocusItemIndex-1; 0 <= i; --i ) {
                if( m_Item[i].IsEnable() ){
                    nextFocusIndex = i;
                    break;
                }
            }
            if( 0 <= nextFocusIndex ){
                m_FocusItemIndex = nextFocusIndex;
                m_Item[m_FocusItemIndex].SetFocus();
            }
        }
    }else{
        if( manager.IsPress(TbUiInput::BUTTON_RIGHT,GetPlayerBit()) )
        {
            m_FocusButtonIndex = (m_FocusButtonIndex+1)%TB_ARRAY_COUNT_OF(m_Button);
            m_Button[m_FocusButtonIndex].SetFocus();
        }
        else if( manager.IsPress(TbUiInput::BUTTON_LEFT,GetPlayerBit()) )
        {
            if( 0 == m_FocusButtonIndex ){
                m_FocusButtonIndex = TB_ARRAY_COUNT_OF(m_Button)-1;
            }else{
                --m_FocusButtonIndex;
            }
            m_Button[m_FocusButtonIndex].SetFocus();
        }
        else if( manager.IsPress(TbUiInput::BUTTON_UP,GetPlayerBit()) ){
            m_MainFocus = MAIN_FOCUS_ITEM;
            m_Item[m_FocusItemIndex].SetFocus();
        }
    }

}

/*!
 * 生成時
 * @author teco
 */
void UiKeyboardConfigWindow::OnCreated()
{
    // 入力情報取得
    for( TbSint32 p = 0; p < TB_ARRAY_COUNT_OF(m_ItemButtonInfo); ++p ){
        for( TbSint32 i = 0; i < ITEM_BUTTON_MAX; ++i ){
            m_ItemButtonInfo[p].buttonKey[i] = SysInput::GetButtonKeyboardKey(p,ITEM_BUTTON_TABLE[i][0]);
        }
    }

    Super::OnCreated();
    m_MainFocus = MAIN_FOCUS_ITEM;
    m_FocusItemIndex = 0;
    m_FocusButtonIndex = BUTTON_CANCEL;
    m_Item[ITEM_PLAYER_INDEX].selectBox.SetSelectedIndex(0);
    updateItemKeyName(0);
    m_Item[m_FocusItemIndex].SetFocus();
    m_Result = RESULT_NONE;
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Item); ++i )
    {
        switch( m_Item[i].type )
        {
        case Item::TYPE_BUTTON:
            break;
        }
    }
}

/*! 
 * 自動リサイズ
 * @author teco
 */
void UiKeyboardConfigWindow::fit()
{
    TbFloat32 width  = 0.0f;
}

/*! 
 * 決定時コールバック
 * @author teco
 */
void UiKeyboardConfigWindow::onDecidedCallback( Result result )
{
    m_Result = result;
    switch( m_Result )
    {
    case RESULT_OK:
        {
            // 反映
            for( TbSint32 p = 0; p < TB_ARRAY_COUNT_OF(m_ItemButtonInfo); ++p ){
                for( TbSint32 i = 0; i < ITEM_BUTTON_MAX; ++i ){
                    for( TbSint32 j = 0; j < ITEM_BUTTON_MAX; ++j ){
                        if( ITEM_BUTTON_TABLE[i][j] == SysInput::BUTTON_MAX ){
                            break;
                        }
                        TbKeyType keyType = m_ItemButtonInfo[p].buttonKey[i];
                        SysInput::BindButtonKeyboardKey(p,ITEM_BUTTON_TABLE[i][j],keyType);
                    }
                }
            }
            Root::GetInstance().SaveUserData();
        }
        break;
    case RESULT_CANCEL:
        break;
    }
    Close();
}

/*! 
 * リセットボタン押した
 * @author teco
 */
void UiKeyboardConfigWindow::onPressedResetButton()
{
    for( TbSint32 playerIndex = 0; playerIndex < CMN_PLAYER_MAX; ++playerIndex ) {
        if( playerIndex == 0 ){
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_UP] = TB_KEY_UP;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_DOWN] = TB_KEY_DOWN;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_LEFT] = TB_KEY_LEFT;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_RIGHT] = TB_KEY_RIGHT;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_A] = TB_KEY_SPACE;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_B] = TB_KEY_BACK;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_START] = TB_KEY_RETURN;
        }else if( playerIndex == 1 ){
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_UP] = TB_KEY_W;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_DOWN] = TB_KEY_S;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_LEFT] = TB_KEY_A;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_RIGHT] = TB_KEY_D;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_A] = TB_KEY_W;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_B] = TB_KEY_INVALID;
            m_ItemButtonInfo[playerIndex].buttonKey[ITEM_BUTTON_START] = TB_KEY_INVALID;
        }else{
            for( TbSint32 i = 0; i < ITEM_BUTTON_MAX; ++i ){
                m_ItemButtonInfo[playerIndex].buttonKey[i] = TB_KEY_INVALID;
            }
        }
    }
    updateItemKeyName(m_Item[ITEM_PLAYER_INDEX].selectBox.GetSelectedIndex());
}

/*! 
 * キーアサイン用ボタン押した
 * @author teco
 */
void UiKeyboardConfigWindow::onPressedKeyButton()
{
    m_Phase = PHASE_TRIGGER_INPUT ;
    m_Item[m_FocusItemIndex].button.SetText("PRESS ANY KEY");
    SetEnableInput(TB_FALSE);
}

/*! 
 * 対象プレイヤー決まった
 * @author teco
 */
void UiKeyboardConfigWindow::onChangedPlayer( TbSint32 index )
{
    updateItemKeyName(index);
    if( 0 == index ) {
        // 1Pのみキーボードでスタート設定可能
        m_Item[ITEM_KEY_BEGIN+ITEM_BUTTON_B].SetEnable(TB_TRUE);
        m_Item[ITEM_KEY_BEGIN+ITEM_BUTTON_START].SetEnable(TB_TRUE);
        m_Item[ITEM_CHARA_CHANGE].button.SetText("NUM KEYS");
    }else{
        m_Item[ITEM_KEY_BEGIN+ITEM_BUTTON_B].SetEnable(TB_FALSE);
        m_Item[ITEM_KEY_BEGIN+ITEM_BUTTON_START].SetEnable(TB_FALSE);
        m_Item[ITEM_CHARA_CHANGE].button.SetText("-");
    }
}

/*!
 * フォーカス情報更新
 * @author teco
 */
void UiKeyboardConfigWindow::updateFocusInfo()
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Item); ++i ) {
        if( m_Item[i].IsFocused() ){
            m_FocusItemIndex = i;
            m_MainFocus = MAIN_FOCUS_ITEM;
            return;
        } 
    }
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        if( m_Button[i].IsFocused() ){
            m_FocusButtonIndex = i;
            m_MainFocus = MAIN_FOCUS_BUTTON;
            return;
        } 
    }
}

/*!
 * ITEM_KEYの名前を更新
 * @author teco
 */
void UiKeyboardConfigWindow::updateItemKeyName( TbSint32 playerIndex )
{
    ItemButtonInfo& info = m_ItemButtonInfo[playerIndex];
    for( TbSint32 i = 0; i < ITEM_BUTTON_MAX; ++i ){
        if( info.buttonKey[i] == TB_KEY_INVALID ) {
            m_Item[ITEM_KEY_BEGIN+i].button.SetText("-");
        }else{
            m_Item[ITEM_KEY_BEGIN+i].button.SetText(TbKeyboard::GetString(info.buttonKey[i]));
        }
    }
}

#endif
