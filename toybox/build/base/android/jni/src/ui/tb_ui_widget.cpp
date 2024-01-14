/*!
 * UIウィジェット
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_widget.h"
#include "ui/tb_ui_window.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbUiWidget::TbUiWidget()
    : m_BasePos(BASE_POS_BEGIN)
    , m_Window(nullptr)
    , m_Parent(nullptr)
{
    m_BitArray.SetBit(FLAG_VISIBLE);
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiWidget::~TbUiWidget()
{
    // インスタンス破棄される前にウィンドウから除去されている必要がある
    TB_ASSERT(!m_Window);
    TB_ASSERT(!m_Parent);
}

/*!
 * 生成
 * @author teco
 */
void TbUiWidget::Create() 
{
    TbUiWindow* window = GetWindow();
    TB_RESQUE(window&&window->IsOpened());
    OnCreated();
    m_BitArray.SetBit(FLAG_CREATE);

    // ウィジェット生成
    if( !m_Widgets.IsEmpty() ){
        TbForIterator( it , m_Widgets ) {
            (*it)->Create();
        }
        if( IsEnableMultiFocus() ){
            TbForIterator( it , m_Widgets ) {
                (*it)->NotifyFocusChanged(TB_TRUE,*it);
            }
        }else{
            m_Widgets[0]->NotifyFocusChanged(TB_TRUE,m_Widgets[0]);
        }
    }
}

/*!
 * 削除
 * @author teco
 */
void TbUiWidget::Delete()
{
    if( !IsCreated() ){
        return;
    }

    // ウィジェット破棄
    if( !m_Widgets.IsEmpty() ){
        TbForIterator( it , m_Widgets ) {
            (*it)->Delete();
        }
    }

    TbUiWindow* window = GetWindow();
    TB_RESQUE(window&&window->IsOpened());

    OnDeleted();
    m_BitArray.ResetBit(FLAG_CREATE);
}

/*!
 * ウィンドウの所属になる
 * @author teco
 */
void TbUiWidget::Enter( TbUiWindow* window )
{
    TB_RESQUE(!m_Parent&&window);
    if( this == window->GetMainWidget() ){
        // 特殊ルート処理
        m_Window = window;
    }else{
        Enter( window->GetMainWidget() );
    }
}

/*!
 * ウィンドウの所属になる
 * @author teco
 */
void TbUiWidget::Enter( TbUiWidget* widget )
{
    TB_RESQUE(widget);
    TB_RESQUE(!IsRoot());
    m_Parent = widget;
    widget->addWidget(this);
    TbUiWindow* window = GetWindow();
    OnEnter(widget);
    if( window && window->IsOpened() ){
        Create();
    }
}

/*!
 * ウィンドウの所属から外れる
 * @author teco
 */
void TbUiWidget::Exit()
{
    OnExit();
    if( IsCreated() ){
        Delete();
    }
    if( m_Parent ){
        m_Parent->removeWidget(this);
    }
    m_Parent = nullptr;
    m_Window = nullptr;
}

/*!
 * 可視情報
 * @author teco
 */
TbBool TbUiWidget::IsVisible() const {
    const TbUiWindow* window = GetWindow();
    if( window && !window->IsVisible() ){
        return TB_FALSE;
    }
    return m_BitArray.Test(FLAG_VISIBLE);
}

/*!
 * フォーカスウィジェット設定
 * @author teco
 */
void TbUiWidget::SetFocusedWidget( TbUiWidget* widget )
{
    TB_RESQUE(widget);
    if( widget->IsFocused() ){
        return;
    }
    WidgetList::Iterator it = TbFind(m_Widgets,widget);
    if( TB_VERIFY(m_Widgets.End() != it) ){
        if( !IsEnableMultiFocus() ) {
            m_Widgets.GetFront()->NotifyFocusChanged(TB_FALSE,widget);
        }
        m_Widgets.Erase(it);
        m_Widgets.Insert(m_Widgets.Begin(),widget);
        if( !IsEnableMultiFocus() ) {
            widget->NotifyFocusChanged(TB_TRUE,widget);
        }
    }
}

/*!
 * フォーカスが当たっているウィジェット取得
 * @author teco
 */
