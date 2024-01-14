/*!
 * オプションメニュー
 * @author teco
 */

#include "fwd.h"
#include "common/ui/ui_option_window.h"
#include <crayon/input/cr_input.h>
#include "root.h"
#include "common/cmn_util.h"
#include <base/util/tb_bind.h>
#include <input/tb_pad.h>

static const TbFloat32 BUTTON_Y = 116.0f;
static const TbFloat32 BUTTON_INTERVAL_Y = 64.0f;

static const TbFloat32 OK_CANCEL_OFFSET_X[] =
{
    -40.0f , 
    40.0f  ,
};

/*!
 * コンストラクタ
 * @author teco
 */
UiOptionWindow::UiOptionWindow()
    : Super(TB_FALSE)
    , m_MainFocus(MAIN_FOCUS_ITEM)
    , m_FocusItemIndex(0)
    , m_FocusButtonIndex(0)
    , m_DefautBgColor(0)
    , m_DefautBgmVolume(CMN_SOUND_VOLUME_LEVEL_DEFAULT)
    , m_DefaultVisiblePlayerNumber(TB_FALSE)
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
        { Item::TYPE_SELECT , "JUMP/SELECT"  , TbRectF32Pod() , 0.0f , 84.0f } ,
        { Item::TYPE_SELECT , "BG COLOR"     , TbRectF32Pod() , 0.0f , 84.0f } ,
        { Item::TYPE_SELECT , "BGM VOLUME"   , TbRectF32Pod() , 0.0f , 84.0f } ,
        { Item::TYPE_SELECT , "DISP NUMBER"  , TbRectF32Pod() , 0.0f , 84.0f } ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ITEM_TABLE)==ITEM_MAX);
    
    TbFloat32 itemX = 500.0f;
    TbFloat32 itemY = 60.0f;
    TbFloat32 captionOffset = -20.0f;

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(ITEM_TABLE); ++i )
    {
        m_Item[i].type = ITEM_TABLE[i].type;
        switch( m_Item[i].type )
        {
        case Item::TYPE_SELECT:
            m_Item[i].selectBox.Enter(this);
            m_Item[i].selectBox.SetPos( itemX , itemY );
            m_Item[i].selectBox.SetEnableFocusFrame();
            m_Item[i].selectBox.SetItemMaxSizeName("WHITE");
            switch( i )
            {
            case ITEM_CHANGE_JUMP_BUTTON:
                m_Item[i].selectBox.AddItem("#");
                m_Item[i].selectBox.AddItem("$");
                m_Item[i].selectBox.SetEnableLoop(TB_TRUE);
                m_Item[i].selectBox.SetSelectedIndex(SaveFile::GetInstance().IsChangeJumpButton());
                m_Item[i].selectBox.SetChangedCallback(TbCreateMemFunc(this, &Self::onChangedJumpButton));
                break;
            case ITEM_BG_COLOR:
                m_Item[i].selectBox.AddItem("WHITE");
                m_Item[i].selectBox.AddItem("CREAM");
                m_Item[i].selectBox.SetEnableLoop(TB_TRUE);
                m_DefautBgColor = SaveFile::GetInstance().GetBgColorType();
                m_Item[i].selectBox.SetSelectedIndex(m_DefautBgColor);
                m_Item[i].selectBox.SetChangedCallback(TbCreateMemFunc(this, &Self::onChangedBgColor));
                break;
            case ITEM_VOLUME:
                m_DefautBgmVolume = SaveFile::GetInstance().GetSoundBgmVolumeLevel();
                for( TbSint32 volume = 0; volume <= CMN_SOUND_VOLUME_LEVEL_MAX; ++volume ) {
                    TbStaticString16 str;
                    str.SetFormatedString("%d",volume);
                    m_Item[i].selectBox.AddItem(str.GetBuffer());
                }
                m_Item[i].selectBox.SetEnableLoop(TB_FALSE);
                m_Item[i].selectBox.SetSelectedIndex(Root::GetInstance().GetSoundVolumeLevel());
                m_Item[i].selectBox.SetChangedCallback(TbCreateMemFunc(this,&Self::onChangedVolume));
                break;
            case ITEM_DISP_PLAYER_NUMBER:
                m_Item[i].selectBox.AddItem("OFF");
                m_Item[i].selectBox.AddItem("ON");
                m_Item[i].selectBox.SetEnableLoop(TB_TRUE);
                m_Item[i].selectBox.SetSelectedIndex(SaveFile::GetInstance().IsVisiblePlayerNumber());
                m_Item[i].selectBox.SetChangedCallback(TbCreateMemFunc(this, &Self::onChangedVisiblePlayerNumber));
                break;
            }
            m_Item[i].selectBox.SetAlignY( TB_UI_ALIGN_Y_CENTER);
            m_Item[i].caption.SetText( ITEM_TABLE[i].name );
            m_Item[i].caption.SetAlignY( TB_UI_ALIGN_Y_CENTER);
            m_Item[i].caption.SetAlignX(TB_UI_ALIGN_X_RIGHT);
            m_Item[i].caption.SetTextAlignY( TB_UI_ALIGN_Y_CENTER );
            m_Item[i].caption.Enter(this);
            m_Item[i].caption.SetPos( itemX + captionOffset, itemY );
            break;
        }
        itemX += ITEM_TABLE[i].offsetX;
        itemY += ITEM_TABLE[i].offsetY;
    }

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].Enter(this);
        m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
        m_Button[i].SetPos(TbVector2(OK_CANCEL_OFFSET_X[i],-10.0f));
        m_Button[i].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
        m_Button[i].SetTextAlign( TB_UI_ALIGN_X_CENTER );
        m_Button[i].SetEnable(TB_TRUE);
    }
    m_Button[0].SetAlignX(TB_UI_ALIGN_X_RIGHT);
    m_Button[0].SetText("CANCEL");
    m_Button[0].SetEnableResize(TB_FALSE);
    m_Button[0].SetText("OK");
    m_Button[0].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_OK));
    m_Button[1].SetAlignX(TB_UI_ALIGN_X_LEFT);
    m_Button[1].SetText("CANCEL");
    m_Button[1].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_CANCEL));

