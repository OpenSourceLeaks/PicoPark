/*!
 * ポーズメニュー
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "common/ui/ui_gamepad_config_window.h"
#include <crayon/input/cr_input.h>
#include "root.h"
#include <crayon/input/cr_remote_input_server.h>
#include <base/util/tb_bind.h>
#include <input/tb_pad.h>

static const TbFloat32 BUTTON_Y = 116.0f;
static const TbFloat32 BUTTON_INTERVAL_Y = 64.0f;
static const TbUint32 TECO_GAME_PAD_MASK = 0xFF00;
static const TbPadButton TECO_GAME_PAD_BUTTON_TABLE[] = 
{
    TB_PAD_A , 
    TB_PAD_B ,
    TB_PAD_START
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiGamePadConfigWindow::UiGamePadConfigWindow()
    : Super(TB_FALSE)
    , m_FocusIndex(0)
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
        { Item::TYPE_TEXT , "PRODUCT" , TbRectF32Pod() , 0.0f , 84.0f } ,
        { Item::TYPE_TEXT , "ACTION/SELECT" , TbRectF32Pod() , 0.0f , 70.0f } ,
        { Item::TYPE_TEXT , "CANCEL" , TbRectF32Pod() , 0.0f , 70.0f } ,
        { Item::TYPE_TEXT , "START" , TbRectF32Pod() , 0.0f , 70.0f  } ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ITEM_TABLE)==ITEM_MAX);

    m_Title.Enter(this);
    m_Title.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_Title.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_Title.SetText("[GAME PAD CONFIG]");

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Line); ++i ) {
        m_Line[i].Enter(this);
        m_Line[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_Line[i].SetAlignX( TB_UI_ALIGN_X_CENTER );
        m_Line[i].SetSize(1096.0f,2.0f);
    }
    m_Line[0].SetPosY( 60.0f );
    m_Line[1].SetPosY( 138.0f );

    TbFloat32 itemX = 440.0f;
    TbFloat32 itemY = 70.0f;
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(ITEM_TABLE); ++i )
    {
        m_Item[i].type = ITEM_TABLE[i].type;
        switch( m_Item[i].type )
        {
        case Item::TYPE_TEXT:
            m_Item[i].text.SetEnableFrame();
            m_Item[i].text.Enter(this);
            m_Item[i].text.SetText("PUSH ANY PAD BUTTON");
            m_Item[i].text.SetPos( itemX , itemY );
            m_Item[i].text.SetEnableResize(TB_FALSE);
            m_Item[i].text.SetWidth(700.0f);
            m_Item[i].text.SetTextAlignX( TB_UI_ALIGN_X_CENTER );
            m_Item[i].caption.SetText( ITEM_TABLE[i].name );
            m_Item[i].caption.Enter(this);
            m_Item[i].caption.SetPos( 0.0f , itemY );
            break;
        case Item::TYPE_BUTTON:
            m_Item[i].button.Enter(this);
            m_Item[i].button.SetBasePos( TbUiWidget::BASE_POS_CENTER_TOP );
            m_Item[i].button.SetText( ITEM_TABLE[i].name );
            m_Item[i].button.SetTextAlign( TB_UI_ALIGN_X_CENTER );
            m_Item[i].button.SetPos( itemX , itemY );
            break;
        }
        itemX += ITEM_TABLE[i].offsetX;
        itemY += ITEM_TABLE[i].offsetY;
    }

    SetClientSize(1160.0f,350.0f);
    fit();
    SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
}


/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiGamePadConfigWindow::~UiGamePadConfigWindow()
{
    ClearWidget();
}

/*!
 * 派生クラス用
 * @author teco
 * @since 2014.12.05
 */
