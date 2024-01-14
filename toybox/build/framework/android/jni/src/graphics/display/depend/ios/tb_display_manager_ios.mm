/*!
 * ディスプレイアダプタークラス( 3DAPI非依存 )
 * @author Miyake Shunsuke
 */

#import <UIKit/UIKit.h>
#import "graphics/display/tb_display_manager.h"

namespace toybox
{

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
 * @author teco
 */
TbSint32 TbDisplayManager::GetDisplayAdapterCount() const
{
    return 1;
}

/*!
 * フルスクリーン設定
 * @param isFullscreen フルスクリーンかどうか
 * @param displayMode ディスプレイモード( nullptrの場合現在設定中のディスプレイモードを利用 )
 * @author teco
 */
TbResult TbDisplayManager::SetFullscreen( TbWindow* window , TbSint32 adapterIndex , TbBool isFullscreen , TbDisplayMode* displayMode )
{
    if( 0 != adapterIndex ) {
        return TB_E_FAIL;
    }
    // 常にフルスクリーン
    if( !isFullscreen ) {
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * フルスクリーンか否か
 * @author teco
 */
TbBool TbDisplayManager::IsFullscreen( TbUint32 adapterIndex ) const
{
    if( 0 != adapterIndex ) {
        return TB_FALSE;
    }
    return TB_TRUE;
}

/*! 
 * 現在のディスプレイモードを取得
 * @author teco
 */
TbBool TbDisplayManager::GetCurrentDisplayMode( TbUint32 adapterIndex , TbDisplayMode* outDisplayMode ) const
{
    if( 0 != adapterIndex ) {
        return TB_FALSE;
    }
    if( outDisplayMode ){
        return GetDisplayMode(adapterIndex,0,outDisplayMode);
    }
    return TB_TRUE;
}

/*! 
 * ディスプレイモードを取得
 * @author teco
 */
TbBool TbDisplayManager::GetDisplayMode( TbUint32 adapterIndex , TbSint32 index , TbDisplayMode* outDisplayMode ) const
{
    if( !outDisplayMode ){
        return TB_FALSE;
    }
    if( 0 != adapterIndex ) {
        return TB_FALSE;
    }
    if( 0 != index ) {
        return TB_FALSE;
    }
    
    if( outDisplayMode ){
        CGRect rect = [UIScreen mainScreen].bounds;
        TbFloat32 scale = [UIScreen mainScreen].scale;
        outDisplayMode->width = rect.size.width * scale;
        outDisplayMode->height = rect.size.height * scale;
        outDisplayMode->refreshRate = 59.94f;
        outDisplayMode->format = TB_COLOR_FORMAT_X8R8G8B8;
    }

    return TB_TRUE;
}

/*! 
 * ディスプレイモードを取得( フォーマット別 )
 * @author teco
 */
TbBool TbDisplayManager::GetDisplayMode( TbUint32 adapterIndex , TbSint32 index , TbColorFormat format , TbDisplayMode* outDisplayMode ) const
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
 * @author teco
 */
TbSint32 TbDisplayManager::GetDisplayModeNum( TbUint32 adapterIndex ) const
{
    if( 0 != adapterIndex ) {
        return TB_FALSE;
    }
    return 1;
}

/*! 
 * 対応しているディスプレイのグラフィックスモード数を取得( フォーマット別 )
 * @author teco
 */
TbSint32 TbDisplayManager::GetDisplayModeNum( TbUint32 adapterIndex , TbColorFormat format ) const
{
    TbSint32 result = 0;
    // 未実装
    return result;
}

}
