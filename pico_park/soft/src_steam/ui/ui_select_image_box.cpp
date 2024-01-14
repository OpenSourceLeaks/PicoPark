/*!
 * UI用選択式イメージボックス
 * @author teco
 */

#include "fwd.h"
#include "ui/ui_select_image_box.h"
#include "system/sys_input.h"
#include <framework/sub/tb_framework_simple.h>
#include <ui/tb_ui_window.h>
#include "root.h"

// UV
static const TbFloat32 SPACE_WIDTH = 8.0f;
static const TbFloat32 CLOSING_SEC = 0.3f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.20
 */
UiSelectImageBox::UiSelectImageBox()
    : m_Index(0)
    , m_ChangingTimer(0.0f)
{
    m_Items.SetEnableSetCapacityAuto(TB_TRUE);

    UiArrowButtonSkin::Type TYPE_TABLE[]
    {
        UiArrowButtonSkin::TYPE_LEFT , 
        UiArrowButtonSkin::TYPE_RIGHT , 
    };
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
        m_Arrow[i].Initilaize(TYPE_TABLE[i]);
        m_Arrow[i].SetBasePos(TbUiWidget::BASE_POS_PARENT_GLOBAL_CENTER);
        m_Arrow[i].SetAlignY(TB_UI_ALIGN_Y_CENTER);
        m_Arrow[i].SetEnable(true);
        m_Arrow[i].SetEnablePress(TB_UI_INPUT_DEVICE_BUTTON,TB_FALSE);
        m_Arrow[i].SetPressedCallback( TbBind(&Self::onPressArrow,this,i==ARROW_RIGHT_NAVI) );
    }
    m_Arrow[ARROW_LEFT_NAVI].SetAlignX(TB_UI_ALIGN_X_RIGHT);
    m_Arrow[ARROW_RIGHT_NAVI].SetAlignX(TB_UI_ALIGN_X_LEFT);

    m_ImageBox.SetBasePos(TbUiWidget::BASE_POS_PARENT_GLOBAL_CENTER);
    m_ImageBox.SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
    m_ImageBox.SetSize(44.0f,48.0f);
    m_ImageBox.SetEnableFrame(TB_TRUE);
    static const TbRectF32Pod PLAYER_UV_RECT = {CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f)};
    m_ImageBox.SetImage(nullptr,PLAYER_UV_RECT);

    SetEnableLoop(TB_TRUE);
}
    
/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.20
 */
UiSelectImageBox::~UiSelectImageBox()
{
}

/*! 
 * アイテム追加
 * @author teco
 * @since 2014.12.20
 */
void UiSelectImageBox::AddItem( const TbRectF32& uv )
{
    TB_RESQUE(!IsCreated());
    Item item;
    item.uv = uv;
    m_Items.PushBack(item);

    resize();
    m_Index = m_Items.GetCount()-1;
}

/*!
 * ウィンドウの所属になる
 * @author teco
 */
void UiSelectImageBox::OnEnter( TbUiWidget* widget )
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
        m_Arrow[i].Enter(this);
    }
    m_ImageBox.Enter(this);
}

/*!
 * ウィンドウの所属になる
 * @author teco
 * @since 2014.12.20
 */
void UiSelectImageBox::OnExit()
{
    m_ImageBox.Exit();
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
        m_Arrow[i].Exit();
    }
}

/*!
 * 定期処理
 * @author teco
 * @since 2014.12.20
 */
