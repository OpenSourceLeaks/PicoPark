/*!
 * @file
 * @brief カラークラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_color.h"
#include "base/system/tb_assert.h"

namespace toybox
{

/*!
 * カラーフォーマットのビット数取得
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbUint8 TbGetColorFormatBitSize( const TbColorFormat colorFormat )
{
    static TbUint8 SIZE_TABLE[] = 
    {
        0U ,  // TB_COLOR_FORMAT_UNKNOWN,
        16U , // TB_COLOR_FORMAT_A4G4B4R4,
        16U , // TB_COLOR_FORMAT_R5G6B5,
        24U , // TB_COLOR_FORMAT_R8G8B8,
        24U , // TB_COLOR_FORMAT_B8G8R8
        32U , // TB_COLOR_FORMAT_A8R8G8B8,
        32U , // TB_COLOR_FORMAT_B8G8R8A8
        32U , // TB_COLOR_FORMAT_A8B8G8R8
        32U , // TB_COLOR_FORMAT_X8R8G8B8,
        128U , // TB_COLOR_FORMAT_A32R32G32B32,
        16U  , // TB_COLOR_FORMAT_D16
        32U  , // TB_COLOR_FORMAT_D32
        32U  , // TB_COLOR_FORMAT_D24S8        
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(SIZE_TABLE) == TB_COLOR_FORMAT_NUM );
    if( TB_COLOR_FORMAT_BEGIN <= colorFormat && colorFormat < TB_COLOR_FORMAT_END )
    {
        return SIZE_TABLE[ colorFormat ];
    }
    return 0U;
}

/*!
 * カラーフォーマットのビット数取得(深度)
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbUint8 TbGetColorFormatDepthBitSize( const TbColorFormat colorFormat )
{
    static TbUint8 SIZE_TABLE[] = 
    {
        0U ,  // TB_COLOR_FORMAT_UNKNOWN,
        0U , // TB_COLOR_FORMAT_A4G4B4R4,
        0U , // TB_COLOR_FORMAT_R5G6B5,
        0U , // TB_COLOR_FORMAT_R8G8B8,
        0U , // TB_COLOR_FORMAT_B8G8R8
        0U , // TB_COLOR_FORMAT_A8R8G8B8,
        0U , // TB_COLOR_FORMAT_B8G8R8A8
        0U , // TB_COLOR_FORMAT_A8B8G8R8
        0U , // TB_COLOR_FORMAT_X8R8G8B8,
        0U , // TB_COLOR_FORMAT_A32R32G32B32,
        16U  , // TB_COLOR_FORMAT_D16
        32U  , // TB_COLOR_FORMAT_D32
        24U  , // TB_COLOR_FORMAT_D24S8        
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(SIZE_TABLE) == TB_COLOR_FORMAT_NUM );
    if( TB_COLOR_FORMAT_BEGIN <= colorFormat && colorFormat < TB_COLOR_FORMAT_END )
    {
        return SIZE_TABLE[ colorFormat ];
    }
    return 0U;
}

/*!
 * カラーフォーマットのビット数取得(ステンシル)
 * @author Miyake Shunsuke
 * @since 2011.01.22
 */
TbUint8 TbGetColorFormatStencilBitSize( const TbColorFormat colorFormat )
{
    static TbUint8 SIZE_TABLE[] = 
    {
        0U ,  // TB_COLOR_FORMAT_UNKNOWN,
        0U , // TB_COLOR_FORMAT_A4G4B4R4,
        0U , // TB_COLOR_FORMAT_R5G6B5
        0U , // TB_COLOR_FORMAT_R8G8B8,
        0U , // TB_COLOR_FORMAT_B8G8R8
        0U , // TB_COLOR_FORMAT_A8R8G8B8,
        0U , // TB_COLOR_FORMAT_B8G8R8A8
        0U , // TB_COLOR_FORMAT_A8B8G8R8
        0U , // TB_COLOR_FORMAT_X8R8G8B8,
        0U , // TB_COLOR_FORMAT_A32R32G32B32,
        0U  , // TB_COLOR_FORMAT_D16
        0U  , // TB_COLOR_FORMAT_D32
        8U  , // TB_COLOR_FORMAT_D24S8        
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(SIZE_TABLE) == TB_COLOR_FORMAT_NUM );
    if( TB_COLOR_FORMAT_BEGIN <= colorFormat && colorFormat < TB_COLOR_FORMAT_END )
    {
        return SIZE_TABLE[ colorFormat ];
    }
    return 0U;
}


}