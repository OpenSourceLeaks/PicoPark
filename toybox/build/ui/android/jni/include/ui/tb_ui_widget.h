/*!
 * Uiウィジェット
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_WIDGET_H_
#define _INCLUDED_TB_UI_WIDGET_H_

#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>
#include <base/container/tb_static_array.h>
#include "ui/Tb_ui_types.h"

namespace toybox
{

class TbUiWindow;

class TbUiWidget
{
public:

    friend class TbUiWindow;

    // 基準位置
    enum BasePos
    {
        BASE_POS_BEGIN , 
        BASE_POS_LEFT_TOP = BASE_POS_BEGIN ,     // 左上基準
        BASE_POS_LEFT_CENTER    ,  // 左中央基準
        BASE_POS_LEFT_DOWN      ,    // 左下基準
         
        BASE_POS_CENTER_TOP     ,   // 中央上基準
        BASE_POS_CENTER         ,   // 中央基準
        BASE_POS_CENTER_DOWN    ,   // 中央下基準

        BASE_POS_RIGHT_TOP      ,    // 右上基準
        BASE_POS_RIGHT_CENTER   , // 右中央基準
        BASE_POS_RIGHT_DOWN     ,   // 右下基準

        BASE_POS_PARENT_BASE_POS          ,   // 親WidgetのBasePos
        BASE_POS_PARENT_GLOBAL_LEFT_TOP   ,   // 親Widgetのグローバル座標
        BASE_POS_PARENT_GLOBAL_CENTER_TOP ,   // 親Widgetのグローバル座標
        BASE_POS_PARENT_GLOBAL_CENTER     ,   // 親Widgetのグローバル座標
        BASE_POS_END            ,

        BASE_POS_MAX = BASE_POS_END - BASE_POS_BEGIN
    };

    typedef TbStaticFunction32<void (TbBool,TbUiWidget*)> FocusChangedCallback;

public:
    
    // コンストラクタ
    TbUiWidget();
    
    // デストラクタ
    virtual ~TbUiWidget();
    
public:

    // 生成
    void Create();

    // 削除
    void Delete();

    // 生成済みか
    TbBool IsCreated() const {
        return m_BitArray.Test(FLAG_CREATE);
    }

    // 所属しているか
    TbBool IsEntered() const {
        return m_Parent != nullptr;
    }

    // ウィンドウの所属になる
    void Enter( TbUiWindow* window );

    // ウィンドウにある特定Widgetの所属になる
    void Enter( TbUiWidget* widget );

    // ウィンドウの所属から外れる
    void Exit();

public:

    // 名前設定
    void SetName( const char* name ) {
        m_Name = name;
    }

    // 名前取得
    const char* GetName() const {
        return m_Name.GetBuffer();
    }

    // 基準位置設定
    void SetBasePos( BasePos base ) {
        TB_RESQUE(BASE_POS_BEGIN<=base&&base<BASE_POS_END);
        m_BasePos = base;
    }

    // 所有者取得
    TbUint32 GetPlayerBit() const;

    // 基準位置取得
    BasePos GetBasePos() const {
        return m_BasePos;
    }
    
    // 座標設定
    void SetPos( const TbVector2& pos ) {
        m_Region.x = pos.GetX();
        m_Region.y = pos.GetY();
    }

    // 座標設定
    void SetPos( TbFloat32 x , TbFloat32 y ) {
        m_Region.x = x;
        m_Region.y = y;
    }

    // X座標設定
    void SetPosX( TbFloat32 x ) {
        m_Region.x = x;
    }

    // Y座標設定
    void SetPosY( TbFloat32 y ) {
        m_Region.y = y;
    }

    // 座標設定
    TbVector2 GetPos() const {
        return TbVector2(m_Region.x,m_Region.y);
    }

    // サイズ設定
    void SetSize( TbFloat32 w , TbFloat32 h ) {
        m_Region.width = w;
        m_Region.height = h;
    }

    // 幅設定
    void SetWidth( TbFloat32 w ) {
        m_Region.width = w;
    }

    // 高さ設定
    void SetHeight( TbFloat32 h ) {
        m_Region.height = h;
    }

    // 領域取得
    const TbRectF32& GetRegion() const {
        return m_Region;
    }

    // 横幅取得
    TbFloat32 GetWidth() const {
        return m_Region.width;
    }

    // 縦幅取得
    TbFloat32 GetHeight() const {
        return m_Region.height;
    }

    // グローバル座標取得
    TbVector2 GetGlobalPos() const;

    // グローバル空間での領域
    TbRectF32 GetGlobalRegion() const {
        TbVector2 pos = GetGlobalPos();
        TbRectF32 result( pos.GetX() , pos.GetY() , m_Region.width , m_Region.height );
        return result;
    }

    // 右下座標取得
    TbVector2 GetGlobalRightBottomPos() const {
        TbVector2 pos = GetGlobalPos();
        pos.Add(m_Region.width,m_Region.height);
        return pos;
    }

    // グローバル空間での子Widgetすべてを含んだ包括領域
    TbRectF32 GetCoverageGlobalRegion() const;

    // フォーカス設定
    void SetFocus();

    // 親も含めてフォーカス状態にする
    void SetBroadCastFocus();

    // マルチフォーカス設定
    void SetEnableMultiFocus( TbBool isEnable );

    // マルチフォーカス設定
    TbBool IsEnableMultiFocus() const {
        return m_BitArray.Test(FLAG_MULTI_FOCUS);
    }

    // フォーカスが当たっている
    TbBool IsFocused() const;

    // 可視設定
    void SetVisible( TbBool isVisible ) {
        if( TB_BOOL_NOT_EQUAL(m_BitArray.Test(FLAG_VISIBLE),isVisible) ){
            m_BitArray.SetBit(FLAG_VISIBLE,isVisible);
            OnVisibleChanged(isVisible);
        }
    }

    // 可視情報
    TbBool IsVisible() const;

    // X軸アライン設定
    void SetAlignX( TbUiAlignX align ) {
        m_Align.x = align;
    }

    // Y軸アライン設定
    void SetAlignY( TbUiAlignY align ) {
        m_Align.y = align;
    }

    // アライン一括設定
    void SetAlign( TbUiAlignX alignX , TbUiAlignY alignY ) {
        m_Align.x = alignX;
        m_Align.y = alignY;
    }

    // フォーカス時コールバック
    void SetFocusChangedCallback( const FocusChangedCallback& cb ) {
        m_FocusChangedCallback = cb;
    }

    // フォーカス状態変更通知
    void NotifyFocusChanged( TbBool isFocus , TbUiWidget* focusedWidget ){
        if( m_FocusChangedCallback.IsValid() ){
            m_FocusChangedCallback(isFocus,focusedWidget);
        }
        OnFocusChanged(isFocus,focusedWidget);
    }

    // ルートウィジェットか
    TbBool IsRoot() const {
        return m_Window != nullptr;
    }

public:

    // 所属ウィンドウ取得
    TbUiWindow* GetWindow() {
        if( m_Parent ){
            return m_Parent->GetWindow();
        }
        return m_Window;
    }

    // 所属ウィンドウ取得
    const TbUiWindow* GetWindow() const {
        if( m_Parent ){
            return m_Parent->GetWindow();
        }
        return m_Window;
    }

public:

    // フォーカスウィジェット設定
    void SetFocusedWidget( TbUiWidget* widget );

    // フォーカスが当たっているウィジェット取得
    TbUiWidget* GetFocusedWidget();

    // フォーカスが当たっているウィジェット取得
    const TbUiWidget* GetFocusedWidget() const;

    // 所属しているウィジェットをクリア
    void ClearWidget();

    // 子Widgetを持っているか
    TbBool IsEmptyChild() {
        return m_Widgets.IsEmpty();
    }

    // 子Widget数取得
    TbUint32 GetChildCount() const {
        return m_Widgets.GetCount();
    }

    // 子Widget取得
    TbUiWidget* GetChildWidget( TbUint32 index ) {
        if( index < m_Widgets.GetCount() ){
            return m_Widgets[index];
        }
        return nullptr;
    }

public:

    // 定期処理
    void Update( TbFloat32 deltasec )
    {
        OnUpdate(deltasec);
        m_ExecWidgets = m_Widgets;
        TbForIterator( it , m_ExecWidgets ) {
            (*it)->Update(deltasec);
        }
    }
    
    // 定期後処理
    void Post( TbFloat32 deltasec )
    {
        OnPost(deltasec);
        m_ExecWidgets = m_Widgets;
        TbForIterator( it , m_ExecWidgets ) {
            (*it)->Post(deltasec);
        }
    }

    // 描画
    void Draw()
    {
        if (IsCreated()) {
            OnDraw();
        }
        TbForIterator( it , m_Widgets ) {
            (*it)->Draw();
        }
    }

    // プレイヤービットが変更された
    void NotifyPlayerBitChanged( TbUint32 playerBit ) {
        OnPlayerBitChanged( playerBit ); // この中でウィジェットの数操作は禁止
        TbForIterator( it , m_Widgets ) {
            (*it)->NotifyPlayerBitChanged(playerBit);
        }
    }

protected:

    // 生成時コールバック
    virtual void OnCreated() {}

    // 破棄時コールバック
    virtual void OnDeleted() {}

protected:

    // ウィンドウの所属になる
    virtual void OnEnter( TbUiWidget* widget ) {}

    // ウィンドウの所属になる
    virtual void OnExit() {}

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltasec ) {}

    // 定期後処理
    virtual void OnPost( TbFloat32 deltaSec ) {}

    // 描画処理
    virtual void OnDraw() {}

    // 可視情報変化時コールバック
    virtual void OnVisibleChanged( TbBool isVisible ) {}

protected:

    // フォーカス変更時コールバック
    virtual void OnFocusChanged( TbBool isFocus , TbUiWidget* focusedWidget ) {}

    // プレイヤービットが変更された
    virtual void OnPlayerBitChanged( TbUint32 playerBit ) {}

private:

    enum Flag
    {
        FLAG_CREATE , 
        FLAG_VISIBLE ,
        FLAG_MULTI_FOCUS , // 子を全てフォーカス状態にする
        FLAG_MAX
    };

    static const TbSizeT WIDGET_MAX = 64;
    typedef TbStaticArray<TbUiWidget*,WIDGET_MAX> WidgetList;

private:

    // ウィジェット追加
    void addWidget( TbUiWidget* widget );

    // ウィジェット除去
    void removeWidget( TbUiWidget* widget );

private:

    TbRectF32               m_Region;
    BasePos                 m_BasePos;
    TbUiAlign               m_Align;
    TbBitArray16            m_BitArray;
    TbStaticString16        m_Name;
    TbUiWindow*             m_Window;
    TbUiWidget*             m_Parent;
    FocusChangedCallback    m_FocusChangedCallback;

private:

    WidgetList              m_Widgets;          // ウィジェット
    WidgetList              m_ExecWidgets;      // 実行中ウィジェット

};

}

#endif
