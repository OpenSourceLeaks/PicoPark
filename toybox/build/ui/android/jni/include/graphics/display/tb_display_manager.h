/*!
 * ディスプレイ管理クラス
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */

#ifndef _INCLUDED_TB_DISPLAY_MANAGER_H_
#define _INCLUDED_TB_DISPLAY_MANAGER_H_

#include <graphics/display/tb_display_types.h>
#include <base/util/tb_singleton.h>

// 依存クラス
#include "./depend/tb_display_manager_depend.h"

namespace toybox
{

struct TbDisplayMode;
class TbWindow;

class TbDisplayManager : public TbSingletonRef<TbDisplayManager>
{
public:

    // コンストラクタ
    TbDisplayManager();

    // デストラクタ
    ~TbDisplayManager();

public:
    
    // ディスプレイアダプタ数取得
    TbSint32 GetDisplayAdapterCount() const;
    
    // 現在のディスプレイモードを取得
    TbBool GetCurrentDisplayMode( TbUint32 adapterIndex , TbDisplayMode* outDisplayMode ) const;
    
    // ディスプレイモードを取得
    TbBool GetDisplayMode( TbUint32 adapterIndex , TbSint32 index , TbDisplayMode* outDisplayMode ) const;
    
    // ディスプレイモードを取得( フォーマット別 )
    TbBool GetDisplayMode( TbUint32 adapterIndex , TbSint32 index , TbColorFormat format , TbDisplayMode* outDisplayMode ) const;
    
    // 対応しているディスプレイのグラフィックスモード数を取得
    TbSint32 GetDisplayModeNum( TbUint32 adapterIndex ) const;
    
    // 対応しているディスプレイのグラフィックスモード数を取得( フォーマット別 )
    TbSint32 GetDisplayModeNum( TbUint32 adapterIndex , TbColorFormat format ) const;

    // フルスクリーン設定
    TbResult SetFullscreen( TbWindow* window , TbSint32 adapterIndex , TbBool isFullscreen , TbDisplayMode* displayMode );
    
    //! フルスクリーンか否か
    TbBool IsFullscreen( TbUint32 adapterIndex ) const;

public:
    // 依存クラス取得
    TbDisplayManagerDepend& GetDepend() { return m_Depend; }
    const TbDisplayManagerDepend& GetDepend() const { return m_Depend; }
private:
    TbDisplayManagerDepend m_Depend;
};

}

#endif