/*!
 * @file
 * @brief GX2用Utiluty関数
 */
 
#ifndef _INCLUDED_TB_RENDER_UTIL_GFX_H_
#define _INCLUDED_TB_RENDER_UTIL_GFX_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>
#include "base/system/tb_assert.h"

namespace toybox
{

#if TB_3DAPI_IS_GFX

    inline nn::gfx::ShaderStage TbShaderTypeToGfx(TbShaderType type)
    {
        static const nn::gfx::ShaderStage TABLE[] =
        {
            nn::gfx::ShaderStage_Vertex ,
            nn::gfx::ShaderStage_Pixel  ,
            nn::gfx::ShaderStage_Geometry
        };
        TbSint32 index = type - TB_SHADER_TYPE_BEGIN;
        if (0 <= index && index < TB_ARRAY_COUNT_OF(TABLE)) {
            return TABLE[index];
        }
        return nn::gfx::ShaderStage_End;
    }

    // toybox用カラーフォーマットから3DAPIに適したフォーマットに変換
    inline TbColorFormat TbGetOptimizedColorFormat(const TbColorFormat src)
    {
        static const TbColorFormat CONVERT_TABLE[] =
        {
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_UNKNOWN

            TB_COLOR_FORMAT_A4G4B4R4 , // TB_COLOR_FORMAT_A4G4B4R4
            TB_COLOR_FORMAT_R5G6B5   , // TB_COLOR_FORMAT_R5G6B5

            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_R8G8B8
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_B8G8R8

            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_A8R8G8B8
            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_B8G8R8A8
            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_A8B8G8R8
            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_X8R8G8B8

            TB_COLOR_FORMAT_A32R32G32B32 , // TB_COLOR_FORMAT_A32R32G32B32

            TB_COLOR_FORMAT_D16 , // TB_COLOR_FORMAT_D16
            TB_COLOR_FORMAT_D32 , // TB_COLOR_FORMAT_D32
            TB_COLOR_FORMAT_D24S8 , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM);
        if (TB_COLOR_FORMAT_BEGIN <= src && src < TB_COLOR_FORMAT_END) {
            return CONVERT_TABLE[src - TB_COLOR_FORMAT_BEGIN];
        }
        return TB_COLOR_FORMAT_UNKNOWN;
    }

