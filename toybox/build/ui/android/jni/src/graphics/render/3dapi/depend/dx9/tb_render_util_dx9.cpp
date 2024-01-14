/*!
 * @file
 * @brief Dx9用Utiluty関数
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_3dapi_types.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/window/tb_window.h"
#include "graphics/window/tb_window_manager.h"

namespace toybox
{
    /*!
     * TbColorFormat → D3DFORMAT
     * @author Miyake Shunsuke
     * @since 2011.01.08
     */
    D3DFORMAT TbColorFormatToD3DFORMAT( const TbColorFormat& format )
    {
        static const D3DFORMAT CONVERT_TABLE[] =
        {
            D3DFMT_UNKNOWN ,       // TB_COLOR_FORMAT_UNKOWN 
            
            D3DFMT_A4R4G4B4 ,      // TB_COLOR_FORMAT_A4G4B4R4
            D3DFMT_R5G6B5   ,      // TB_COLOR_FORMAT_R5G6B5
            
            D3DFMT_R8G8B8 ,        // TB_COLOR_FORMAT_R8G8B8
            D3DFMT_R8G8B8 ,        // TB_COLOR_FORMAT_B8G8R8 (強制変換)
            
            D3DFMT_A8R8G8B8 ,      // TB_COLOR_FORMAT_A8R8G8B8
            D3DFMT_A8R8G8B8 ,      // TB_COLOR_FORMAT_B8G8R8A8 (強制変換)
            D3DFMT_A8R8G8B8 ,      // TB_COLOR_FORMAT_A8B8G8R8 (強制変換)
            D3DFMT_X8R8G8B8 ,      // TB_COLOR_FORMAT_X8R8G8B8
            
            D3DFMT_A32B32G32R32F , // TB_COLOR_FORMAT_A32R32G32B32

            D3DFMT_D16 ,           // TB_COLOR_FORMAT_D16
            D3DFMT_D32 ,           // TB_COLOR_FORMAT_D32
            D3DFMT_D24S8 ,         // TB_COLOR_FORMAT_D24S8
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM );
        return CONVERT_TABLE[ format ];
    }

    /*!
     * TbColorFormat → D3DFORMAT
     * @author Miyake Shunsuke
     * @since 2011.01.08
     */
    TbColorFormat TbD3DFORMATToColorFormat( const D3DFORMAT& format )
    {
        TbColorFormat result = TB_COLOR_FORMAT_UNKNOWN;
        switch( format )
        {
        case D3DFMT_A4R4G4B4:
            result = TB_COLOR_FORMAT_A4G4B4R4;
            break;
        case D3DFMT_R8G8B8:
            result = TB_COLOR_FORMAT_R8G8B8;
            break;
        case D3DFMT_A8R8G8B8:
            result = TB_COLOR_FORMAT_A8R8G8B8;
            break;
        case D3DFMT_X8R8G8B8:
            result = TB_COLOR_FORMAT_X8R8G8B8;
            break;
        case D3DFMT_A32B32G32R32F:
            result = TB_COLOR_FORMAT_A32R32G32B32;
            break;
        case D3DFMT_D16:
            result = TB_COLOR_FORMAT_D16;
            break;
        case D3DFMT_D32:
            result = TB_COLOR_FORMAT_D32;
            break;
        case D3DFMT_D24S8:
            result = TB_COLOR_FORMAT_D24S8;
            break;
        }
        return result;
    }

    /*!
     * D3DFORMAT -> TbColorFormat
     * @author Miyake Shunsuke
     * @since 2011.01.08
     */
    TbColorFormat D3DFORMATtoTbColorFormat( const D3DFORMAT& format )
    {
        TbColorFormat result = TB_COLOR_FORMAT_UNKNOWN;
        switch( format )
        {
        case  D3DFMT_A8R8G8B8:
            result = TB_COLOR_FORMAT_A8R8G8B8;
            break;
        case  D3DFMT_X8R8G8B8:
            result = TB_COLOR_FORMAT_X8R8G8B8;
            break;
        };
        return result;
    }

    /*! 
     * TbRenderViewParameter -> D3DPRESENT_PARAMETERS
     * @author Miyake Shunsuke
     * @since 2011.01.14
     */
    TbBool TbRenderViewParameterToD3DPRESENT_PARAMETERS( LPDIRECT3D9 direct3d , 
                                                         TbRenderDevice* device , 
                                                         TbUint32 adapterIndex , 
                                                         TbWindow* focusWindow , 
                                                         const TbRenderViewParameter& src , 
                                                         D3DPRESENT_PARAMETERS* dst , 
                                                         D3DPRESENT_PARAMETERS* fullscreenDst )
    {
        if( direct3d ){
            HRESULT result = S_OK;

            //ディスプレイモードチェック
            D3DDISPLAYMODE displayMode;
            if(FAILED(direct3d->GetAdapterDisplayMode(adapterIndex,&displayMode))){
                return E_FAIL;
            }

            dst->BackBufferWidth = src.backBufferWidth;
            dst->BackBufferHeight = src.backBufferHeight;
            dst->BackBufferFormat = TbColorFormatToD3DFORMAT( src.backBufferFormat );
            dst->BackBufferCount = src.backBufferNum; // 0 は 1 として処理される
            dst->MultiSampleType = TbMultiSampleTypeToD3DMULTISAMPLE_TYPE( src.multiSampleType );
            dst->MultiSampleQuality = src.multiSampleQuality;
            dst->SwapEffect = D3DSWAPEFFECT_DISCARD;
            dst->hDeviceWindow = ( focusWindow ) ? nullptr : src.window->GetDepend().GetWindowHandle(); // CreateDeviceのhFocusWindowを利用する
            dst->Windowed = true;
            dst->EnableAutoDepthStencil = src.enableCreateDepthStencil;
            dst->AutoDepthStencilFormat = TbColorFormatToD3DFORMAT( src.autoDepthStencilsFormat );
            dst->Flags = 0;
            dst->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            
            if( fullscreenDst )
            {
                *fullscreenDst = *dst;
                fullscreenDst->Windowed = false;
                // フルスクリーンの時はディスプレイの設定を利用する
                TbDisplayMode displayMode;
                if( TB_VERIFY( device->GetCurrentDisplayMode( 0 , displayMode ) ) )
                {
                    fullscreenDst->BackBufferWidth  = displayMode.width;
                    fullscreenDst->BackBufferHeight = displayMode.height;
                    fullscreenDst->BackBufferFormat = TbColorFormatToD3DFORMAT(displayMode.format);
                    fullscreenDst->FullScreen_RefreshRateInHz = displayMode.refreshRate;
                    fullscreenDst->BackBufferCount = 1;
                }
            }

            //src.isFullscreen ? D3DPRESENT_INTERVAL_IMMEDIATE : 

            // バックバッファフォーマットチェック
            result = direct3d->CheckDeviceType( adapterIndex , D3DDEVTYPE_HAL , displayMode.Format , dst->BackBufferFormat , dst->Windowed );
            if(FAILED(result)){
                return TB_FALSE;
            }

            // 深度ステンシルフォーマットチェック
            if( dst->EnableAutoDepthStencil ){
                result = direct3d->CheckDeviceFormat( adapterIndex , D3DDEVTYPE_HAL , displayMode.Format , D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , dst->AutoDepthStencilFormat );
                if(FAILED(result)){
                    return TB_FALSE;
                }
            }

            return TB_TRUE;
        }
        return TB_FALSE;
    }

    /*!
     * D3DPRESENT_PARAMETERS -> TbRenderViewParameter
     * @author Miyake Shunsuke
     * @since 2011.03.20
     */
    TbBool D3DPRESENT_PARAMETERSToTbRenderViewParameter( TbRenderViewParameter& dst , const D3DPRESENT_PARAMETERS& src )
    {
        dst.backBufferWidth = src.BackBufferWidth;
        dst.backBufferHeight = src.BackBufferHeight;
        dst.backBufferFormat = D3DFORMATtoTbColorFormat( src.BackBufferFormat );
        dst.backBufferNum = src.BackBufferCount;
        dst.multiSampleType = D3DMULTISAMPLE_TYPEToTbMultiSampleType( src.MultiSampleType );
        dst.multiSampleQuality = src.MultiSampleQuality;
        dst.window = src.hDeviceWindow ? TbWindowManager::GetInstance().GetWindow( reinterpret_cast<TbWindowId>(src.hDeviceWindow) ) : nullptr;
        dst.isFullscreen = src.Windowed;
        dst.enableCreateDepthStencil = src.EnableAutoDepthStencil;
        dst.autoDepthStencilsFormat = D3DFORMATtoTbColorFormat( src.AutoDepthStencilFormat );
        dst.flags = 0;
        return TB_TRUE;
    }

    /*!
     * TbTextureFilterMode -> D3DTEXTUREFILTERTYPE
     * @author Miyake Shunsuke
     * @since 2011.04.01
     */
    TbBool TbTextureFilterModeToD3DTEXTUREFILTERTYPE( D3DTEXTUREFILTERTYPE& dst , const TbTextureFilterMode& src )
    {
        static const D3DTEXTUREFILTERTYPE CONVERT_TABLE[] = {
            D3DTEXF_POINT , 
            D3DTEXF_LINEAR ,
        };
        TbBool isValid = ( TB_TEXTURE_FILTER_MODE_BEGIN <= src && src < TB_TEXTURE_FILTER_MODE_END );
        if( isValid ){
            dst = CONVERT_TABLE[ src ];
        }
        return ( isValid );
    }

    /*!
     * TbTextureAddressMode -> D3DTEXTUREADDRESS
     * @author Miyake Shunsuke
     * @since 2011.04.04
     */
    TbBool TbTextureAddressModeToD3DTEXTUREADDRESS( D3DTEXTUREADDRESS& dst , const TbTextureAddressMode& src )
    {
        static const D3DTEXTUREADDRESS CONVERT_TABLE[] = {
            D3DTADDRESS_WRAP , 
            D3DTADDRESS_CLAMP ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_TEXTURE_ADDRESS_MODE_MAX );
        TbBool isValid = ( TB_TEXTURE_ADDRESS_MODE_BEGIN <= src && src < TB_TEXTURE_ADDRESS_MODE_END );
        TB_ASSERT(isValid);
        if( isValid ){
            dst = CONVERT_TABLE[ src ];
        }
        return ( isValid );
    }

    /*!
     * IDirect3DSurface9から横幅と縦幅を取得
     * @author Miyake Shunsuke
     * @since 2011.05.30
     */
    TbBool TbGetSizeFromIDirect3DSurface9( TB_OUT TbUint32& width , TB_OUT TbUint32& height , IDirect3DSurface9* surface )
    {
        if( surface ){
            D3DSURFACE_DESC desc;
            surface->GetDesc( &desc );
            width = desc.Width;
            height = desc.Height;
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    /*!
     * IDirect3DTexture9から横幅と縦幅を取得
     * @author Miyake Shunsuke
     * @since 2011.05.30
     */
    TbBool TbGetSizeFromIDirect3DTexture9( TB_OUT TbUint32& width , TB_OUT TbUint32& height , IDirect3DTexture9* texture , TbUint32 level )
    {
        if( texture ){
            D3DSURFACE_DESC desc;
            texture->GetLevelDesc( level , &desc );
            width = desc.Width;
            height = desc.Height;
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    /*!
     * IDirect3DSurface9からフォーマット取得
     * @author Miyake Shunsuke
     * @since 2011.10.30
     */
    TbBool TbGetColorFormatFromIDirect3DSurface9( TB_OUT TbColorFormat& format , IDirect3DSurface9* surface )
    {
        if( surface ){
            D3DSURFACE_DESC desc;
            surface->GetDesc( &desc );
            format = TbD3DFORMATToColorFormat( desc.Format );
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    /*!
     * IDirect3DTexture9からカラーフォーマット取得
     * @author Miyake Shunsuke
     * @since 2011.05.30
     */
    TbBool TbGetColorFormatFromIDirect3DTexture9( TB_OUT TbColorFormat& format , IDirect3DTexture9* texture , TbUint32 level )
    {
        if( texture ){
            D3DSURFACE_DESC desc;
            texture->GetLevelDesc( level , &desc );
            format = TbD3DFORMATToColorFormat( desc.Format );
            return TB_TRUE;
        }
        return TB_FALSE;
    }


    /*!
     * IDirect3DTexture9からサーフェイス情報を取得
     * @author Miyake Shunsuke
     * @since 2011.05.30
     */
    TbBool TbGetSurfaceInfoFromIDirect3DTexture9( TB_OUT TbUint32& width , TB_OUT TbUint32& height , TB_OUT TbColorFormat& format , IDirect3DTexture9* texture , TbUint32 level )
    {
        if( texture ){
            D3DSURFACE_DESC desc;
            texture->GetLevelDesc( level , &desc );
            width = desc.Width;
            height = desc.Height;
            format = TbD3DFORMATToColorFormat( desc.Format );
            return TB_TRUE;
        }
        return TB_FALSE;
    }
    

}

#endif