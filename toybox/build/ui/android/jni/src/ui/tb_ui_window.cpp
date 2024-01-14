/*!
 * ポーズメニュー
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_window.h"
#include "ui/tb_ui_widget.h"
#include "ui/tb_ui_manager.h"

namespace toybox
{

static const TbFloat32 BASE_SIZE = 32.0f;
static const TbFloat32 WIDTH_MIN = BASE_SIZE*2.0f;
static const TbFloat32 HEIGHT_MIN = BASE_SIZE*2.0f;

static const TbFloat32 CLIENT_RECT_MARGIN_X =24.0f;
static const TbFloat32 CLIENT_RECT_MARGIN_Y_TITLE = 16.0f + BASE_SIZE;
static const TbFloat32 CLIENT_RECT_MARGIN_Y = 24.0f;

/*! 
 * コンストラクタ
 * @author teco
 */
TbUiWindow::TbUiWindow( TbBool hasTitle )
    : m_PlayerBit(1)
    , m_SpriteOffsetZ(0.0f)
    , m_CloseSec(-1.0f)
{
    m_BitArray.SetBit(FLAG_HAS_TITLE_BAR,hasTitle);
    m_BitArray.SetBit(FLAG_ENABLE_INPUT);

    static const TbRectF32Pod SPRITE_RECT = 
    {
        0.0f , 0.0f , WIDTH_MIN , HEIGHT_MIN ,
    };
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(BASE_SIZE,BASE_SIZE);
    spriteParam.rect = SPRITE_RECT;
    m_DefaultUv = *TbUiManager::GetInstance().GetUV(TB_UI_UV_WINDOW);
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_DefaultUv.rectUV); ++i ) {
        spriteParam.rectUV[i] = m_DefaultUv.rectUV[i];
    }
    m_Sprite.Initialize(spriteParam);
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_1);

    // 閉じるボタン
    static const TbRectF32Pod SPRITE_CLOSE_RECT = 
    {
        0.0f , 0.0f , BASE_SIZE , BASE_SIZE ,
    };
    m_CloseSprite.Initialize(SPRITE_CLOSE_RECT,*TbUiManager::GetInstance().GetUVRect(TB_UI_UV_RECT_WINDOW_CLOSE));
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_2);

    m_MainWidget.Enter(this);
    SetFocus(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiWindow::~TbUiWindow()
{
    m_MainWidget.ClearWidget();
    m_MainWidget.Exit();
    Close();
    TbUiManager& manager = TbUiManager::GetInstance();
    manager.RemoveWindow(this);
}

/*!
 * 開く
 * @author teco
 */
void TbUiWindow::Open()
{
    if( !m_BitArray.Test(FLAG_OPEN) ){
        OnCreating();
        TbUiManager& manager = TbUiManager::GetInstance();
        manager.PlaySE(TB_UI_SE_OPEN);
        manager.AddWindow(this);
        m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_1 + m_SpriteOffsetZ);
        m_CloseSprite.SetZ(TB_UI_SPRITE_Z_FRONT_2 + m_SpriteOffsetZ);
        m_BitArray.SetBit(FLAG_OPEN);
        m_BitArray.SetBit(FLAG_FIRST);
    }
}

/*!
 * 閉じる
 * @author teco
 */
void TbUiWindow::Close()
{
    if( m_BitArray.Test(FLAG_OPEN) ){
        // ウィジェット破棄
        m_MainWidget.Delete();
        TbUiManager& manager = TbUiManager::GetInstance();
        manager.RemoveWindow(this);
        m_CloseSprite.Exit();
        m_Sprite.Exit();
        m_BitArray.ResetBit(FLAG_OPEN);
        OnClosed();
    }
}

/*!
 * 定期処理
 * @author teco
 */