    // toybox用カラーフォーマットをGX2SurfaceFormatに変換
    inline TbBool TbColorFormatToImageFormat(nn::gfx::ImageFormat* dst, const TbColorFormat mode)
    {
        static const nn::gfx::ImageFormat CONVERT_TABLE[] =
        {
            nn::gfx::ImageFormat_Undefined  ,   // D3DFMT_UNKNOWN

            nn::gfx::ImageFormat_R4_G4_B4_A4_Unorm , // TB_COLOR_FORMAT_A4G4B4R4
            nn::gfx::ImageFormat_R5_G6_B5_Unorm    , // TB_COLOR_FORMAT_R5G6B5

            nn::gfx::ImageFormat_Undefined    ,   // TB_COLOR_FORMAT_R8G8B8
            nn::gfx::ImageFormat_Undefined    ,   // TB_COLOR_FORMAT_B8G8R8

            nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm   ,  // TB_COLOR_FORMAT_A8R8G8B8
            nn::gfx::ImageFormat_B8_G8_R8_A8_Unorm   ,  // TB_COLOR_FORMAT_B8G8R8A8
            nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm   ,  // TB_COLOR_FORMAT_A8B8G8R8
            nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm ,  // TB_COLOR_FORMAT_X8R8G8B8
            nn::gfx::ImageFormat_R32_G32_B32_A32_Float , // TB_COLOR_FORMAT_A32R32G32B32

            nn::gfx::ImageFormat_D16_Unorm , // TB_COLOR_FORMAT_D16
            nn::gfx::ImageFormat_D32_Float , // TB_COLOR_FORMAT_D32
            nn::gfx::ImageFormat_D24_Unorm_S8_Uint  , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM);
        if (TB_COLOR_FORMAT_BEGIN <= mode && mode < TB_COLOR_FORMAT_END) {
            *dst = CONVERT_TABLE[mode - TB_COLOR_FORMAT_BEGIN];
            if (TB_VERIFY(nn::gfx::ImageFormat_Undefined != *dst)) {
                return TB_TRUE;
            }
        }
        *dst = nn::gfx::ImageFormat_Undefined;
        TB_ASSERT(!"Invalid mode");
        return TB_FALSE;
    }

 
    // toybox用フィルタモードをGfx用に変換
    inline TbBool TbTextureFilterModeToGfx( nn::gfx::FilterMode* dst , 
                                            const TbTextureFilterMode minFilterMode , 
                                            const TbTextureFilterMode magFilterMode )
    {
        static const nn::gfx::detail::FilterModeBit CONVERT_TABLE_MIN[] = {
            nn::gfx::detail::FilterModeBit_MinPoint ,
            nn::gfx::detail::FilterModeBit_MinLinear ,
        };
        static const nn::gfx::detail::FilterModeBit CONVERT_TABLE_MAG[] = {
            nn::gfx::detail::FilterModeBit_MagPoint ,
            nn::gfx::detail::FilterModeBit_MagLinear ,
        };
        static const nn::gfx::detail::FilterModeBit CONVERT_TABLE_MIP[] = {
            nn::gfx::detail::FilterModeBit_MipPoint ,
            nn::gfx::detail::FilterModeBit_MipLinear ,
        };
        TbBool isValid = ( TB_TEXTURE_FILTER_MODE_BEGIN <= minFilterMode && minFilterMode < TB_TEXTURE_FILTER_MODE_END );
        TB_ASSERT(isValid);
        isValid = (TB_TEXTURE_FILTER_MODE_BEGIN <= magFilterMode && magFilterMode < TB_TEXTURE_FILTER_MODE_END);
        TB_ASSERT(isValid);
        *dst = static_cast<nn::gfx::FilterMode>( CONVERT_TABLE_MIN[minFilterMode - TB_TEXTURE_FILTER_MODE_BEGIN ] |
                    CONVERT_TABLE_MAG[magFilterMode - TB_TEXTURE_FILTER_MODE_BEGIN] |
                    CONVERT_TABLE_MIP[minFilterMode - TB_TEXTURE_FILTER_MODE_BEGIN] );
        return isValid;
    }
    
