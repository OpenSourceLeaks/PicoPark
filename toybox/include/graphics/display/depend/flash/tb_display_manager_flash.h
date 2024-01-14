/*!
 * ディスプレイアダプタのFlash依存
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_DISPLAY_MANAGER_FLASH_H_
#define _INCLUDED_TB_DISPLAY_MANAGER_FLASH_H_

namespace toybox
{

class TbDisplayManagerDepend
{
public:
    
    // ディスプレイ情報を外部から設定
    static void Initialize( TbUint32 width
                          , TbUint32 height
                          , TbColorFormat pixelFormat
                          , TbUint32 refreshRate );
    
    // ディスプレイ情報を外部から設定
    static void SetSize( TbUint32 width
                       , TbUint32 height );

};

}

#endif