void UiGamePadConfigWindow::OnUpdate( TbFloat32 deltasec  )
{
    Super::OnUpdate(deltasec);
    if( !IsOpened() ) {
        return;
    }

    TbBool isEnableNext = TB_FALSE;

    static const TbPadButton BUTTON_TABLE[] = 
    {
        TB_PAD_BUTTON_MAX , 
        TB_PAD_A , 
        TB_PAD_B ,
        TB_PAD_START
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(BUTTON_TABLE)==ITEM_MAX);

    // フォーカス遷移
    switch( m_FocusIndex )
    {
    case ITEM_DEVICE:
        {
            TbUint32 count = TbPad::GetPadCount();
            for( TbUint32 i = 0; i < count; ++i )
            {
                TbUint32 buttonCount = TbPad::GetRawButtonCount(i);
                for( TbUint32 key = 0; key < buttonCount; ++key )
                {
                    if( TbPad::IsPressRaw(key ,i) ){
                        static const TbUint32 P_NAME_MAX = 17;
                        m_Item[m_FocusIndex].input = i;
                        const char* str = TbPad::GetPadProductName(i);
                        if( !str ) {
                            str = "";
                        }
                        TbStaticString128 name = str;
                        if( name.GetCharCount() > P_NAME_MAX ) {
                            name = name.Substr(0,P_NAME_MAX);
                            name += "...";
                        }
                        m_Item[m_FocusIndex].text.SetText( name.GetBuffer() );
                        isEnableNext = TB_TRUE;
                        break;
                    }
                }
                if( isEnableNext ){
                    break;
                }
            }
            if( !isEnableNext )
            {
#if CMN_ENABLE_REMOTE_INPUT
                CrRemoteInputServer& netInput = CrRemoteInputServer::GetInstance();
                count = netInput.GetPadCount();;
                for( TbUint32 i = 0; i < count; ++i )
                {
                    for( TbUint32 button = 0; button < TB_PAD_BUTTON_MAX; ++button )
                    {
                        if( netInput.IsPressRaw(TB_BIT(button),i) ){
                            m_Item[m_FocusIndex].input = TECO_GAME_PAD_MASK + i;
                            TbStaticString128 name = "TecoGamePad";
                            m_Item[m_FocusIndex].text.SetText( name.GetBuffer() );
                            isEnableNext = TB_TRUE;
                            break;
                        }
                    }
                }
                if( isEnableNext ){
                    break;
                }
#endif
            }
        }
        break;
    case ITEM_START:
    case ITEM_B:
    case ITEM_A:
        {
            TbUint32 padIndex = m_Item[ITEM_DEVICE].input;
            if( (padIndex & TECO_GAME_PAD_MASK) == TECO_GAME_PAD_MASK  )
            {
#if CMN_ENABLE_REMOTE_INPUT
                CrRemoteInputServer& netInput = CrRemoteInputServer::GetInstance();
                padIndex &= 0xFF;
                for( TbUint32 key = 0; key < TB_ARRAY_COUNT_OF(TECO_GAME_PAD_BUTTON_TABLE); ++key )
                {
                    if( m_Item[ITEM_START].input == key ||
                        m_Item[ITEM_B].input == key ||
                        m_Item[ITEM_A].input == key )
                    {
                        // 多重設定はできない
                        continue;
                    }
                    if( netInput.IsPressRaw( TB_BIT(TECO_GAME_PAD_BUTTON_TABLE[key]) , padIndex ) ){
                        m_Item[m_FocusIndex].input = TECO_GAME_PAD_BUTTON_TABLE[key];
                        TbStaticString32 keyName = TbStaticString32::Format("%d",key);
                        m_Item[m_FocusIndex].text.SetText( keyName.GetBuffer() );
                        isEnableNext = TB_TRUE;
                        break;
                    }
                }
#endif
            }
            else
            {
                TbUint32 buttonCount = TbPad::GetRawButtonCount(padIndex);
                for( TbUint32 key = 0; key < buttonCount; ++key )
                {
                    if( m_Item[ITEM_START].input == key ||
                        m_Item[ITEM_B].input == key ||
                        m_Item[ITEM_A].input == key )
                    {
                        // 多重設定はできない
                        continue;
                    }
                    if( TbPad::IsPressRaw( key , padIndex ) ){
                        m_Item[m_FocusIndex].input = key;
                        TbStaticString32 keyName = TbStaticString32::Format("%d",key);
                        m_Item[m_FocusIndex].text.SetText( keyName.GetBuffer() );
                        isEnableNext = TB_TRUE;
                        break;
                    }
                }
            }
        }
        break;
    };
    if( isEnableNext ) {
        if( m_FocusIndex < TB_ARRAY_COUNT_OF(m_Item)-1 ){
            ++m_FocusIndex;
            switch( m_Item[m_FocusIndex].type )
            {
            case Item::TYPE_TEXT:
                m_Item[m_FocusIndex].text.SetFocus();
            case Item::TYPE_BUTTON:
                m_Item[m_FocusIndex].button.SetFocus();
            }
        }else{
            m_Result = RESULT_OK;

            // 反映
            TbUint32 padIndex = m_Item[ITEM_DEVICE].input;
#if CMN_ENABLE_REMOTE_INPUT
            if( (padIndex & TECO_GAME_PAD_MASK) == TECO_GAME_PAD_MASK  )
            {
                CrRemoteInputServer& netInput = CrRemoteInputServer::GetInstance();
                for( TbSint32 i = ITEM_BUTTON_BEGIN; i < ITEM_BUTTON_END; ++i )
                {
                    netInput.SetPadButtonSetting( BUTTON_TABLE[i] , m_Item[i].input );
                }
            }
            else
#endif
            {
                for( TbSint32 i = ITEM_BUTTON_BEGIN; i < ITEM_BUTTON_END; ++i )
                {
                    TbPad::SetPadButtonSetting( padIndex , BUTTON_TABLE[i] , m_Item[i].input );
                }
            }

            // Root::GetInstance().SaveUserData();
            Close();
        }
    }
}

/*!
 * 生成時
 * @author teco
 * @since 2014.12.15
 */
void UiGamePadConfigWindow::OnCreated()
{
    Super::OnCreated();
    m_FocusIndex = 0;
    m_Item[m_FocusIndex].text.SetFocus();
    m_Result = RESULT_NONE;
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Item); ++i )
    {
        m_Item[i].input = -1;
        switch( m_Item[i].type )
        {
        case Item::TYPE_TEXT:
            m_Item[i].text.SetText("PUSH ANY PAD BUTTON");
            break;
        case Item::TYPE_BUTTON:
            break;
        }
    }
    CrInput::SetEnablePad(TB_FALSE);
#if CMN_ENABLE_REMOTE_INPUT
    CrRemoteInputServer::GetInstance().SetEnableInput(TB_FALSE);
#endif
}

/*! 
 * 閉じた
 * @author teco
 */
void UiGamePadConfigWindow::OnClosed()
{
    CrInput::SetEnablePad(TB_TRUE);
#if CMN_ENABLE_REMOTE_INPUT
    CrRemoteInputServer::GetInstance().SetEnableInput(TB_TRUE);
#endif
    if( RESULT_NONE == m_Result ){
        m_Result = RESULT_CANCEL;
    }
}


/*! 
 * 自動リサイズ
 * @author teco
 * @since 2014.12.18
 */
void UiGamePadConfigWindow::fit()
{
    TbFloat32 width  = 0.0f;
}