#if UI_OPTION_ENABLE_FULLSCREEN
    SetClientSize(800.0f, 450.0f);
#else
    SetClientSize(800.0f, 450.0f);
#endif
    fit();
    SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);

//    SetEnableCloseButton(TB_FALSE);
}


/*!
 * デストラクタ
 * @author teco
 */
UiOptionWindow::~UiOptionWindow()
{
    ClearWidget();
}

/*!
 * 派生クラス用
 * @author teco
 */
void UiOptionWindow::OnUpdate( TbFloat32 deltasec  )
{
    Super::OnUpdate(deltasec);
    if( !IsOpened() ) {
        return;
    }
    if (!IsFocus()) {
        return;
    }

    updateFocusInfo();
    TbUiManager& manager = TbUiManager::GetInstance();

    // フォーカスが当たっていて、利用状態の場合はクリック処理
    if( m_MainFocus == MAIN_FOCUS_ITEM )
    {
        if( manager.IsPress(TbUiInput::BUTTON_DOWN,GetPlayerBit()) ){
            if( m_FocusItemIndex < TB_ARRAY_COUNT_OF(m_Item)-1 ){
                ++m_FocusItemIndex;
                m_Item[m_FocusItemIndex].selectBox.SetFocus();
            }else{
                m_MainFocus = MAIN_FOCUS_BUTTON;
                m_Button[m_FocusButtonIndex].SetFocus();
            }
        }else if( manager.IsPress(TbUiInput::BUTTON_UP,GetPlayerBit()) ){
            if( 0 < m_FocusItemIndex ){
                --m_FocusItemIndex;
                m_Item[m_FocusItemIndex].selectBox.SetFocus();
            }
        }
    }else{
        if( manager.IsPress(TbUiInput::BUTTON_LEFT,GetPlayerBit()) || 
            manager.IsPress(TbUiInput::BUTTON_RIGHT,GetPlayerBit()) )
        {
            m_FocusButtonIndex = (m_FocusButtonIndex+1)%TB_ARRAY_COUNT_OF(m_Button);
            m_Button[m_FocusButtonIndex].SetFocus();
        }else if( manager.IsPress(TbUiInput::BUTTON_UP,GetPlayerBit()) ){
            m_MainFocus = MAIN_FOCUS_ITEM;
            m_Item[m_FocusItemIndex].selectBox.SetFocus();
        }
    }
}