    // toybox用アドレスモードをGfx用に変換
    inline TbBool TbTextureAddressModeToGfx(nn::gfx::TextureAddressMode *dst, const TbTextureAddressMode& mode)
    {
        static const nn::gfx::TextureAddressMode CONVERT_TABLE[] = {
            nn::gfx::TextureAddressMode_Repeat ,
            nn::gfx::TextureAddressMode_ClampToEdge// 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_TEXTURE_ADDRESS_MODE_MAX );
        TbBool isValid = ( TB_TEXTURE_ADDRESS_MODE_BEGIN <= mode && mode < TB_TEXTURE_ADDRESS_MODE_END );
        TB_ASSERT(isValid);
        if (isValid) {
            *dst = CONVERT_TABLE[mode - TB_TEXTURE_ADDRESS_MODE_BEGIN];
        }
        return isValid;
    }

    // toybox用ブレンド係数 -> Gfx
    inline TbBool TbBlendFactorToGfx(nn::gfx::BlendFactor* dst , const TbBlendFactor& src )
    {
        static const nn::gfx::BlendFactor CONVERT_TABLE[] =
        {
            nn::gfx::BlendFactor_Zero ,
            nn::gfx::BlendFactor_One ,
            nn::gfx::BlendFactor_SourceColor ,
            nn::gfx::BlendFactor_OneMinusSourceColor ,
            nn::gfx::BlendFactor_DestinationColor ,
            nn::gfx::BlendFactor_OneMinusDestinationColor ,
            nn::gfx::BlendFactor_SourceAlpha ,
            nn::gfx::BlendFactor_OneMinusSourceAlpha ,
            nn::gfx::BlendFactor_DestinationAlpha ,
            nn::gfx::BlendFactor_OneMinusDestinationAlpha ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_FACTOR_MAX );
        if( TB_BLEND_FACTOR_BEGIN <= src && src < TB_BLEND_FACTOR_END ){
            *dst = CONVERT_TABLE[src-TB_BLEND_FACTOR_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }       
        
    // toybox用ブレンド演算子 -> Gfx
    inline TbBool TbBlendOpToGfx(nn::gfx::BlendFunction* dst , const TbBlendOp& src )
    {
        static const nn::gfx::BlendFunction CONVERT_TABLE[] =
        {
            nn::gfx::BlendFunction_Add ,
            nn::gfx::BlendFunction_Subtract ,
            nn::gfx::BlendFunction_ReverseSubtract ,
            nn::gfx::BlendFunction_Min ,
            nn::gfx::BlendFunction_Max ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_OP_MAX );
        if( TB_BLEND_OP_BEGIN <= src && src < TB_BLEND_OP_END ){
            *dst = CONVERT_TABLE[src-TB_BLEND_OP_BEGIN];
            return TB_TRUE;
        }
        TB_ASSERT(!"Invalid TbBlendOp");
        *dst = nn::gfx::BlendFunction_Add;
        return TB_FALSE;
    }

    // toybox用比較関数 -> Gfx
    inline TbBool TbCmpFuncToGfx( nn::gfx::ComparisonFunction* dst , const TbCmpFunc func )
    {
        static const nn::gfx::ComparisonFunction CONVERT_TABLE[] =
        {
            nn::gfx::ComparisonFunction_Never ,
            nn::gfx::ComparisonFunction_Less ,
            nn::gfx::ComparisonFunction_Equal ,
            nn::gfx::ComparisonFunction_LessEqual ,
            nn::gfx::ComparisonFunction_Greater ,
            nn::gfx::ComparisonFunction_NotEqual ,
            nn::gfx::ComparisonFunction_GreaterEqual ,
            nn::gfx::ComparisonFunction_Always
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_CMP_FUNC_MAX );
        if( TB_CMP_FUNC_BEGIN <= func && func < TB_CMP_FUNC_END ){
            *dst = CONVERT_TABLE[func-TB_CMP_FUNC_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用ステンシル関数 -> Gfx
    inline TbBool TbStencilOpToGfx( nn::gfx::StencilOperation* dst , const TbStencilOp op )
    {
        static const nn::gfx::StencilOperation CONVERT_TABLE[] =
        {
            nn::gfx::StencilOperation_Keep ,
            nn::gfx::StencilOperation_Zero ,
            nn::gfx::StencilOperation_Replace ,
            nn::gfx::StencilOperation_IncrementWrap ,
            nn::gfx::StencilOperation_DecrementWrap ,
            nn::gfx::StencilOperation_Invert ,
            nn::gfx::StencilOperation_IncrementClamp ,
            nn::gfx::StencilOperation_DecrementClamp ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_STENCIL_OP_MAX );
        if( TB_STENCIL_OP_BEGIN <= op && op < TB_STENCIL_OP_END ){
            *dst = CONVERT_TABLE[ op - TB_STENCIL_OP_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }
       
    // toybox用カリングモード -> Gfx
    inline TbBool TbCullModeToGfx(nn::gfx::CullMode* dst, TbCullMode mode)
    {
        static const nn::gfx::CullMode CONVERT_TABLE[] =
        {
            nn::gfx::CullMode_None , 
            nn::gfx::CullMode_Front ,
            nn::gfx::CullMode_Back ,
        };
        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_CULL_MODE_MAX);
        if (TB_CULL_MODE_BEGIN <= mode && mode < TB_CULL_MODE_END) {
            *dst = CONVERT_TABLE[mode - TB_CULL_MODE_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用FillMode -> Gfx
    inline TbBool TbFillModeToGfx(nn::gfx::FillMode* dst, TbFillMode mode)
    {
        static const nn::gfx::FillMode CONVERT_TABLE[] =
        {
            nn::gfx::FillMode_Solid ,
            nn::gfx::FillMode_Wireframe ,
            nn::gfx::FillMode_Solid
        };
        TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_FILL_MODE_MAX);
        if (TB_FILL_MODE_BEGIN <= mode && mode < TB_FILL_MODE_END) {
            *dst = CONVERT_TABLE[mode - TB_FILL_MODE_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

}
#endif

#endif