void UiSelectImageBox::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsCreated() ){
        return;
    }
    if( m_Items.IsEmpty() ){
        TB_ASSERT(0);
        return;
    }

    // 変更中時間
    if( m_ChangingTimer > 0.0f ) {
        m_ChangingTimer -= deltaSec;
        if( m_ChangingTimer <= 0.0f ){
            for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
                if( m_Arrow[i].IsFocused() && !m_Arrow[i].IsCursorOver() ){
                    m_ImageBox.SetFocus();
                }
            }
        }
    }else{
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
            if( m_Arrow[i].IsFocused() && !m_Arrow[i].IsCursorOver() ){
                m_ChangingTimer = CLOSING_SEC;
            }
        }
    }

    if( !IsFocused() ){
        return;
    }

    TbUiManager& manager = TbUiManager::GetInstance();

    // フォーカスが当たっていて、利用状態の場合はクリック処理
    if( manager.IsOnRepeat(TbUiInput::BUTTON_RIGHT,GetPlayerBit()) ){
        addIndex(TB_TRUE);
        m_Arrow[ARROW_RIGHT_NAVI].SetFocus();
    }else if( manager.IsOnRepeat(TbUiInput::BUTTON_LEFT,GetPlayerBit()) ){
        addIndex(TB_FALSE);
        m_Arrow[ARROW_LEFT_NAVI].SetFocus();
    }

    if( manager.IsPress(TbUiInput::BUTTON_DECIDE,GetPlayerBit()) ||
        manager.IsAnyoneCursorPress(m_ImageBox.GetGlobalRegion(),TB_FALSE) )
    {
        if( m_DecidedCallback.IsValid() ){
            m_DecidedCallback(m_Index);
        }
    }
}

/*!
 * 生成時コールバック
 * @author teco
 */
void UiSelectImageBox::OnCreated()
{
//    m_TextBox.SetText(GetSelectedString());
//    m_TextBox.SetFocus();
    resize();
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void UiSelectImageBox::OnDeleted()
{
    if( m_Items.IsEmpty() ){
        TB_ASSERT(0);
        return;
    }
}

/*!
 * フォーカス状態変更
 * @author teco
 */
void UiSelectImageBox::OnFocusChanged( TbBool isFocus , TbUiWidget* widget )
{
    if( !isFocus ) {
        m_ImageBox.SetFocus();
    }

    // 画像フレーム
    {
        TbElasticSprite::UvParam param;
        const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV( isFocus ? TB_UI_UV_TEXT_BOX_FOCUS : TB_UI_UV_TEXT_BOX );
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(uv->rectUV); ++i ) {
            param.rectUV[i] = uv->rectUV[i];
        }
        if( isFocus )
        {
            TbFloat32 offsetX = 0.0f;
            TbUint32 playerBit = GetPlayerBit();
            if( TbBitPopulation(playerBit) > 1 ) { // 複数立っている場合
                offsetX += CMN_TEX_POS(26.0f);
            }else{
                TbUint32 index = SysInput::GetPlayerPadIndex(TbBitLeastSignificant(playerBit));
                offsetX += CMN_TEX_POS(13.0f*index);
            }
            for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(param.rectUV); ++i ) {
                param.rectUV[i].x += offsetX;
            }
            m_ImageBox.SetFrameUV(param);
        }
        m_ImageBox.SetVisibleFrame(isFocus);
    }
}


/*!
 * リサイズ
 * @author teco
 */
void UiSelectImageBox::resize()
{
    TbFloat32 offsetX = m_ImageBox.GetWidth()/2.0f+16.0f;
    if( m_ImageBox.IsEnableFrame() ){
        offsetX += 8.0f;
    }
    m_Arrow[ARROW_LEFT_NAVI].SetPos(-offsetX,0.0f);
    m_Arrow[ARROW_RIGHT_NAVI].SetPos(offsetX,0.0f);

    TbFloat32 width = m_Arrow[ARROW_RIGHT_NAVI].GetGlobalRightBottomPos().GetX() - 
                      m_Arrow[ARROW_LEFT_NAVI].GetGlobalPos().GetX();
    SetSize(width,m_ImageBox.GetHeight());
}

/*!
 * インデックス加算
 * @author teco
 */
void UiSelectImageBox::addIndex( TbBool up )
{
    if( 1 == m_Items.GetCount() ) {
        return;
    }
    if( up ) {
        if( IsEnableLoop() ){
            m_Index = (m_Index + 1) % m_Items.GetCount();
        }else if( m_Index < m_Items.GetCount()-1 ){
            ++m_Index;
        }
    }else{
        if( 0 == m_Index ) {
            if( IsEnableLoop() ){
                m_Index = m_Items.GetCount()-1;
            }
        }else{
            m_Index--;
        }
    }
    m_ImageBox.SetImage(nullptr,m_Items[m_Index].uv);
    if( m_ChangedCallback.IsValid() ){
        m_ChangedCallback(m_Index);
    }
}

/*!
 * 矢印を押した
 * @author teco
 */
void UiSelectImageBox::onPressArrow( TbBool isRight )
{
    addIndex(isRight);
}
