/*!
 * オプションメニュー
 * @author teco
 */

#include "fwd.h"
#include "common/ui/ui_option_window_old.h"
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
UiOptionWindowOld::UiOptionWindowOld()
    : Super(TB_FALSE)
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
        { Item::TYPE_SELECT , "BGM VOLUME" , TbRectF32Pod() , 0.0f , 84.0f } ,
#if UI_OPTION_ENABLE_FULLSCREEN
        { Item::TYPE_SELECT , "WINDOW MODE" , TbRectF32Pod() , 0.0f , 70.0f  } ,
#endif
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ITEM_TABLE)==ITEM_MAX);

    m_Title.Enter(this);
    m_Title.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_Title.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_Title.SetText("[OPTION]");

    TbFloat32 itemX = 400.0f;
    TbFloat32 itemY = 90.0f;

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(ITEM_TABLE); ++i )
    {
        m_Item[i].type = ITEM_TABLE[i].type;
        switch( m_Item[i].type )
        {
        case Item::TYPE_SELECT:
            m_Item[i].selectBox.Enter(this);
            m_Item[i].selectBox.SetPos( itemX , itemY );
            m_Item[i].selectBox.SetEnableFocusFrame();
            if( i == ITEM_VOLUME ){
                m_Item[i].selectBox.SetItemMaxSizeName("FULLSCREEN");
                for( TbSint32 volume = 0; volume <= CMN_SOUND_VOLUME_LEVEL_MAX; ++volume ) {
                    TbStaticString16 str;
                    str.SetFormatedString("%d",volume);
                    m_Item[i].selectBox.AddItem(str.GetBuffer());
                }
                m_Item[i].selectBox.SetEnableLoop(TB_FALSE);
                m_Item[i].selectBox.SetSelectedIndex(Root::GetInstance().GetSoundVolumeLevel());
                m_Item[i].selectBox.SetChangedCallback(TbCreateMemFunc(this,&Self::onChangedVolume));
#if UI_OPTION_ENABLE_FULLSCREEN
            }else if( i == ITEM_WINDOW_MODE ) {
                m_Item[i].selectBox.SetItemMaxSizeName("FULLSCREEN");
                m_Item[i].selectBox.AddItem("  WINDOW  ");
                m_Item[i].selectBox.AddItem("FULLSCREEN");
                m_Item[i].selectBox.SetSelectedIndex(Root::GetInstance().IsFullscreen());
#endif
            }
            m_Item[i].selectBox.SetAlignY( TB_UI_ALIGN_Y_CENTER);
            m_Item[i].caption.SetText( ITEM_TABLE[i].name );
            m_Item[i].caption.SetAlignY( TB_UI_ALIGN_Y_CENTER);
            m_Item[i].caption.SetTextAlignY( TB_UI_ALIGN_Y_CENTER);
            m_Item[i].caption.Enter(this);
            m_Item[i].caption.SetPos( 0.0f , itemY );
            break;
        }
        itemX += ITEM_TABLE[i].offsetX;
        itemY += ITEM_TABLE[i].offsetY;
    }

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].Enter(this);
        m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
        m_Button[i].SetPos(TbVector2(OK_CANCEL_OFFSET_X[i],0.0f));
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
    SetClientSize(850.0f,300.0f);
#else
    SetClientSize(650.0f,200.0f);
#endif
    fit();
    SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
}


/*!
 * デストラクタ
 * @author teco
 */
UiOptionWindowOld::~UiOptionWindowOld()
{
    ClearWidget();
}

/*!
 * 派生クラス用
 * @author teco
 */
void UiOptionWindowOld::OnUpdate( TbFloat32 deltasec  )
{
    Super::OnUpdate(deltasec);
    if( !IsOpened() ) {
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
void UiOptionWindowOld::OnCreated()
{
    Super::OnCreated();
    m_MainFocus = MAIN_FOCUS_ITEM;
    m_FocusItemIndex = 0;
    m_FocusButtonIndex = BUTTON_CANCEL;
    m_Item[m_FocusItemIndex].selectBox.SetFocus();
    m_Item[ITEM_VOLUME].selectBox.SetSelectedIndex(Root::GetInstance().GetSoundVolumeLevel());
#if UI_OPTION_ENABLE_FULLSCREEN
    m_Item[ITEM_WINDOW_MODE].selectBox.SetSelectedIndex(Root::GetInstance().IsFullscreen());
#endif
    m_Result = RESULT_NONE;
}

/*! 
 * 自動リサイズ
 * @author teco
 * @since 2014.12.18
 */
void UiOptionWindowOld::fit()
{
    TbFloat32 width  = 0.0f;
}

/*! 
 * 決定時コールバック
 * @author teco
 */
void UiOptionWindowOld::onDecidedCallback( Result result ) {
    m_Result = result;
    if( m_DecidedCallback.IsValid() ){
        m_DecidedCallback(result);
    }
    if( RESULT_OK == m_Result ){
        Root::GetInstance().SetSoundVolumeLevel(m_Item[ITEM_VOLUME].selectBox.GetSelectedIndex());
        Root::GetInstance().SetFullscreen(!Root::GetInstance().IsFullscreen());
        // フルスクリーン設定
        // Root::GetInstance().SaveUserData();
    }else{
        Root::GetInstance().SetSoundVolumeLevel(Root::GetInstance().GetSoundVolumeLevel());
    }
    Close();
}

/*! 
 * 音量変更通知
 * @author teco
 */
void UiOptionWindowOld::onChangedVolume( TbSint32 index )
{
    TbFloat32 volume = CmnUtil::ConvertSoundLevelToVolume(index);
    TbSoundManager::GetInstance().SetMasterVolume(volume);
}

/*! 
 * フォーカス情報更新
 * @author teco
 */
void UiOptionWindowOld::updateFocusInfo()
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
