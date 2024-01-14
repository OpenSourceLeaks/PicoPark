/*!
 * UIボタン
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_button.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"
#include <graphics/render/tb_draw_2d.h>

#include <input/tb_touch.h>

namespace toybox
{

static const TbFloat32 BASE_SIZE = 8.0f;
static const TbFloat32 WIDTH_MIN = BASE_SIZE*1.2f;
static const TbFloat32 HEIGHT_MIN = BASE_SIZE*1.2f;

static const TbFloat32 MARGIN_X =8.0f;
static const TbFloat32 MARGIN_Y = 8.0f;
static const TbFloat32 INPUT_DELAY_SEC = 0.1f;

/*!
 * コンストラクタ
 * @author teco
 */
TbUiButton::TbUiButton()
    : Super()
    , m_Phase(PHASE_IDLE)
    , m_FontSize(0)
    , m_InputDelaytimer(INPUT_DELAY_SEC)
    , m_TextAlignX(TB_UI_ALIGN_X_LEFT)
    , m_TextColor(TbColor::Black())
    , m_Skin(nullptr)
    , m_PressButton(TbUiInput::BUTTON_DECIDE)
    , m_PressPlayerIndex(-1)
{
    m_Skin = &m_DefaultSkin;
    m_Skin->SetOwner(this);
    TbRectF32 rect = m_Skin->GetRegion();
    SetSize( rect.width , rect.height );
    SetEnableResize(TB_TRUE);

    for( TbSint32 i = FLAG_ENABLE_PRESS_DEVICE_BEGIN; i < FLAG_ENABLE_PRESS_DEVICE_END; ++i ) {
        m_BitArray.SetBit(i);
    }
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiButton::~TbUiButton()
{
}

/*!
 * スキン設定
 * @author teco
 */
void TbUiButton::SetSkin( TbUiButtonSkin* skin )
{
    TB_RESQUE(!IsCreated()); // 生成前である必要あり
    if( m_Skin ) {
        m_Skin->SetOwner(nullptr);
    }
    if( skin ){
        m_Skin = skin;
    }else{
        m_Skin = &m_DefaultSkin;
    }
    if( m_Skin ) {
        m_Skin->SetOwner(this);
    }
    TbRectF32 rect = m_Skin->GetRegion();
    SetSize( rect.width , rect.height );
}

/*!
 * 生成時コールバック
 * @author teco
 */
void TbUiButton::OnCreated()
{
    m_Skin->OnCreated();
    m_InputDelaytimer = INPUT_DELAY_SEC;
    m_Phase = PHASE_IDLE;
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void TbUiButton::OnDeleted()
{
    m_Skin->OnDeleted();
}

/*!
 * 定期処理
 * @author teco
 */
void TbUiButton::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsCreated() ){
        return;
    }
    if( 0.0f < m_InputDelaytimer ) {
        m_InputDelaytimer -= deltaSec;
    }
    TbUiManager& manager = TbUiManager::GetInstance();
    TbBool isWindowFocus = GetWindow()->IsFocus();
    TbSint32 prePhase = m_Phase;
    if( IsEnable() )
    {
        switch( m_Phase )
        {
        case PHASE_IDLE:
            {
                m_Phase = PHASE_ACTIVE;
            }
            break;
        case PHASE_ACTIVE:
            {
                if( !isWindowFocus ) {
                    break;
                }

                if( !GetWindow()->IsEnableInput() ){
                    break;
                }

                TbUint32 cursorCount =  manager.GetCursorCount();
                for( TbUint32 i = 0; i < cursorCount; ++i )
                {
                    if( isCursorOver(i,TB_FALSE) )
                    {
                        if( (IsEnablePressCursorOver() || manager.IsCursorPress(i)) && IsEnablePress(TB_UI_INPUT_DEVICE_CURSOR) ) {
                            if( IsEnableFocusCursorOver() ){
                                SetBroadCastFocus();
                            }
                            if( m_PressedCallback.IsValid() ){
                                m_PressedCallback();
                            }
                            m_PressPlayerIndex = static_cast<TbSint32>(i);
                            OnPressChanged(TB_TRUE);
                            manager.PlaySE(TB_UI_SE_SELECT);
                            m_Phase = PHASE_PRESS_CURSOR;
                            break;
                        }else{
                            if( IsEnableFocusCursorOver() && manager.IsCursorMoving(i) ){
                                SetBroadCastFocus();
                            }
                        }
                    }
                }

                if( !IsFocused() ){
                    break;
                }

                // フォーカスが当たっていて、利用状態の場合はクリック処理
                if(m_BitArray.Test(FLAG_PRESS))
                {
                    if (m_PressedCallback.IsValid()) {
                        m_PressedCallback();
                    }
                    manager.PlaySE(TB_UI_SE_SELECT);
                    OnPressChanged(TB_TRUE);
                    m_Phase = PHASE_PRESS_KEY;
                }
            }
            break;
        case PHASE_PRESS_CURSOR:
            {
                TbBool isOn = TB_FALSE;
                TbUint32 cursorCount =  manager.GetCursorCount();
                for( TbUint32 i = 0; i < cursorCount; ++i )
                {
                    if( isCursorOver(i,TB_TRUE) && manager.IsCursorOn(i) ) {
                        isOn = TB_TRUE;
                        break;
                    }
                }
                if( !isWindowFocus ) {
                    isOn = TB_FALSE;
                }
                if( isOn ){
                    break;
                }
                if( m_ReleasedCallback.IsValid() ){
                    m_ReleasedCallback();
                    break;
                }
                OnPressChanged(TB_FALSE);
                m_Phase = PHASE_ACTIVE;
            }
            break;
        case PHASE_PRESS_KEY:
            {
                if( !manager.IsOn(m_PressButton,GetPlayerBit()) || !isWindowFocus ){
                    if( m_ReleasedCallback.IsValid() ){
                        m_ReleasedCallback();
                        break;
                    }
                    OnPressChanged(TB_FALSE);
                    m_Phase = PHASE_ACTIVE;
                }
            }
            break;
        }
    }
    else
    {
        m_BitArray.ResetBit(FLAG_CURSOR_OVER);
    }
    m_BitArray.ResetBit(FLAG_PRESS);
    if( PHASE_IDLE != prePhase && m_InputDelaytimer <= 0.0f)
    {
        TbUint32 inputPlayer = 0;
        if (manager.IsPress(m_PressButton, GetPlayerBit(),&inputPlayer) && IsEnablePress(TB_UI_INPUT_DEVICE_BUTTON)) {
            m_PressPlayerIndex = static_cast<TbSint32>(inputPlayer);
            m_BitArray.SetBit(FLAG_PRESS);
        }
    }
}