TbUiWidget* TbUiWidget::GetFocusedWidget()
{
    if( m_Widgets.IsEmpty() ){
        return nullptr;
    }
    return m_Widgets.GetFront();
}

/*!
 * フォーカスが当たっているウィジェット取得
 * @author teco
 */
const TbUiWidget* TbUiWidget::GetFocusedWidget() const
{
    if( m_Widgets.IsEmpty() ){
        return nullptr;
    }
    return m_Widgets.GetFront();
}

/*!
 * 所属しているウィジェットをクリア
 * @author teco
 */
void TbUiWidget::ClearWidget()
{
    while( !m_Widgets.IsEmpty() ) {
        m_Widgets.GetFront()->ClearWidget();
        m_Widgets.GetFront()->Exit();
    }
}

/*!
 * グローバル座標取得
 * @author teco
 */
TbVector2 TbUiWidget::GetGlobalPos() const
{
    TbVector2 result(m_Region.x,m_Region.y);
    const TbUiWindow* window = GetWindow();
    if( window ) {
        TbRectF32 clientRegion = window->GetClientRegion();
        BasePos basePos = m_BasePos;
        if( BASE_POS_PARENT_BASE_POS == m_BasePos && m_Parent ) {
            basePos = m_Parent->GetBasePos();
        }
        switch ( basePos )
        {
        case BASE_POS_LEFT_TOP:
            result.Add( clientRegion.x , clientRegion.y );
            break;
        case BASE_POS_LEFT_CENTER:
            result.Add( clientRegion.x , clientRegion.y + clientRegion.height/2.0f );
            break;
        case BASE_POS_LEFT_DOWN:
            result.Add( clientRegion.x , clientRegion.y + clientRegion.height );
            break;
        case BASE_POS_CENTER_TOP:
            result.Add( clientRegion.x+clientRegion.width/2.0f , clientRegion.y );
            break;
        case BASE_POS_CENTER:
            result.Add( clientRegion.x+clientRegion.width/2.0f , clientRegion.y + clientRegion.height/2.0f );
            break;
        case BASE_POS_CENTER_DOWN:
            result.Add( clientRegion.x+clientRegion.width/2.0f , clientRegion.y + clientRegion.height );
            break;
        case BASE_POS_RIGHT_TOP:
            result.Add( clientRegion.x+clientRegion.width , clientRegion.y );
            break;
        case BASE_POS_RIGHT_CENTER:
            result.Add( clientRegion.x+clientRegion.width , clientRegion.y + clientRegion.height/2.0f );
            break;
        case BASE_POS_RIGHT_DOWN:
            result.Add( clientRegion.x+clientRegion.width , clientRegion.y + clientRegion.height );
            break;
        case BASE_POS_PARENT_GLOBAL_LEFT_TOP:
            if( m_Parent ) {
                result += m_Parent->GetGlobalPos();
            }
            break;
        case BASE_POS_PARENT_GLOBAL_CENTER_TOP:
            if( m_Parent ) {
                result += m_Parent->GetGlobalPos();
                result.Add(m_Parent->GetWidth()/2.0f,0.0f);
            }
            break;
        case BASE_POS_PARENT_GLOBAL_CENTER:
            if( m_Parent ) {
                result += m_Parent->GetGlobalPos();
                result.Add(m_Parent->GetWidth()/2.0f,m_Parent->GetHeight()/2.0f);
            }
            break;
        }
        switch (m_Align.x)
        {
        case TB_UI_ALIGN_X_CENTER:
            result.Add(-m_Region.width/2.0f,0.0f);
            break;
        case TB_UI_ALIGN_X_RIGHT:
            result.Add(-m_Region.width,0.0f);
            break;
        }
        switch (m_Align.y)
        {
        case TB_UI_ALIGN_Y_CENTER:
            result.Add(0.0f,-m_Region.height/2.0f);
            break;
        case TB_UI_ALIGN_Y_BOTTOM:
            result.Add(0.0f,-m_Region.height);
            break;
        }
    }
    return result;
}

/*!
 * グローバル空間での子Widgetすべてを含んだ包括領域
 * @author teco
 */
