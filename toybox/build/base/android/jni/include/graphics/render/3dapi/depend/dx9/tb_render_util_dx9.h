/*!
 * @file
 * @brief Dx9用Utiluty関数
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */
 
#ifndef _INCLUDED_TB_RENDER_UTIL_DX9_H_
#define _INCLUDED_TB_RENDER_UTIL_DX9_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>
#include <graphics/render/3dapi/tb_render_view.h>
#include <graphics/render/3dapi/tb_texture.h>
#include <graphics/render/3dapi/tb_texture_sampler.h>
#include <base/math/tb_vector4.h>

namespace toybox
{
    class TbRenderManager;
    class TbWindow;

    // toybox用カラーフォーマットから3DAPIに適したフォーマットに変換
    inline TbColorFormat TbGetOptimizedColorFormat( const TbColorFormat& src )
    {
        static const TbColorFormat CONVERT_TABLE[] = 
        {
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_UNKNOWN

            TB_COLOR_FORMAT_A4G4B4R4 , // TB_COLOR_FORMAT_A4G4B4R4
            TB_COLOR_FORMAT_R5G6B5 ,   // TB_COLOR_FORMAT_R5G6B5

            TB_COLOR_FORMAT_R8G8B8 ,   // TB_COLOR_FORMAT_R8G8B8
            TB_COLOR_FORMAT_R8G8B8 ,   // TB_COLOR_FORMAT_B8G8R8

            TB_COLOR_FORMAT_A8R8G8B8 ,  // TB_COLOR_FORMAT_A8R8G8B8
            TB_COLOR_FORMAT_A8R8G8B8 ,  // TB_COLOR_FORMAT_B8G8R8A8
            TB_COLOR_FORMAT_A8R8G8B8 ,  // TB_COLOR_FORMAT_A8B8R8R8
            TB_COLOR_FORMAT_X8R8G8B8 ,  // TB_COLOR_FORMAT_X8R8G8B8

            TB_COLOR_FORMAT_A32R32G32B32 , // TB_COLOR_FORMAT_A32R32G32B32

            TB_COLOR_FORMAT_D16 , // TB_COLOR_FORMAT_D16
            TB_COLOR_FORMAT_D32 , // TB_COLOR_FORMAT_D32
            TB_COLOR_FORMAT_D24S8 , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM );
        if( TB_COLOR_FORMAT_BEGIN <= src && src < TB_COLOR_FORMAT_END ){
            return CONVERT_TABLE[ src - TB_COLOR_FORMAT_BEGIN ];
        }
        return TB_COLOR_FORMAT_UNKNOWN;
    }

    // TbColorFormat -> D3DFORMAT
    D3DFORMAT TbColorFormatToD3DFORMAT( const TbColorFormat& format );
    TbColorFormat D3DFORMATtoTbColorFormat( const D3DFORMAT& format );

    // TbMultiSampleType -> D3DMULTISAMPLE_TYPE
    inline D3DMULTISAMPLE_TYPE TbMultiSampleTypeToD3DMULTISAMPLE_TYPE( const TbMultiSampleType& type ) { return static_cast<D3DMULTISAMPLE_TYPE>(type); }

    // D3DMULTISAMPLE_TYPE -> TbMultiSampleType
    inline TbMultiSampleType D3DMULTISAMPLE_TYPEToTbMultiSampleType( const D3DMULTISAMPLE_TYPE& type ) { return static_cast<TbMultiSampleType>(type); }

    // TbRenderViewParameter -> D3DPRESENT_PARAMETERS
    TbBool TbRenderViewParameterToD3DPRESENT_PARAMETERS(    LPDIRECT3D9 direct3d , 
                                                            TbRenderDevice* device , 
                                                            TbUint32 adapterIndex , 
                                                            TbWindow* focusWindow , 
                                                            const TbRenderViewParameter& src , 
                                                            D3DPRESENT_PARAMETERS* dst ,
                                                            D3DPRESENT_PARAMETERS* dstFullscreen );

    // D3DPRESENT_PARAMETERS -> TbRenderViewParameter
    TbBool D3DPRESENT_PARAMETERSToTbRenderViewParameter( TbRenderViewParameter& dst , const D3DPRESENT_PARAMETERS& src );

    // TbTextureFilterMode -> D3DTEXTUREFILTERTYPE
    TbBool TbTextureFilterModeToD3DTEXTUREFILTERTYPE( D3DTEXTUREFILTERTYPE& dst , const TbTextureFilterMode& src );

    // TbTextureAddressMode -> D3DTEXTUREADDRESS
    TbBool TbTextureAddressModeToD3DTEXTUREADDRESS( D3DTEXTUREADDRESS& dst , const TbTextureAddressMode& src );

    // IDirect3DSurface9から横幅と縦幅を取得
    TbBool TbGetSizeFromIDirect3DSurface9( TB_OUT TbUint32& width , TB_OUT TbUint32& height , IDirect3DSurface9* surface );

    // IDirect3DTexture9から横幅と縦幅を取得
    TbBool TbGetSizeFromIDirect3DTexture9( TB_OUT TbUint32& width , TB_OUT TbUint32& height , IDirect3DTexture9* texture , TbUint32 level );