/*!
 * 定期処理
 * @author teco
 */
void TbUiButton::OnPost( TbFloat32 deltaSec )
{
    TbRectF32 globalRegion = GetGlobalRegion();
    m_Skin->OnPosChanged(TbVector2(globalRegion.x,globalRegion.y));
    if( m_BitArray.Test(FLAG_ENABLE_RESIZE) ){
        m_Skin->OnSizeChanged(globalRegion.width,globalRegion.height);
    }else{
        TbRectF32 rect = m_Skin->GetRegion();
        SetSize(rect.width,rect.height);
    }
}

/*!
 * 描画処理
 * @author teco
 */
void TbUiButton::OnDraw()
{
    if( !IsVisible() ){
        return;
    }
    TbUiManager& manager = TbUiManager::GetInstance();
    TbRectF32 globalRegion = GetGlobalRegion();

    // 文字表示
    if( !m_Text.IsEmpty() )
    {
        TbVector2 pos(globalRegion.x,globalRegion.y);
        TbFontBuffer2d* font = manager.GetFontBuffer();
        font->PushSetting();
        font->SetColor(m_TextColor);
        if( 0 < m_FontSize ){
            font->SetFontSize(m_FontSize);
        }
        
        switch( m_TextAlignX ) {
        case TB_UI_ALIGN_X_LEFT:
            pos.Add( MARGIN_X , MARGIN_Y );
            break;
        case TB_UI_ALIGN_X_RIGHT:
            pos.Add( GetRegion().width - m_TextSize.width , MARGIN_Y );
            break;
        case TB_UI_ALIGN_X_CENTER:
            pos.Add( GetRegion().width*0.5f - m_TextSize.width*0.5f , MARGIN_Y );
            break;
        }
        pos.Add(0.0f,-2.0f); // 暫定調整

        TbRectF32 region;
        font->Printf( region , 
                      static_cast<TbSint32>(pos.GetX()) , 
                      static_cast<TbSint32>(pos.GetY()) , 
                      m_Text.GetBuffer());
        font->PopSetting();
    }
#if 0
    TbDraw2dBegin();
    TbDrawRectParam param;
    param.SetRect( globalRegion );
    param.SetColor( TbColor::Red() );
//    param.SetZ(TB_UI_SPRITE_Z_FRONT_1);
    TbDrawRect(param);
    TbDraw2dEnd();
#endif
}

