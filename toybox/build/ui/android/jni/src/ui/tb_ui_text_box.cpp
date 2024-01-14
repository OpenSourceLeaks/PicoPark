/*!
 * UI用テキストボックス
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_text_box.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"

static const TbFloat32 BASE_SIZE = 8.0f;
static const TbFloat32 WIDTH_MIN = BASE_SIZE*1.2f;
static const TbFloat32 HEIGHT_MIN = BASE_SIZE*1.2f;

static const TbFloat32 MARGIN_X = 12.0f;
static const TbFloat32 MARGIN_Y = 12.0f;

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
TbUiTextBox::TbUiTextBox( TbBool hasFrame )
    : Super()
    , m_FontSize(0U)
    , m_TextAlignX(TB_UI_ALIGN_X_LEFT)
    , m_TextAlignY(TB_UI_ALIGN_Y_TOP)
    , m_TextColor(TbColor::Black())
{
    if( hasFrame ){
        SetEnableFrame();
    }
    SetEnableResize(TB_TRUE);
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
TbUiTextBox::~TbUiTextBox()
{
}

/*! 
 * 文字設定
 * @author Miyake_Shunsuke
 * @since 2014.12.07
 */
void TbUiTextBox::SetText( const char* text ) 
{
    m_Text = text;
    TbFontBuffer2d* font = TbUiManager::GetInstance().GetFontBuffer();
    TbRectF32 region;
    
    if( 0 < m_FontSize )
    {
        font->PushSetting();
        font->SetFontSize(m_FontSize);
    }
    if( TB_SUCCEEDED(font->GetRegion( region , 0 , 0 ,
                                      m_Text.GetBuffer())) )
    {
        if( IsEnableResize() )
        {
            if (m_FrameSprite.IsInitialized() ){
                m_FrameSprite.SetWidth(region.width+MARGIN_X*2.0f);
                m_FrameSprite.SetHeight(region.height+MARGIN_Y*2.0f);
                SetSize(m_FrameSprite.GetWidth(),m_FrameSprite.GetHeight());
            }else{
                SetSize(region.width,region.height);
            }
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
 * @author Miyake_Shunsuke
 * @since 2014.12.07
 */
void TbUiTextBox::SetSizeWithText( const char* text )
{
    TbFontBuffer2d* font = TbUiManager::GetInstance().GetFontBuffer();
    TbRectF32 region;
    if (0 < m_FontSize)
    {
        font->PushSetting();
        font->SetFontSize(m_FontSize);
    }
    if( TB_SUCCEEDED(font->GetRegion( region , 0 , 0 ,
                                      text)) )
    {
        if( IsEnableResize() )
        {
            if (m_FrameSprite.IsInitialized() ){
                m_FrameSprite.SetWidth(region.width+MARGIN_X*2.0f);
                m_FrameSprite.SetHeight(region.height+MARGIN_Y*2.0f);
                SetSize(m_FrameSprite.GetWidth(),m_FrameSprite.GetHeight());
            }else{
                SetSize(region.width,region.height);
            }
        }
        m_TextSize.width = region.width;
        m_TextSize.height = region.height;
    }
    if (0 < m_FontSize)
    {
        font->PopSetting();
    }
}

/*!
 * フレーム有効
 * @author teco
 */
void TbUiTextBox::SetEnableFrame()
{
    TB_RESQUE(!IsCreated());
    if( !m_FrameSprite.IsInitialized() ){
        // スプライト設定
        static const TbRectF32Pod SPRITE_RECT = 
        {
            0.0f , 0.0f , WIDTH_MIN , HEIGHT_MIN ,
        };
        TbElasticSprite::InitParam spriteParam;
        spriteParam.basePlaceSize = TbDimensionF32(BASE_SIZE,BASE_SIZE);
        spriteParam.rect = SPRITE_RECT;
        const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV(TB_UI_UV_TEXT_BOX);
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(uv->rectUV); ++i ) {
            spriteParam.rectUV[i] = uv->rectUV[i];
        }
        m_FrameSprite.Initialize(spriteParam);
        m_FrameSprite.SetZ(TB_UI_SPRITE_Z_FRONT_2);
        SetSize( m_FrameSprite.GetWidth() , m_FrameSprite.GetHeight() );
    }
}

/*!
 * 生成時コールバック
 * @author teco
 */
void TbUiTextBox::OnCreated()
{
    if( m_FrameSprite.IsInitialized() )
    {
        TbUiManager& manager = TbUiManager::GetInstance();
        m_FrameSprite.SetZ(TB_UI_SPRITE_Z_FRONT_2+GetWindow()->GetSpriteOffsetZ());
        m_FrameSprite.Enter(&manager.GetSpriteScene());
        m_FrameSprite.SetTranslate(GetGlobalPos());
        m_FrameSprite.SetSize( GetRegion().width , GetRegion().height );
    }
}

/*!
 * 破棄時コールバック
 * @author Miyake_Shunsuke
 */
void TbUiTextBox::OnDeleted()
{
    if( m_FrameSprite.IsInitialized() ){
        m_FrameSprite.Exit();
    }
}


/*!
 * 定期処理
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
void TbUiTextBox::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsCreated() ){
        return;
    }

    if( m_FrameSprite.IsInitialized() ){
        m_FrameSprite.SetTranslate(GetGlobalPos());
        m_FrameSprite.SetSize( GetRegion().width , GetRegion().height );
    }
}

/*!
 * 描画処理
 * @author teco
 */
void TbUiTextBox::OnDraw()
{
    if (!IsCreated()) {
        return;
    }
    if (!IsVisible()) {
        return;
    }
    // 文字表示
    if (!m_Text.IsEmpty())
    {
        TbVector2 pos = GetGlobalPos();
        TbRectF32 region;
        TbFontBuffer2d* font = TbUiManager::GetInstance().GetFontBuffer();
        font->PushSetting();
        font->SetColor(m_TextColor);
        if (0 < m_FontSize) {
            font->SetFontSize(m_FontSize);
        }

        switch (m_TextAlignX) {
        case TB_UI_ALIGN_X_LEFT:
            font->SetAlignX(TbFontBuffer2d::ALIGN_X_LEFT);
            pos.Add(m_FrameSprite.IsInitialized() ? MARGIN_X : 0.0f, 0.0f);
            break;
        case TB_UI_ALIGN_X_RIGHT:
            font->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
            pos.Add(GetRegion().width, 0.0f);
            pos.Add(m_FrameSprite.IsInitialized() ? -MARGIN_X : 0.0f, 0.0f);
            break;
        case TB_UI_ALIGN_X_CENTER:
            font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
            pos.Add(GetRegion().width*0.5f, 0.0f);
            break;
        }
        switch (m_TextAlignY) {
        case TB_UI_ALIGN_Y_TOP:
            font->SetAlignY(TbFontBuffer2d::ALIGN_Y_UP);
            pos.Add(0.0f, m_FrameSprite.IsInitialized() ? MARGIN_Y : 0.0f);
            break;
        case TB_UI_ALIGN_Y_BOTTOM:
            font->SetAlignY(TbFontBuffer2d::ALIGN_Y_DOWN);
            pos.Add(0.0f, GetRegion().height);
            pos.Add(0.0f, m_FrameSprite.IsInitialized() ? -MARGIN_Y : 0.0f);
            break;
        case TB_UI_ALIGN_Y_CENTER:
            font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
            pos.Add(0.0f, GetRegion().height*0.5f);
            break;
        }

        if (m_FrameSprite.IsInitialized())
        {
            pos.Add(0.0f, -2.0f); // 暫定調整
        }
        font->Printf(region,
            static_cast<TbSint32>(pos.GetX()),
            static_cast<TbSint32>(pos.GetY()),
            m_Text.GetBuffer());
        font->PopSetting();
    }
}

/*!
 * フォーカス状態変更
 * @author teco
 */
void TbUiTextBox::OnFocusChanged( TbBool isFocus , TbUiWidget* focusedWidget )
{
    if( m_FrameSprite.IsInitialized() && !IsEnableManualFocusFrame() ){
        SetFocusFrame(isFocus);
    }
}

/*!
 * フレームのフォーカス表示の手動
 * @author teco
 */
void TbUiTextBox::SetFocusFrame( TbBool isFocus )
{
    if( m_FrameSprite.IsInitialized() )
    {
        TbElasticSprite::UvParam param;
        const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV( isFocus ? TB_UI_UV_TEXT_BOX_FOCUS : TB_UI_UV_TEXT_BOX );
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(uv->rectUV); ++i ) {
            param.rectUV[i] = uv->rectUV[i];
        }
        m_FrameSprite.SetUV(param);
    }
}

}