void TbUiWindow::Update( TbFloat32 deltasec )
{
    if( m_BitArray.Test(FLAG_FIRST) ){
        TbUiManager& manager = TbUiManager::GetInstance();
        m_Sprite.Enter(&manager.GetSpriteScene());
        m_CloseSprite.Enter(&manager.GetSpriteScene());
        m_MainWidget.Create();
        OnCreated();
        m_BitArray.ResetBit(FLAG_FIRST);
    }
    if (m_BitArray.Test(FLAG_AUTO_RESIZABLE) && m_MainWidget.IsEmptyChild()) {
        fit();
    }

    m_MainWidget.Update(deltasec);    
    OnUpdate(deltasec);
    m_MainWidget.Post(deltasec);

    // Closeボタンを押した
    // ボタン入力は上位層で行ってる（要検討)
    // ここではマウスのみ
    if( IsEnableCloseButton() ){
        TbRectF32 rect;
        TbVector2 pos =m_CloseSprite.GetTranslate();
        rect.x = pos.GetX();
        rect.y = pos.GetY();
        rect.width = BASE_SIZE;
        rect.height = BASE_SIZE;
        TbUiManager& manager = TbUiManager::GetInstance();
        if( manager.IsAnyoneCursorPress(rect,TB_FALSE) ){
            manager.PlaySE(TB_UI_SE_CANCEL);
            Close();
            return;
        }
    }

    // リクエスト
    if( 0.0f <= m_CloseSec ){
        m_CloseSec -= deltasec;
        if( m_CloseSec < 0.0f ){
            Close();
            return;
        }
    }
}

/*!
 * 描画処理
 * @author teco
 */
void TbUiWindow::Draw()
{
    if (!m_BitArray.Test(FLAG_FIRST))
    {
        m_MainWidget.Draw();
        OnDraw();
    }
}

/*!
 * クライアント座標設定
 * @author teco
 */
void TbUiWindow::SetClientPos( const TbVector2& pos )
{
    TbVector2 result = pos;
    result.Add(-CLIENT_RECT_MARGIN_X,0.0f);
    if( HasTitleBar() ){
        result.Add(0.0f,-CLIENT_RECT_MARGIN_Y_TITLE);
    }else{
        result.Add(0.0f,-CLIENT_RECT_MARGIN_Y);
    }
    SetPos(result);
}

/*!
 * クライント領域取得
 * @author teco
 */
TbRectF32   TbUiWindow::GetClientRegion() const
{
    TbRectF32 rect = GetRegion();
    rect.x += CLIENT_RECT_MARGIN_X;
    rect.width -= CLIENT_RECT_MARGIN_X*2;
    if( HasTitleBar() ){
        rect.y += CLIENT_RECT_MARGIN_Y_TITLE;
        rect.height -= CLIENT_RECT_MARGIN_Y_TITLE;
    }else{
        rect.y += CLIENT_RECT_MARGIN_Y;
        rect.height -= CLIENT_RECT_MARGIN_Y;
    }
    rect.height -= CLIENT_RECT_MARGIN_Y;
    return rect;
}

/*!
 * クライアントサイズ設定
 * @author teco
 */
void TbUiWindow::SetClientSize( TbFloat32 width , TbFloat32 height )
{
    TbFloat32 w = width + CLIENT_RECT_MARGIN_X*2;
    TbFloat32 h = height +CLIENT_RECT_MARGIN_Y;
    if( HasTitleBar() ){
        h += CLIENT_RECT_MARGIN_Y_TITLE;
    }else{
        h += CLIENT_RECT_MARGIN_Y;
    }
    SetSize(w,h);
}

/*!
 * クライアントサイズ設定
 * @author teco
 */
void TbUiWindow::SetClientWidth( TbFloat32 width )
{
    TbFloat32 w = width + CLIENT_RECT_MARGIN_X*2;
    SetWidth(w);
}

/*!
 * クライアントサイズ設定
 * @author teco
 */
void TbUiWindow::SetClientHeight( TbFloat32 height )
{
    TbFloat32 h = height +CLIENT_RECT_MARGIN_Y;
    if( HasTitleBar() ){
        h += CLIENT_RECT_MARGIN_Y_TITLE;
    }else{
        h += CLIENT_RECT_MARGIN_Y;
    }
    SetHeight(h);
}

/*!
 * サイズ設定
 * @author teco
 */
void TbUiWindow::SetSize( TbFloat32 width , TbFloat32 height )
{
    TbFloat32 sprWidth = TbMax(width,WIDTH_MIN);
    TbFloat32 sprHeight = TbMax(height,HEIGHT_MIN);
    m_Sprite.SetSize( sprWidth , sprHeight );
    updatePos();
}

/*!
 * サイズ設定
 * @author teco
 */
void TbUiWindow::SetWidth( TbFloat32 width )
{
    TbFloat32 sprWidth = TbMax(width,WIDTH_MIN);
    m_Sprite.SetWidth(sprWidth);
    updatePos();
}

/*!
 * サイズ設定
 * @author Miyake_Shunsuke
 * @since 2014.12.09
 */
void TbUiWindow::SetHeight( TbFloat32 height )
{
    TbFloat32 sprHeight = TbMax(height,HEIGHT_MIN);
    m_Sprite.SetHeight( sprHeight );
    updatePos();
}