/*!
 * 有効
 * @author teco
 */
void TbUiButton::SetEnable( TbBool isEnable )
{
    if( TB_BOOL_NOT_EQUAL(IsEnable(),isEnable) ) {
        m_BitArray.SetBit(FLAG_ENABLE,isEnable);
        m_Skin->OnEnableChanged(isEnable);
    }
}

/*!
 * 有効
 * @author teco
 */
TbBool TbUiButton::IsCursorOver() const {
    TbBool isEnableCursorRadius = (m_Phase == PHASE_PRESS_CURSOR);
    TbUiManager& manager = TbUiManager::GetInstance();
    TbUint32 cursorCount =  manager.GetCursorCount();
    for( TbUint32 i = 0; i < cursorCount; ++i )
    {
        if( isCursorOver(i,isEnableCursorRadius) ) {
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 文字設定
 * @author teco
 */
void TbUiButton::SetText( const char* text )
{
    m_Text = text;
    TbFontBuffer2d* font = TbUiManager::GetInstance().GetFontBuffer();
    if( 0 < m_FontSize ){
        font->PushSetting();
        font->SetFontSize(m_FontSize);
    }
    TbRectF32 region;
    if( TB_SUCCEEDED(font->GetRegion( region , 0 , 0 ,
                                        m_Text.GetBuffer())) )
    {
        if( m_BitArray.Test(FLAG_ENABLE_RESIZE) ){
            m_Skin->OnSizeChanged(region.width+MARGIN_X*2.0f
                                 ,region.height+MARGIN_Y*2.0f);
            TbRectF32 rect = m_Skin->GetRegion();
            SetSize(rect.width,rect.height);
        }
        m_TextSize.width = region.width;
        m_TextSize.height = region.height;
    }
    if (0 < m_FontSize) {
        font->PopSetting();
    }
}

/*!
 * サイズを参照テキストを使って設定
 * @author teco
 */
void TbUiButton::SetSizeWithText( const char* text )
{
    TbFontBuffer2d* font = TbUiManager::GetInstance().GetFontBuffer();
    if (0 < m_FontSize) {
        font->PushSetting();
        font->SetFontSize(m_FontSize);
    }
    TbRectF32 region;
    if( TB_SUCCEEDED(font->GetRegion( region , 0 , 0 ,
                                        text)) )
    {
        if( m_BitArray.Test(FLAG_ENABLE_RESIZE) ){
            m_Skin->OnSizeChanged(region.width+MARGIN_X*2.0f
                                 ,region.height+MARGIN_Y*2.0f);
            TbRectF32 rect = m_Skin->GetRegion();
            SetSize(rect.width,rect.height);
        }
    }
    if (0 < m_FontSize) {
        font->PopSetting();
    }
}

/*!
 * フォーカス状態変更
 * @author teco
 */
void TbUiButton::OnFocusChanged( TbBool isFocus , TbUiWidget* focusedWidget )
{
    m_Skin->OnFocusChanged(isFocus);
}

/*!
 * 押された状態変更
 * @author teco
 */
void TbUiButton::OnPressChanged( TbBool isPress )
{
    m_Skin->OnPressChanged(isPress);
}

/*!
 * カーソルが上にあるか
 * @author teco
 */
TbBool TbUiButton::isCursorOver( TbUint32 index , TbBool isEnableCursorRadius ) const
{
    TbUiManager& manager = TbUiManager::GetInstance();
    TbRectF32 region = GetGlobalRegion();
    region.x -= m_Margin.left;
    region.width += m_Margin.left + m_Margin.right;
    region.y -= m_Margin.top;
    region.height += m_Margin.top + m_Margin.bottom;
    return manager.IsCursorOver(index,region,isEnableCursorRadius);
}

}
