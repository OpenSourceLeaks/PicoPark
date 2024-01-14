/*!
 * ディスプレイアダプタの外部指定依存
 * @author teco
 */

#ifndef _INCLUDED_TB_DISPLAY_MANAGER_EXTERNAL_H_
#define _INCLUDED_TB_DISPLAY_MANAGER_EXTERNAL_H_

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