TbRectF32 TbUiWidget::GetCoverageGlobalRegion() const
{
    TbRectF32 result;
    TbVector2 leftTop(TB_FLOAT32_MAX,TB_FLOAT32_MAX);
    TbVector2 rightDown(TB_FLOAT32_MIN,TB_FLOAT32_MIN);
    if( !IsRoot() ){ // Windowを持っているのはルートで自信の座標は無視する
        result = GetGlobalRegion();
        leftTop.Set(result.x,result.y);
        rightDown.Set(result.x+result.width,result.y+result.height);
    }

    TbForIterator( it , m_Widgets ) {
        TbRectF32 region = (*it)->GetGlobalRegion();
        if( region.x < leftTop.GetX() ){
            leftTop.SetX( region.x );
        }
        if( region.y < leftTop.GetY() ){
            leftTop.SetY( region.y );
        }
        if( rightDown.GetX() < region.x+region.width ){
            rightDown.SetX( region.x+region.width );
        }
        if( rightDown.GetY() < region.y+region.height ){
            rightDown.SetY( region.y+region.height );
        }
    }

    result.x        = leftTop.GetX();
    result.y        = leftTop.GetY();
    result.width    = rightDown.GetX() - leftTop.GetX();
    result.height   = rightDown.GetY() - leftTop.GetY();
    return result;
}

/*!
 * フォーカス設定
 * @author teco
 */
void TbUiWidget::SetFocus()
{
    if( m_Parent ){
        m_Parent->SetFocusedWidget(this);
    }
}

/*!
 * 親も含めてフォーカス状態にする
 * @author teco
 */
void TbUiWidget::SetBroadCastFocus()
{
    if( m_Parent ){
        m_Parent->SetFocusedWidget(this);
        m_Parent->SetBroadCastFocus();
    }
}

/*!
 * マルチフォーカス設定
 * @author teco
 */
void TbUiWidget::SetEnableMultiFocus( TbBool isEnable )
{
    TbBool isFirst = TB_TRUE;
    m_BitArray.SetBit(FLAG_MULTI_FOCUS,isEnable);
    if( IsCreated() )
    {
        TbForIterator( it , m_Widgets ){
            if( isFirst ){
                isFirst = TB_FALSE;
            }else{
                if( isEnable ) {
                    (*it)->NotifyFocusChanged(TB_TRUE,*it);
                }else{
                    (*it)->NotifyFocusChanged(TB_FALSE,m_Widgets.GetFront());
                }
            }
        }
    }
}

/*!
 * フォーカスが当たっている
 * @author teco
 */
TbBool TbUiWidget::IsFocused() const
{
    if( !IsCreated() ){
        return TB_FALSE;
    }
    if( IsRoot() ){
        return TB_TRUE;
    }
    const TbUiWindow* window = GetWindow();
    if( !window || !window->IsFocus() ) {
        return TB_FALSE;
    }
    if(m_Parent->IsFocused() )
    {
        if( this == m_Parent->GetFocusedWidget() ) {
            return TB_TRUE;
        }else if( m_Parent->IsEnableMultiFocus() ) {
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 所有者取得
 * @author teco
 */
TbUint32 TbUiWidget::GetPlayerBit() const
{
    if( !IsCreated() ){
        return static_cast<TbUint32>(-1);
    }
    const TbUiWindow* window = GetWindow();
    return window ? window->GetPlayerBit() : 0;
}

/*!
 * ウィジェット追加
 * @author teco
 */
void TbUiWidget::addWidget( TbUiWidget* widget )
{
    TB_RESQUE(widget);
    TB_RESQUE(widget->GetWindow()==GetWindow());
    TB_ASSERT(TbFind(m_Widgets,widget)==m_Widgets.End());
    m_Widgets.PushBack(widget);
}

/*! 
 * ウィジェット除去
 * @author teco
 */
void TbUiWidget::removeWidget( TbUiWidget* widget )
{
    TB_RESQUE(widget);
    TB_RESQUE(widget->GetWindow()==GetWindow());
    WidgetList::Iterator it = TbFind(m_Widgets,widget);
    if( it != m_Widgets.End() ){
        m_Widgets.Erase(it);
    }
    it = TbFind(m_ExecWidgets,widget);
    if( it != m_ExecWidgets.End() ){
        *it = nullptr;
    }

}

}