/*!
 * 所有者設定
 * @author Miyake_Shunsuke
 * @since 2014.12.27
 */
void TbUiWindow::SetPlayerBit( TbUint32 bit )
{
    if( m_PlayerBit == bit ) {
        return;
    }
    m_PlayerBit = bit;
    if( IsFullscreen() ){
        return;
    }
    OnPlayerBitChanged(bit);
    m_MainWidget.NotifyPlayerBitChanged(bit);
}

/*!
 * フォーカス設定
 * @author Miyake_Shunsuke
 * @since 2014.12.27
 */
void TbUiWindow::SetFocus( TbBool isFocus )
{
    m_BitArray.SetBit(FLAG_FOCUS,isFocus);
}

/*! 
 * フォーカスが当たっているウィジェット取得
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
TbUiWidget* TbUiWindow::GetFocusedWidget()
{
    return m_MainWidget.GetFocusedWidget();
}

/*!
 * フォーカスが当たっているウィジェット取得
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
const TbUiWidget* TbUiWindow::GetFocusedWidget() const
{
    return m_MainWidget.GetFocusedWidget();
}

/*!
 * フルスクリーン設定
 * @author Miyake_Shunsuke
 * @since 2014.12.19
 */
void TbUiWindow::SetFullscreen( TbBool isFullscreen )
{
    if( TB_BOOL_NOT_EQUAL(IsFullscreen(),isFullscreen) )
    {
        m_BitArray.SetBit(FLAG_FULLSCREEN,isFullscreen);

        const TbElasticSprite::UvParam* uv = nullptr;
        if( isFullscreen ) {
            SetSize(TbUiManager::GetInstance().GetFullscreenSize());
            uv = TbUiManager::GetInstance().GetUV(TB_UI_UV_WINDOW_FULLSCREEN);
        }else{
            // 復帰コード書け
            uv = &m_DefaultUv;
        }
        m_Sprite.SetUV(*uv);
        m_CloseSprite.SetVisible(!isFullscreen);
        OnFullscreenChanged(isFullscreen);
    }
}

/*!
 * ウィジェットが入れるように
 * @author Miyake_Shunsuke
 * @since 2014.12.08
 */
void TbUiWindow::fit()
{
    // 自動リサイズ機能
    TbRectF32 clientRegion = TbUiWindow::GetClientRegion();
    TbRectF32 region = m_MainWidget.GetCoverageGlobalRegion();
    TB_RESQUE(  clientRegion.x<=region.x&&
                region.x+region.width<=clientRegion.x+clientRegion.width&&
                clientRegion.y<=region.y&&
                region.y+region.height<=clientRegion.y+clientRegion.height);
    SetClientPos( TbVector2(region.x,region.y) );
    SetClientSize( region.width , region.height );
}

/*!
 * ウィジェット追加
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
void TbUiWindow::addWidget( TbUiWidget* widget )
{
    TB_RESQUE(widget);
    TB_RESQUE(widget->GetWindow()==this);
    m_MainWidget.addWidget(widget);
}

/*! 
 * ウィジェット除去
 * @author Miyake_Shunsuke
 * @since 2014.12.06
 */
void TbUiWindow::removeWidget( TbUiWidget* widget )
{
    TB_RESQUE(widget);
    TB_RESQUE(widget->GetWindow()==this);
    m_MainWidget.removeWidget(widget);
}

/*!
 * 座標更新
 * @author Miyake_Shunsuke
 * @since 2014.12.17
 */
void TbUiWindow::updatePos()
{
    if( IsFullscreen() ){
        m_Pos.Clear();
        m_Sprite.SetTranslate(0.0f,0.0f);
    }else{
        TbVector2 pos = m_Pos;
        const TbDimensionF32& size = GetSize();
        switch (m_Align.x)
        {
        case TB_UI_ALIGN_X_CENTER:
            pos.Add(-size.width/2.0f,0.0f);
            break;
        case TB_UI_ALIGN_X_RIGHT:
            pos.Add(-size.width,0.0f);
            break;
        }
        switch (m_Align.y)
        {
        case TB_UI_ALIGN_Y_CENTER:
            pos.Add(0.0f,-size.height/2.0f);
            break;
        case TB_UI_ALIGN_Y_BOTTOM:
            pos.Add(0.0f,-size.height);
            break;
        }
        m_Sprite.SetTranslate(pos);

        TbVector2 closePos = pos;
        closePos.Add(size.width-BASE_SIZE,0.0f);
        m_CloseSprite.SetTranslate(closePos);
    }
}

}
