/*!
 * ディスプレイアダプタークラス( 3DAPI非依存 )
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */

#include "tb_fwd.h"
#include "graphics/window/tb_window.h"
#include "graphics/display/tb_display_manager.h"

namespace toybox
{

namespace
{
    TbColorFormat s_PixelFormat = TB_COLOR_FORMAT_UNKNOWN;
    TbUint32      s_Width       = 0;
    TbUint32      s_Height      = 0;
    TbUint32      s_RefreshRate = 0;
}

/*!
 * ディスプレイ情報を外部から設定
 * @author Miyake Shunsuke
 * @since 2012.07.10
 */
void TbDisplayManagerDepend::Initialize( TbUint32 width
                                       , TbUint32 height
                                       , TbColorFormat pixelFormat
                                       , TbUint32 refreshRate )
{
    s_Width = width;
    s_Height = height;
    s_RefreshRate = refreshRate;
    s_PixelFormat = pixelFormat;
}

/*!
 * ディスプレイ情報を外部から設定
 * @author Miyake Shunsuke
 * @since 2012.07.10
 */
void TbDisplayManagerDepend::SetSize( TbUint32 width
                                    , TbUint32 height )
{
    s_Width = width;
    s_Height = height;
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.25
 */
TbDisplayManager::TbDisplayManager()
    : m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.25
 */
TbDisplayManager::~TbDisplayManager()
{
}

/*!
 * ディスプレイアダプタ数取得
 * @author Miyake Shunsuke
 * @since 2011.01.25
 */
TbSint32 TbDisplayManager::GetDisplayAdapterCount() const {
    return 1;
}

/*!
 * 
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */
TbResult TbDisplayManager::SetFullscreen( TbWindow* window , TbSint32 adapterIndex , TbBool isFullscreen , TbDisplayMode* displayMode )
{
    if( isFullscreen ){
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * フルスクリーンか否か
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */
TbBool TbDisplayManager::IsFullscreen( TbUint32 adapterIndex ) const
{
    return TB_TRUE;
}

/*! 
 * 現在のディスプレイモードを取得
 * @author Miyake Shunsuke
 * @since 2011.01.26
 */
TbBool TbDisplayManager::GetCurrentDisplayMode( TbUint32 adapterIndex , TbDisplayMode* outDisplayMode ) const
{
    if( !outDisplayMode || 0 != adapterIndex ){
        return TB_FALSE;
    }
    outDisplayMode->width = s_Width;
    outDisplayMode->height = s_Height;
    outDisplayMode->refreshRate = s_RefreshRate;
    outDisplayMode->format = s_PixelFormat;
    return TB_TRUE;
    
}

/*! 
 * ディスプレイモードを取得
 * @author Miyake Shunsuke
 * @since 2011.01.26
 */
TbBool TbDisplayManager::GetDisplayMode( TbUint32 adapterIndex , TbSint32 mode , TbDisplayMode* outDisplayMode ) const
{
    if( !outDisplayMode || 0 != adapterIndex || 0 != mode ){
        // 一つのみ
        return TB_FALSE;
    }
    return GetCurrentDisplayMode( adapterIndex , outDisplayMode );
}

/*! 
 * ディスプレイモードを取得( フォーマット別 )
 * @author Miyake Shunsuke
 * @since 2011.01.26
 */
TbBool TbDisplayManager::GetDisplayMode( TbUint32 adapterIndex , TbSint32 mode , TbColorFormat format , TbDisplayMode* outDisplayMode ) const
{
    if( !outDisplayMode ){
        return TB_FALSE;
    }
    TbBool result =TB_FALSE;
    // 未実装
    return result;
}

/*! 
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author Miyake Shunsuke
 * @since 2011.01.26
 */
TbSint32 TbDisplayManager::GetDisplayModeNum( TbUint32 adapterIndex ) const
{
    TbSint32 result = 0;
    if( 0 == adapterIndex ) {
        return 1;
    }
    return result;
}

/*! 
 * 対応しているディスプレイのグラフィックスモード数を取得( フォーマット別 )
 * @author Miyake Shunsuke
 * @since 2011.01.26
 */
TbSint32 TbDisplayManager::GetDisplayModeNum( TbUint32 adapterIndex , TbColorFormat format ) const
{
    TbSint32 result = 0;
    // 未実装
    TB_ASSERT(!"unsupported");
    return result;
}

}