/*!
 * 生成時
 * @author teco
 */
void UiOptionWindow::OnCreated()
{
    Super::OnCreated();
    m_MainFocus = MAIN_FOCUS_ITEM;
    m_FocusItemIndex = 0;
    m_FocusButtonIndex = BUTTON_CANCEL;
    m_Item[m_FocusItemIndex].selectBox.SetFocus();

    m_Item[ITEM_CHANGE_JUMP_BUTTON].selectBox.SetSelectedIndex(SaveFile::GetInstance().IsChangeJumpButton());
    m_Item[ITEM_VOLUME].selectBox.SetSelectedIndex(SaveFile::GetInstance().GetSoundBgmVolumeLevel());
    m_DefautBgmVolume = SaveFile::GetInstance().GetSoundBgmVolumeLevel();
    m_DefautBgColor = SaveFile::GetInstance().GetBgColorType();
    m_DefaultVisiblePlayerNumber = SaveFile::GetInstance().IsVisiblePlayerNumber();
    m_Item[ITEM_BG_COLOR].selectBox.SetSelectedIndex(m_DefautBgColor);
    m_Item[ITEM_DISP_PLAYER_NUMBER].selectBox.SetSelectedIndex(m_DefaultVisiblePlayerNumber ? 1 : 0);

#if UI_OPTION_ENABLE_FULLSCREEN
//    m_Item[ITEM_WINDOW_MODE].selectBox.SetSelectedIndex(Root::GetInstance().IsFullscreen());
#endif
    m_Result = RESULT_NONE;
}

/*! 
 * 自動リサイズ
 * @author teco
 * @since 2014.12.18
 */
void UiOptionWindow::fit()
{
    TbFloat32 width  = 0.0f;
}

/*! 
 * 決定時コールバック
 * @author teco
 */
void UiOptionWindow::onDecidedCallback( Result result ) {
    m_Result = result;
    if( m_DecidedCallback.IsValid() ){
        m_DecidedCallback(result);
    }
    if( RESULT_OK == m_Result ){
        SaveFile::GetInstance().SetSoundBgmVolumeLevel(m_Item[ITEM_VOLUME].selectBox.GetSelectedIndex());
        SaveFile::GetInstance().SetChangeJumpButton(m_Item[ITEM_CHANGE_JUMP_BUTTON].selectBox.GetSelectedIndex());
        SaveFile::GetInstance().SetVisiblePlayerNumber(m_Item[ITEM_DISP_PLAYER_NUMBER].selectBox.GetSelectedIndex());
        // フルスクリーン設定
        // Root::GetInstance().SetFullscreen(!Root::GetInstance().IsFullscreen());
        // Root::GetInstance().SaveUserData();
    }else{
        // 元に戻す
        SaveFile::GetInstance().SetSoundBgmVolumeLevel(m_DefautBgmVolume);
        SaveFile::GetInstance().SetBgColorType(m_DefautBgColor);
        SaveFile::GetInstance().SetVisiblePlayerNumber(m_DefaultVisiblePlayerNumber);
    }
    Close();
//    SetFocus(TB_FALSE);
}

/*!
 *  音量変更通知
 * @author teco
 */
void UiOptionWindow::onChangedJumpButton(TbSint32 index)
{
}

// 背景カラー変更
void UiOptionWindow::onChangedBgColor(TbSint32 index)
{
    SaveFile::GetInstance().SetBgColorType(index);
}

/*!
 * 音量変更通知
 * @author teco
 */
void UiOptionWindow::onChangedVolume( TbSint32 index )
{
    TbFloat32 volume = CmnUtil::ConvertSoundLevelToVolume(index);
//    TbSoundManager::GetInstance().SetMasterVolume(volume);
    SeqManager::GetInstance().SetBgmVolume(volume);
}

/*!
 * プレイヤー番号表示変更
 * @author teco
 */
void UiOptionWindow::onChangedVisiblePlayerNumber(TbSint32 index)
{
    SaveFile::GetInstance().SetVisiblePlayerNumber(0 < index);
}

/*! 
 * フォーカス情報更新
 * @author teco
 */
void UiOptionWindow::updateFocusInfo()
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Item); ++i ) {
        if( m_Item[i].selectBox.IsFocused() ){
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