    // IDirect3DSurface9からフォーマット取得
    TbBool TbGetColorFormatFromIDirect3DSurface9( TB_OUT TbColorFormat& format , IDirect3DSurface9* surface );

    // IDirect3DTexture9からフォーマット取得
    TbBool TbGetColorFormatFromIDirect3DTexture9( TB_OUT TbColorFormat& format , IDirect3DTexture9* texture , TbUint32 level );

    // IDirect3DTexture9からサーフェイス情報を取得
    TbBool TbGetSurfaceInfoFromIDirect3DTexture9( TB_OUT TbUint32& width , TB_OUT TbUint32& height , TB_OUT TbColorFormat& format , IDirect3DTexture9* texture , TbUint32 level );

    // toybox用ブレンド係数 -> D3DBLEND
    inline TbBool TbBlendFactorToD3DBLEND( D3DBLEND& dst , const TbBlendFactor& src )
    {       
        static const D3DBLEND CONVERT_TABLE[] =
        {
            D3DBLEND_ZERO , 
            D3DBLEND_ONE , 
            D3DBLEND_SRCCOLOR ,
            D3DBLEND_INVSRCCOLOR ,
            D3DBLEND_DESTCOLOR ,
            D3DBLEND_DESTCOLOR ,
            D3DBLEND_SRCALPHA ,
            D3DBLEND_INVSRCALPHA ,
            D3DBLEND_DESTALPHA , 
            D3DBLEND_INVDESTALPHA ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_FACTOR_MAX );
        if( TB_BLEND_FACTOR_BEGIN <= src && src < TB_BLEND_FACTOR_END ){
            dst = CONVERT_TABLE[src-TB_BLEND_FACTOR_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }       

    // toybox用ブレンド演算子 -> D3DBLENDOP
    inline TbBool TbBlendOpToD3DBLENDOP( D3DBLENDOP& dst , const TbBlendOp& src )
    {
        static const D3DBLENDOP CONVERT_TABLE[] =
        {
            D3DBLENDOP_ADD , 
            D3DBLENDOP_SUBTRACT , 
            D3DBLENDOP_REVSUBTRACT ,
            D3DBLENDOP_MIN , 
            D3DBLENDOP_MAX , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_OP_MAX );
        if( TB_BLEND_OP_BEGIN <= src && src < TB_BLEND_OP_END ){
            dst = CONVERT_TABLE[src-TB_BLEND_OP_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用比較関数 -> D3DCMPFUNC
    inline TbBool TbCmpFuncToD3DCMPFUNC( D3DCMPFUNC& dst , const TbCmpFunc& func )
    {
        static const D3DCMPFUNC CONVERT_TABLE[] =
        {
            D3DCMP_NEVER,
            D3DCMP_LESS,
            D3DCMP_EQUAL,
            D3DCMP_LESSEQUAL,
            D3DCMP_GREATER,
            D3DCMP_NOTEQUAL,
            D3DCMP_GREATEREQUAL,
            D3DCMP_ALWAYS,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_CMP_FUNC_MAX );
        if( TB_CMP_FUNC_BEGIN <= func && func < TB_CMP_FUNC_END ){
            dst = CONVERT_TABLE[func-TB_CMP_FUNC_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用ステンシル関数 -> D3DSTENCILOP
    inline TbBool TbStencilOpToD3DSTENCILOP( D3DSTENCILOP& dst , const TbStencilOp& op )
    {
        static const D3DSTENCILOP CONVERT_TABLE[] =
        {
            D3DSTENCILOP_KEEP ,
            D3DSTENCILOP_ZERO ,
            D3DSTENCILOP_REPLACE ,
            D3DSTENCILOP_INCRSAT ,
            D3DSTENCILOP_DECRSAT ,
            D3DSTENCILOP_INVERT ,
            D3DSTENCILOP_INCR ,
            D3DSTENCILOP_DECR , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_STENCIL_OP_MAX );
        if( TB_STENCIL_OP_BEGIN <= op && op < TB_STENCIL_OP_END ){
            dst = CONVERT_TABLE[ op - TB_STENCIL_OP_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用カラークラスをD3DCOLORVALUEに変換
    inline D3DCOLORVALUE TbColorToD3DCOLORVALUE( const TbColor& src )
    {
        D3DCOLORVALUE result = {
            src.GetRedF32() , 
            src.GetGreenF32() , 
            src.GetBlueF32() , 
            src.GetAlphaF32()
        };
        return result;
    };

    // toybox用ベクトルからD
    inline D3DVECTOR TbVector4ToD3DVECTOR( const TbVector4& vec )
    {
        D3DVECTOR result = {
            vec.GetX() ,
            vec.GetY() ,
            vec.GetZ()
        };
        return result;
    }

    // エラーコードを出力
    inline void TbDxPrintErrorCode( HRESULT hr ) {
        TbPrintf("[TbDxPrintErrorCode]%s\n",DXGetErrorDescriptionA(hr));
    }

}

#endif
