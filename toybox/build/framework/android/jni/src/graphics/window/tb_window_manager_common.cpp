/*!
 * Window管理クラス(共通)
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */

#include "tb_fwd.h"
#include "graphics/window/tb_window_manager.h"
#include "graphics/window/tb_window.h"
#include "base/algorithm/tb_search.h"
#include "base/algorithm/tb_sort.h"

namespace toybox
{

/*!
 * ウィンドウ生成
 * @author Miyake Shunsuke
 * @since 2012.07.08
 */
TbWindow* TbWindowManager::CreateWindowWithParam( const TbWindowParameter&param )
{
    if( !m_WindowList.IsFull() || m_WindowList.IsEnableSetCapacityAuto() ){
        TbWindow* window = TB_NEW TbWindow( param );
        KeyValue keyValue = { window->GetId() ,
                              window };
        m_WindowList.PushBack( keyValue );
        TbInsertionSort( m_WindowList.Begin() , m_WindowList.End() , KeyCompareFunc() );
        return window;
    }
    return nullptr;
}

/*!
 * ウィンドウ破棄
 * @author Miyake Shunsuke
 * @since 2012.07.08
 */
TbResult TbWindowManager::DeleteWindow( TbWindowId id )
{
    KeyValue keyValue = { id , nullptr };
    TbSint32 index = TbBinarySearchIndex( m_WindowList.Begin() , m_WindowList.End() , keyValue , KeyCompareFunc() );
    if( index >= 0 ) {
        TbWindow* window = m_WindowList.GetAt(index).value;
        if( window && m_WindowList.End() != m_WindowList.Erase( index ) ){
            TB_SAFE_DELETE(window);
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * 全ウィンドウ破棄
 * @author Miyake Shunsuke
 * @since 2012.07.08
 */
TbResult TbWindowManager::DeleteWindowAll()
{
    TbArray<KeyValue>::Iterator ite = m_WindowList.Begin();
    TbArray<KeyValue>::Iterator iteEnd = m_WindowList.End();
    TbWindow* window = nullptr;
    for( ; ite != iteEnd; ++ite ){
        window = (*ite).value;
        TB_SAFE_DELETE( window );
    }
    m_WindowList.Clear();
    return TB_S_OK;
}

/*!
 * ウィンドウ取得
 * @author Miyake Shunsuke
 * @since 2010.4.10
 */
TbWindow* TbWindowManager::GetWindow( TbWindowId id )
{
    KeyValue key = { id , nullptr };
    TbSint32 index = TbBinarySearchIndex( m_WindowList.Begin() , m_WindowList.End() , key , KeyCompareFunc() );
    if( index >= 0 ){
        TbArray<KeyValue>::Iterator resultIte = m_WindowList.Begin() + index;
        return (*resultIte).value;
    }
    return nullptr;
}

/*!
 * フォーカスウィンドウ設定
 * @author Miyake Shunsuke
 * @since 2011.03.09
 */
void TbWindowManager::SetFocusWindow( TbWindow* window )
{
    // 変更してないなら何もしない
    if( m_FocusWindow == window ){
        return;
    }
    // フラグ操作
    if( m_FocusWindow ){
        m_FocusWindow->setFocused( TB_FALSE );
    }
    if( window ){
        window->setFocused( TB_TRUE );
    }
    m_FocusWindow = window;
}

/*!
 * ウィンドウ生成
 * @author Miyake Shunsuke
 * @since 2013.06.20
 */
TbWindow* TbCreateWindow( TbSizeT width , TbSizeT height , const char* title )
{
    TbWindowParameter param;
    param.rect.width = width;
    param.rect.height = height;
    param.name = title;
    param.isVisible = TB_TRUE;
    return TbWindowManager::GetInstance().CreateWindowWithParam(param);
}

}
