/*!
 * @file
 * @brief ウィンドう管理クラスのヘッダーファイル
 * @author Miyake Shunsuke
 * @since 2010.4.10
 */
 
#ifndef _INCLUDED_TB_WINDOW_MANAGER_H_
#define _INCLUDED_TB_WINDOW_MANAGER_H_

#include <base/container/tb_array.h>
#include <base/util/tb_singleton.h>

#include <graphics/window/depend/tb_window_manager_depend.h>

namespace toybox
{

struct TbWindowParameter;
class TbWindow;

/*!
 * @class 
 */
class TbWindowManager : public TbSingletonRef<TbWindowManager>
{
public:

    struct KeyValue
    {
        TbUint32 key;
        TbWindow* value;
    };
    typedef TbArray<KeyValue> WindowList;

public:

    // コンストラクタ
    TbWindowManager();

    // デストラクタ
    ~TbWindowManager();

public:

    // ウィンドウ生成
    TbWindow* CreateWindowWithParam( const TbWindowParameter& param );

    // ウィンドウ破棄
    TbResult DeleteWindow( TbWindowId id );

    // 全ウィンドウ破棄
    TbResult DeleteWindowAll();

    //! エラー
    TbBool IsError() const { 
        return m_BitArray.Test( FLAG_ERROR ); 
    }

    //! ウィンドウ開始イテレータ
    WindowList::Iterator GetWindowBegin() {
        return m_WindowList.Begin();
    }

    //! ウィンドウ最後尾イテレータ
    WindowList::Iterator GetWindowEnd() {
        return m_WindowList.End();
    }

    //! ウィンドウ開始イテレータ
    WindowList::ConstIterator GetWindowBegin() const {
        return m_WindowList.Begin();
    }

    //! ウィンドウ最後尾イテレータ
    WindowList::ConstIterator GetWindowEnd() const {
        return m_WindowList.End();
    }

    // ウィンドウ取得
    TbWindow* GetWindow( TbWindowId id );

    // 現在フォーカスが当たっているウィンドウを取得
    TbWindow* GetFocusWindow() const { 
        return m_FocusWindow; 
    }

    // フォーカスウィンドウ設定
    void SetFocusWindow( TbWindow* window );

    //! 更新
    TbBool Update();

public:

    // 依存クラス取得
    TbWindowManagerDepend& GetDepend() {
        return m_Depend; 
    }

    // 依存クラス取得
    const TbWindowManagerDepend& GetDepend() const {
        return m_Depend; 
    }

private:

    enum
    {
        FLAG_INIT,  //!< 初期化済み
        FLAG_ERROR, //!< エラー
    };

private:

    // 比較関数の型
    struct KeyCompareFunc {
        TbBool operator() ( const KeyValue& left , const KeyValue& right) const {
            return ( left.key < right.key );
        }
    };

private:

    // セットアップ
    TbResult setup();

private:

    TbBitArray32            m_BitArray;             //!< ビット配列
    WindowList              m_WindowList;           //!< ウィンドウリスト
    TbWindow*               m_FocusWindow;          //!< 現在フォーカスが当たっているウィンドウ
    TbWindowManagerDepend   m_Depend;               //!< 依存クラス

};

/*!
 * ウィンドウ生成
 * @author Miyake Shunsuke
 * @since 2013.06.20
 */
inline TbWindow* TbCreateWindow( const TbWindowParameter& param )
{
    return TbWindowManager::GetInstance().CreateWindowWithParam(param);
}

/*!
 * ウィンドウ生成
 * @author Miyake Shunsuke
 * @since 2013.06.20
 */
extern TbWindow* TbCreateWindow( TbSizeT width , TbSizeT height , const char* title );

/*!
 * ウィンドウ破棄
 * @author Miyake Shunsuke
 * @since 2013.06.20
 */
inline TbResult TbDeleteWindow( TbWindowId id )
{
    return TbWindowManager::GetInstance().DeleteWindow(id);
}

}

#endif
