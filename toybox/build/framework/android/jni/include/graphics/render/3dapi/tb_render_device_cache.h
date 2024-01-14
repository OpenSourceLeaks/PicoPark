/*!
 * 描画デバイス用キャッシュ(実装途中)
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_DEVICE_CACHE_H_
#define _INCLUDED_TB_RENDER_DEVICE_CACHE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbRenderDeviceCache
{
public:

    // コンストラクタ
    TbRenderDeviceCache()
    {
    }
    
    // デストラクタ
    ~TbRenderDeviceCache()
    {
    }

public:

    // ビューポート設定
    TbBool SetViewPort( TbUint32 x , TbUint32 y , TbUint32 width , TbUint32 height )
    {
        if( m_ViewPort.x == x && m_ViewPort.y == y &&
            m_ViewPort.width == width && m_ViewPort.height == height ) 
        {
            // キャッシュヒット
            return TB_FALSE;
        }
        m_ViewPort.x = x;
        m_ViewPort.y = y;
        m_ViewPort.width = width;
        m_ViewPort.height = height;
        return TB_TRUE;
    }

private:

    enum Flag
    {
        FLAG_
    };

private:
    TbRectS32       m_ViewPort;
    TbBitArray32    m_BitArray;
    TbUint32        m_ColorMaskBit;
};

}

#endif
